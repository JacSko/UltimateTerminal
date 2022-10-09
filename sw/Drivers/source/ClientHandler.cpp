/* =============================
 *   Includes of project headers
 * =============================*/
#include "ClientHandler.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <sys/socket.h>

/* namespace wrapper around system function to allow replace in unit tests */

namespace system_call
{
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return ::send(socket, message, length, flags);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return ::recv(socket, buffer, length, flags);
}
}

namespace Drivers
{
namespace SocketServer
{

constexpr char SERVER_DELIMITER = '\n';

ClientHandler::ClientHandler(int client_id, DataMode mode, ClientHandlerListener* listener):
m_worker(std::bind(&ClientHandler::execute, this), getThreadName(client_id)),
m_client_id(client_id),
m_listener(listener),
m_recv_buffer(SOCKET_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_mode(mode)
{
   m_write_buffer.reserve(SOCKET_MAX_PAYLOAD_LENGTH);
}

bool ClientHandler::start(uint32_t timeout)
{
   return m_worker.start(timeout);
}

void ClientHandler::stop()
{
   m_worker.stop();
}


bool ClientHandler::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;

   m_write_buffer.clear();
   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);

   ssize_t bytes_to_write = m_write_buffer.size();
   if (bytes_to_write <= SOCKET_MAX_PAYLOAD_LENGTH)
   {
      ssize_t bytes_written = 0;
      ssize_t current_write = 0;
      result = true;
      while (bytes_to_write > 0)
      {
         current_write = sendData(m_client_id, m_write_buffer.data() + bytes_written, bytes_to_write, 0);
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

int ClientHandler::getClientID()
{
   return m_client_id;
}

void ClientHandler::execute()
{
   switch(m_mode)
   {
   case DataMode::NEW_LINE_DELIMITER:
      startDelimiterMode();
      break;
   default:
      break;
   }
}

void ClientHandler::startDelimiterMode()
{
   while(m_worker.isRunning())
   {
      int recv_bytes = readData(m_client_id, m_recv_buffer.data() + m_recv_buffer_idx, SOCKET_MAX_PAYLOAD_LENGTH, 0);
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
      else if (recv_bytes == 0)
      {
         /* client disconnected, break from main loop and wait for object destroy or restart */
         if (m_listener) m_listener->onClientEvent(m_client_id, ClientEvent::CLIENT_DISCONNECTED, {}, 0);
         break;
      }
   }
}

const std::string ClientHandler::getThreadName(int client_id)
{
   return std::string("CLIENT") + std::to_string(client_id);
}

ssize_t ClientHandler::sendData(int socket, const void *message, size_t length, int flags)
{
   return system_call::send(socket, message, length, flags);
}

ssize_t ClientHandler::readData(int socket, void *buffer, size_t length, int flags)
{
   return system_call::recv(socket, buffer, length, flags);
}

ClientHandler::~ClientHandler()
{
   m_worker.stop();
}

}
}
