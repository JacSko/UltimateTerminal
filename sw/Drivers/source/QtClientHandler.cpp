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
constexpr uint32_t CLIENT_RECEIVE_TIMEOUT = 500;

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
   std::unique_lock<std::mutex> lock(m_starting_mutex);
   m_state = State::STARTING;
   m_worker.start(timeout);
   m_starting_cond_var.wait_for(lock, std::chrono::milliseconds(500), [&](){return (m_state == State::RUNNING) || (m_state == State::IDLE); });
   return m_state == State::RUNNING;
}

void QtClientHandler::stop()
{
   m_worker.stop();
}


bool QtClientHandler::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;

   m_write_buffer.clear();
   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);

   ssize_t bytes_to_write = m_write_buffer.size();
   if (m_socket && bytes_to_write <= SOCKET_MAX_PAYLOAD_LENGTH)
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
            result = false;
            break;
         }
         bytes_to_write -= bytes_written;
      }
   }
   return result;
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
                  auto it = std::find(m_recv_buffer.begin(), (m_recv_buffer.begin() + m_recv_buffer_idx), (uint8_t)SERVER_DELIMITER);
                  if (it != (m_recv_buffer.begin() + m_recv_buffer_idx))
                  {
                     it++; //include the found newline too
                     if (m_listener) m_listener->onClientEvent(m_client_id, ClientEvent::DATA_RECEIVED, std::vector<uint8_t>(m_recv_buffer.begin(), it), std::distance(m_recv_buffer.begin(), it));
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
            if (m_listener) m_listener->onClientEvent(m_client_id, ClientEvent::CLIENT_DISCONNECTED, {}, 0);
            break;
         }
         else
         {
         }
      }
   }
   else
   {
   }

   m_socket->abort();
   delete m_socket;
   m_socket = nullptr;
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
