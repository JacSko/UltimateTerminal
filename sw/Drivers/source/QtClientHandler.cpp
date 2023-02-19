/* =============================
 *   Includes of project headers
 * =============================*/
#include "QtClientHandler.h"
/* =============================
 *   Includes of common headers
 * =============================*/

namespace Drivers
{
namespace SocketServer
{

constexpr char SERVER_DELIMITER = '\n';
constexpr uint32_t CLIENT_RECEIVE_TIMEOUT = 50;

QtClientHandler::QtClientHandler(int client_id, DataMode mode, ClientHandlerListener* listener):
m_socket(nullptr),
m_worker(std::bind(&QtClientHandler::execute, this), getThreadName(client_id)),
m_client_id(client_id),
m_listener(listener),
m_recv_buffer(SOCKET_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_mode(mode)
{
   m_write_buffer.reserve(SOCKET_MAX_PAYLOAD_LENGTH);
}

bool QtClientHandler::start(uint32_t timeout)
{
   UT_Stdout_Log(SOCK_DRV, LOW, "starting thread for client %u", m_client_id);
   std::unique_lock<std::mutex> lock(m_starting_mutex);
   m_state = State::STARTING;
   m_worker.start(timeout);
   m_starting_cond_var.wait_for(lock, std::chrono::milliseconds(500), [&](){return (m_state == State::RUNNING) || (m_state == State::IDLE); });
   return m_state == State::RUNNING;
}

void QtClientHandler::stop()
{
   UT_Stdout_Log(SOCK_DRV, LOW, "stopping thread for client %u", m_client_id);
   m_state = State::STOPPING;
   m_worker.stop();
}


bool QtClientHandler::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   UT_Stdout_Log(SOCK_DRV, LOW, "writing %d for client %u", size, m_client_id);

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
void QtClientHandler::writePendingData()
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
int QtClientHandler::getClientID()
{
   return m_client_id;
}

void QtClientHandler::execute()
{
   {
      std::lock_guard<std::mutex> lock(m_starting_mutex);
      m_socket = new QTcpSocket();
      m_state = State::RUNNING;
      m_starting_cond_var.notify_all();
   }

   if(m_socket->setSocketDescriptor(m_client_id))
   {
      m_mode == DataMode::NEW_LINE_DELIMITER? startDelimiterMode() : startHeaderMode();
   }
   else
   {
      UT_Stdout_Log(SOCK_DRV, LOW, "cannot setSocketDescriptor");
   }

   m_socket->abort();
   delete m_socket;
   m_socket = nullptr;
}
void QtClientHandler::startDelimiterMode()
{
   UT_Stdout_Log(SOCK_DRV, HIGH, "%s", __func__);

   while(m_state == State::RUNNING)
   {
      writePendingData();
      if (m_socket->waitForReadyRead(CLIENT_RECEIVE_TIMEOUT))
      {
         bool bytes_available = true;
         while (bytes_available)
         {
            int recv_bytes = m_socket->read((char*)m_recv_buffer.data() + m_recv_buffer_idx, SOCKET_MAX_PAYLOAD_LENGTH);
            if (recv_bytes > 0)
            {
               m_recv_buffer_idx += recv_bytes;
               bool is_next_new_line = true;
               do
               {
                  auto it = std::find(m_recv_buffer.begin(), (m_recv_buffer.begin() + m_recv_buffer_idx), (uint8_t)SERVER_DELIMITER);
                  if (it != (m_recv_buffer.begin() + m_recv_buffer_idx))
                  {
                     it++; //include the found newline too
                     UT_Stdout_Log(SOCK_DRV, HIGH, "received %u bytes from server", (uint32_t)std::distance(m_recv_buffer.begin(), it));
                     m_listener->onClientEvent(m_client_id, ClientEvent::DATA_RECEIVED, std::vector<uint8_t>(m_recv_buffer.begin(), it), std::distance(m_recv_buffer.begin(), it));
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
         UT_Stdout_Log(SOCK_DRV, INFO, "remote server disconnected");
         m_listener->onClientEvent(m_client_id, ClientEvent::CLIENT_DISCONNECTED, {}, 0);
         break;
      }
      UT_Log_If(m_state == State::STOPPING, SOCK_DRV, HIGH, "Disconnect requested, exiting read loop");
   }
}
void QtClientHandler::startHeaderMode()
{
   std::vector<uint8_t> header(HeaderHandler::HEADER_SIZE, 0x00);
   UT_Stdout_Log(SOCK_DRV, HIGH, "%s", __func__);

   while(m_state == State::RUNNING)
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
                  m_listener->onClientEvent(m_client_id, ClientEvent::DATA_RECEIVED, m_recv_buffer, payload_size);
               }
               else if (recv_bytes == 0)
               {
                  /* server disconnected, break from main loop and wait for new connection */
                  m_listener->onClientEvent(m_client_id, ClientEvent::CLIENT_DISCONNECTED, {}, 0);
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
         UT_Stdout_Log(SOCK_DRV, INFO, "remote server disconnected");
         m_listener->onClientEvent(m_client_id, ClientEvent::CLIENT_DISCONNECTED, {}, 0);
         break;
      }
      UT_Log_If(m_state == State::STOPPING, SOCK_DRV, HIGH, "Disconnect requested, exiting read loop");
   }
}

const std::string QtClientHandler::getThreadName(int client_id)
{
   return std::string("CLIENT") + std::to_string(client_id);
}

QtClientHandler::~QtClientHandler()
{
   m_worker.stop();
}

}
}
