/* =============================
 *   Includes of project headers
 * =============================*/
#include "QtSocketClient.h"
#include "Logger.h"
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
constexpr uint32_t CLIENT_RECEIVE_TIMEOUT = 500;
constexpr char CLIENT_DELIMITER = '\n';

QtSocketClient::QtSocketClient():
m_worker(std::bind(&QtSocketClient::receivingThread, this), "CLIENT_WORKER"),
m_mode(DataMode::NEW_LINE_DELIMITER),
m_recv_buffer(SOCKET_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_listeners {},
m_state(State::IDLE)
{
   m_write_buffer.reserve(SOCKET_MAX_PAYLOAD_LENGTH);
   m_worker.start(CLIENT_THREAD_START_TIMEOUT);
}
bool QtSocketClient::connect(DataMode mode, std::string ip_address, uint16_t port)
{
   bool result = false;
   UT_Log(SOCK_DRV, INFO, "Connecting to %s:%u", m_ip_address.c_str(), m_port);

   if (!isConnected())
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_mode = mode;
      m_ip_address = ip_address;
      m_port = port;
      m_state = State::CONNECTION_REQUESTED;
      m_cond_var.notify_all();
      UT_Log(SOCK_DRV, HIGH, "Connection to %s:%u requested", m_ip_address.c_str(), m_port);
      if (m_cond_var.wait_for(lock, std::chrono::milliseconds(CLIENT_THREAD_START_TIMEOUT), [&]()->bool{return (m_state == State::CONNECTED) || (m_state == State::IDLE);}))
      {
         result = (m_state == State::CONNECTED);
         UT_Log(SOCK_DRV, LOW, "Connection to %s:%u - result %d", m_ip_address.c_str(), m_port, result);
      }
   }
   return result;
}
void QtSocketClient::disconnect()
{
   UT_Log(SOCK_DRV, INFO, "Disconnecting from %s:%u", m_ip_address.c_str(), m_port);

   std::lock_guard<std::mutex> lock(m_mutex);
   if (m_socket)
   {
      m_socket->abort();
   }
}
bool QtSocketClient::isConnected()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   return m_state == State::CONNECTED;
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

   UT_Log(SOCK_DRV, HIGH, "Writing %u bytes to %s:%u", data.size(), m_ip_address.c_str(), m_port);

   m_write_buffer.clear();
   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);

   ssize_t bytes_to_write = m_write_buffer.size();
   if (m_socket && (bytes_to_write <= SOCKET_MAX_PAYLOAD_LENGTH))
   {
      ssize_t bytes_written = 0;
      ssize_t current_write = 0;
      result = true;
      while (bytes_to_write > 0)
      {
         current_write = m_socket->write((char*)m_write_buffer.data() + bytes_written, bytes_to_write);
         if (current_write > 0)
         {
            bytes_written += current_write;
         }
         else
         {
            UT_Log(SOCK_DRV, HIGH, "Error writing to %u bytes to %s:%u", data.size(), m_ip_address.c_str(), m_port);
            result = false;
            break;
         }
         bytes_to_write -= bytes_written;
      }
   }
   return result;
}
void QtSocketClient::receivingThread()
{
   bool is_started = false;
   m_socket = new QTcpSocket();
   UT_Assert(m_socket);
   UT_Log(SOCK_DRV, HIGH, "Starting thread");

   while(m_worker.isRunning())
   {
      {
         std::unique_lock<std::mutex> lock(m_mutex);
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
         UT_Log(SOCK_DRV, LOW, "Starting connection to %s:%u", m_ip_address.c_str(), m_port);
         m_socket->connectToHost(QString(m_ip_address.c_str()), m_port);
         if (m_socket->waitForConnected(CLIENT_CONNECTION_TIMEOUT))
         {
            UT_Log(SOCK_DRV, INFO, "Connection to %s:%u successfull", m_ip_address.c_str(), m_port);
            {
               std::unique_lock<std::mutex> lock(m_mutex);
               m_state = State::CONNECTED;
               m_cond_var.notify_all();
            }

            while(m_worker.isRunning())
            {
               if (m_socket->waitForReadyRead(CLIENT_RECEIVE_TIMEOUT))
               {
                  int recv_bytes = m_socket->read((char*)m_recv_buffer.data() + m_recv_buffer_idx, SOCKET_MAX_PAYLOAD_LENGTH);
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
               }
               else if (m_socket->error() == QAbstractSocket::RemoteHostClosedError)
               {
                  UT_Log(SOCK_DRV, INFO, "remote server disconnected");
                  {
                     std::unique_lock<std::mutex> lock(m_mutex);
                     m_state = State::IDLE;
                     m_socket->abort();
                     m_cond_var.notify_all();
                  }
                  notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
                  break;
               }
               else
               {
                  UT_Log(SOCK_DRV, HIGH, "no new data");
               }
            }
         }
         else
         {
            UT_Log(SOCK_DRV, ERROR, "Connection to %s:%u error", m_ip_address.c_str(), m_port);
            std::unique_lock<std::mutex> lock(m_mutex);
            m_state = State::IDLE;
            m_socket->abort();
            m_cond_var.notify_all();
         }
      }
   }

   UT_Log(SOCK_DRV, HIGH, "Exiting main loop...");
   std::unique_lock<std::mutex> lock(m_mutex);
   m_socket->abort();
   m_state = State::IDLE;
   m_cond_var.notify_all();
   delete m_socket;
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
