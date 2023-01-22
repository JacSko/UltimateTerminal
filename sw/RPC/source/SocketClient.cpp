/* =============================
 *   Includes of project headers
 * =============================*/
#include "SocketClient.h"
#include "Logger.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>

/* namespace wrapper around system function to allow replace in unit tests */
namespace system_call
{

__attribute__((weak)) int socket(int domain, int type, int protocol)
{
   return ::socket(domain, type, protocol);
}
__attribute__((weak)) int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
   return ::setsockopt(socket, level, option_name, option_value, option_len);
}
__attribute__((weak)) uint32_t htons(uint32_t port)
{
   return ::htons(port);
}
__attribute__((weak)) int inet_pton(int af, const char * src, void * dst)
{
   return ::inet_pton(af, src, dst);
}
__attribute__((weak)) int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
   return ::connect(sockfd, addr, addrlen);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return ::send(socket, message, length, flags);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return ::recv(socket, buffer, length, flags);
}
__attribute__((weak)) int close (int fd)
{
   return ::close(fd);
}

}

namespace Drivers
{
namespace SocketClient
{

constexpr uint32_t CLIENT_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t CLIENT_RECEIVE_TIMEOUT = 500;
constexpr char CLIENT_DELIMITER = '\n';

std::unique_ptr<ISocketClient> ISocketClient::create()
{
   return std::unique_ptr<ISocketClient>(new SocketClient());
}

SocketClient::SocketClient():
m_worker(std::bind(&SocketClient::receivingThread, this), "CLIENT_WORKER"),
m_sock_fd(-1),
m_recv_buffer(SOCKET_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_listeners {}
{
   m_write_buffer.reserve(SOCKET_MAX_PAYLOAD_LENGTH);
}
bool SocketClient::connect(DataMode mode, std::string ip_address, uint16_t port)
{
   bool result = false;
   struct sockaddr_in serv_addr;

   if (!isConnected())
   {
      m_mode = mode;
      m_ip_address = ip_address;
      m_port = port;

      m_sock_fd = system_call::socket(AF_INET, SOCK_STREAM, 0);
      if (m_sock_fd >= 0)
      {
         struct timeval tv = {};
         tv.tv_usec = CLIENT_RECEIVE_TIMEOUT * 1000; /* convert milliseconds to microseconds*/
         if (system_call::setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval)) != -1)
         {
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = system_call::htons(m_port);

            if (system_call::inet_pton(AF_INET, m_ip_address.c_str(), &serv_addr.sin_addr) > 0)
            {
               if (system_call::connect(m_sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
               {
                  result = true;
                  UT_Log(SOCK_DRV, INFO, "Connection to %s:%u successfull", m_ip_address.c_str(), m_port);
                  m_worker.start(CLIENT_THREAD_START_TIMEOUT);
               }
               else
               {
                  UT_Log(SOCK_DRV, ERROR, "cannot connect to %s:%u", m_ip_address.c_str(), m_port);
                  system_call::close(m_sock_fd);
                  m_sock_fd = -1;
               }
            }
            else
            {
               UT_Log(SOCK_DRV, ERROR, "cannot convert ip address %s:%u", m_ip_address.c_str(), m_port);
               system_call::close(m_sock_fd);
               m_sock_fd = -1;
            }
         }
         else
         {
            UT_Log(SOCK_DRV, ERROR, "cannot set receive timeout");
            system_call::close(m_sock_fd);
            m_sock_fd = -1;
         }
      }
      else
      {
         UT_Log(SOCK_DRV, ERROR, "cannot create socket %s:%u", m_ip_address.c_str(), m_port);
      }
   }
   else
   {
      UT_Log(SOCK_DRV, ERROR, "already connected to %s:%u", m_ip_address.c_str(), m_port);
   }

   return result;
}
void SocketClient::disconnect()
{
   if (m_worker.isRunning())
   {
      m_worker.stop();
   }
   system_call::close(m_sock_fd);
   m_sock_fd = -1;
}
bool SocketClient::isConnected()
{
   return m_worker.isRunning();
}
void SocketClient::addListener(ClientListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void SocketClient::removeListener(ClientListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}
bool SocketClient::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;

   m_write_buffer.clear();
   if (m_mode == DataMode::PAYLOAD_HEADER)
   {
      m_header_handler.preapreHeader(m_write_buffer, (uint32_t) size);
   }
   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);

   ssize_t bytes_to_write = m_write_buffer.size();
   if (bytes_to_write <= SOCKET_MAX_PAYLOAD_LENGTH)
   {
      ssize_t bytes_written = 0;
      ssize_t current_write = 0;
      result = true;
      while (bytes_to_write > 0)
      {
         current_write = system_call::send(m_sock_fd, m_write_buffer.data() + bytes_written, bytes_to_write, 0);
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
void SocketClient::receivingThread()
{
   switch(m_mode)
   {
   case DataMode::NEW_LINE_DELIMITER:
      startDelimiterMode();
      break;
   case DataMode::PAYLOAD_HEADER:
      startHeaderMode();
      break;
   default:
      break;
   }
}
void SocketClient::startDelimiterMode()
{
   while(m_worker.isRunning())
   {
      int recv_bytes = system_call::recv(m_sock_fd, m_recv_buffer.data() + m_recv_buffer_idx, SOCKET_MAX_PAYLOAD_LENGTH, 0);
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
         /* client disconnected, break from main loop and wait for object destroy or restart */
         notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
         break;
      }
   }
}

void SocketClient::startHeaderMode()
{
   std::vector<uint8_t> header(HeaderHandler::HEADER_SIZE, 0x00);
   while(m_worker.isRunning())
   {
      int recv_bytes = system_call::recv(m_sock_fd, header.data(), HeaderHandler::HEADER_SIZE, 0);
      if (recv_bytes == HeaderHandler::HEADER_SIZE)
      {
         uint32_t payload_size = m_header_handler.decodeMessageLength(header);
         m_recv_buffer.clear();
         m_recv_buffer.resize(payload_size);
         recv_bytes = system_call::recv(m_sock_fd, m_recv_buffer.data(), payload_size, 0);
         if (recv_bytes == payload_size)
         {
            notifyListeners(ClientEvent::SERVER_DATA_RECV, m_recv_buffer, payload_size);
         }
         else if (recv_bytes == 0)
         {
            /* server disconnected, break from main loop and wait for object destroy or restart */
            notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
            break;
         }
      }
      else if (recv_bytes == 0)
      {
         /* client disconnected, break from main loop and wait for object destroy or restart */
         notifyListeners(ClientEvent::SERVER_DISCONNECTED, {}, 0);
         break;
      }
   }
}
void SocketClient::notifyListeners(ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
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
SocketClient::~SocketClient()
{
   if (isConnected())
   {
      disconnect();
   }
}

}
}
