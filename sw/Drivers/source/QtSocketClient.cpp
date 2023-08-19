/* =============================
 *   Includes of project headers
 * =============================*/
#include "QtSocketClient.h"

/* =============================
 *   Includes of common headers
 * =============================*/
#include <unistd.h>
#include <algorithm>


namespace Drivers
{
namespace SocketClient
{

constexpr uint32_t CLIENT_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t CLIENT_CONNECTION_TIMEOUT = 500;
constexpr uint32_t CLIENT_RECEIVE_TIMEOUT = 50;
constexpr char CLIENT_DELIMITER = '\n';

std::unique_ptr<ISocketClient> ISocketClient::create(DataMode mode)
{
   return std::unique_ptr<ISocketClient>(new QtSocketClient(mode));
}

QtSocketClient::QtSocketClient(DataMode mode):
m_worker(std::bind(&QtSocketClient::receivingThread, this), "CLIENT_WORKER"),
m_state(State::IDLE),
m_mode(mode),
m_ip_address(""),
m_port(0),
m_recv_buffer(SOCKET_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_listeners {}
{
   m_write_buffer.reserve(SOCKET_MAX_PAYLOAD_LENGTH);
   m_worker.start(CLIENT_THREAD_START_TIMEOUT);
}
bool QtSocketClient::connect(std::string ip_address, uint16_t port)
{
   bool result = false;

   if (!isConnected())
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_ip_address = ip_address;
      m_port = port;
      m_state = State::CONNECTION_REQUESTED;
      m_cond_var.notify_all();
      if (m_cond_var.wait_for(lock, std::chrono::milliseconds(CLIENT_THREAD_START_TIMEOUT), [&]()->bool{return (m_state == State::CONNECTED) || (m_state == State::IDLE);}))
      {
         result = (m_state == State::CONNECTED);
      }
   }
   return result;
}
void QtSocketClient::disconnect()
{
   std::unique_lock<std::mutex> lock(m_mutex);
   if (m_state == State::CONNECTED)
   {
      m_state = State::DISCONNECTING;
      m_cond_var.wait_for(lock, std::chrono::milliseconds(5000), [&]()->bool{return m_state == State::IDLE;});
   }
}
bool QtSocketClient::isConnected()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   return m_state == State::CONNECTED || m_state == State::CONNECTION_REQUESTED;
}
void QtSocketClient::addListener(ClientListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void QtSocketClient::removeListener(ClientListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}
bool QtSocketClient::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;

   std::unique_lock<std::mutex> lock(m_write_buffer_mutex);
   m_write_buffer.clear();
   if (m_mode == DataMode::PAYLOAD_HEADER)
   {
      m_header_handler.preapreHeader(m_write_buffer, (uint32_t) size);
   }

   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);
   if (m_write_buffer_condvar.wait_for(lock, std::chrono::milliseconds(2000), [&](){return m_write_buffer.empty();}))
   {
      result = true;
   }
   return result;
}
void QtSocketClient::writePendingData()
{
   std::lock_guard<std::mutex> lock(m_write_buffer_mutex);
   if (!m_write_buffer.empty())
   {
      size_t bytes_to_write = m_write_buffer.size();
      size_t bytes_written = 0;
      size_t current_write = 0;

      if (m_socket && bytes_to_write <= SOCKET_MAX_PAYLOAD_LENGTH)
      {
         while (bytes_to_write > 0)
         {
            current_write = m_socket->write((char*)m_write_buffer.data() + bytes_written, bytes_to_write);
            if (current_write > 0)
            {
               bytes_written += current_write;
            }
            else
            {
               break;
            }
            bytes_to_write -= bytes_written;
         }
      }
      if (m_write_buffer.size() == bytes_written)
      {
         m_write_buffer.clear();
         m_write_buffer_condvar.notify_all();
      }
   }
}
void QtSocketClient::receivingThread()
{
   bool is_started = false;
   m_socket = new QTcpSocket();

   while(m_worker.isRunning())
   {
      {
         std::unique_lock<std::mutex> lock(m_mutex);
         is_started = false;
         m_cond_var.wait(lock);
         if (m_state == State::CONNECTION_REQUESTED)
         {
            is_started = true;
         }
         else if (m_state == State::DESTROYING)
         {
            break;
         }
      }

      if (is_started)
      {
         m_socket->connectToHost(QString(m_ip_address.c_str()), m_port);
         if (m_socket->waitForConnected(CLIENT_CONNECTION_TIMEOUT))
         {
            {
               std::unique_lock<std::mutex> lock(m_mutex);
               m_state = State::CONNECTED;
               m_cond_var.notify_all();
            }

            m_mode == DataMode::NEW_LINE_DELIMITER? startDelimiterMode() : startHeaderMode();
         }
         m_state = State::IDLE;
         m_socket->abort();
         m_cond_var.notify_all();
      }
   }

   m_cond_var.notify_all();
   delete m_socket;
}
void QtSocketClient::startDelimiterMode()
{
   while(m_state == State::CONNECTED)
   {
      writePendingData();
      if (m_socket->waitForReadyRead(CLIENT_RECEIVE_TIMEOUT))
      {
         bool bytes_available = true;
         while (bytes_available)
         {
            if (m_recv_buffer_idx == m_recv_buffer.size())
            {
               m_recv_buffer_idx = 0;
            }
            int recv_bytes = m_socket->read((char*)m_recv_buffer.data() + m_recv_buffer_idx, m_recv_buffer.size() - m_recv_buffer_idx);
            if (recv_bytes > 0)
            {
               m_recv_buffer_idx += recv_bytes;
               bool is_next_new_line = true;
               do
               {
                  auto it = std::find(m_recv_buffer.begin(), (m_recv_buffer.begin() + m_recv_buffer_idx), (uint8_t)CLIENT_DELIMITER);
                  if (it != (m_recv_buffer.begin() + m_recv_buffer_idx))
                  {
                     it++; //include the found newline too
                     notifyListeners(ClientEvent::SERVER_DATA_RECV, std::vector<uint8_t>(m_recv_buffer.begin(), it), std::distance(m_recv_buffer.begin(), it));
                     std::copy(it, m_recv_buffer.begin() + m_recv_buffer_idx, m_recv_buffer.begin());
                     m_recv_buffer_idx = std::distance(it, m_recv_buffer.begin() + m_recv_buffer_idx);
                  }
                  else
                  {
                     is_next_new_line = false;
                  }
               }
               while(is_next_new_line);
            }
            else if (recv_bytes == 0)
            {
               bytes_available = false;
            }
         }
      }
      else if (m_socket->error() == QAbstractSocket::RemoteHostClosedError)
      {
         notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
         break;
      }
   }
}
void QtSocketClient::startHeaderMode()
{
   std::vector<uint8_t> header(HeaderHandler::HEADER_SIZE, 0x00);

   while(m_state == State::CONNECTED)
   {
      writePendingData();
      if (m_socket->waitForReadyRead(CLIENT_RECEIVE_TIMEOUT))
      {
         bool bytes_available = true;
         while (bytes_available)
         {
            int recv_bytes = m_socket->read((char*)header.data(), HeaderHandler::HEADER_SIZE);
            if (recv_bytes > 0)
            {
               uint32_t payload_size = m_header_handler.decodeMessageLength(header);
               m_recv_buffer.clear();
               m_recv_buffer.resize(payload_size);
               m_recv_buffer_idx += recv_bytes;

               recv_bytes = m_socket->read((char*)m_recv_buffer.data(), payload_size);
               if (static_cast<uint32_t>(recv_bytes) == payload_size)
               {
                  notifyListeners(ClientEvent::SERVER_DATA_RECV, m_recv_buffer, payload_size);
               }
               else if (recv_bytes == 0)
               {
                  /* server disconnected, break from main loop and wait for new connection */
                  notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
                  break;
               }
            }
            else if (recv_bytes == 0)
            {
               bytes_available = false;
            }
         }
      }
      else if (m_socket->error() == QAbstractSocket::RemoteHostClosedError)
      {
         notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
         break;
      }
   }
}

void QtSocketClient::notifyListeners(ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   for (auto& listener : m_listeners)
   {
      if (listener)
      {
         listener->onClientEvent(ev, data, size);
      }
   }
}
QtSocketClient::~QtSocketClient()
{
   if (isConnected())
   {
      disconnect();
   }
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::DESTROYING;
   }
   m_cond_var.notify_all();
   m_worker.stop();
}

}
}
