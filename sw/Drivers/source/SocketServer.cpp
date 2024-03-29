/* =============================
 *   Includes of project headers
 * =============================*/
#include "SocketServer.h"
#include "Logger.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <string.h>

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
__attribute__((weak)) int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
   return ::bind(sockfd, addr, addrlen);
}
__attribute__((weak)) int listen(int sockfd, int backlog)
{
   return ::listen(sockfd, backlog);
}
__attribute__((weak)) int accept(int socket, struct sockaddr * address, socklen_t * address_len)
{
   return ::accept(socket, address, address_len);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return ::send(socket, message, length, flags);
}
__attribute__((weak)) int close (int fd)
{
   return ::close(fd);
}

}

namespace Drivers
{
namespace SocketServer
{

constexpr uint32_t SERVER_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t SERVER_RECEIVE_TIMEOUT = 500;

std::unique_ptr<ISocketServer> ISocketServer::create(DataMode mode)
{
   return std::unique_ptr<ISocketServer>(new SocketServer(mode));
}

SocketServer::SocketServer(DataMode mode):
m_listening_thread(std::bind(&SocketServer::listening_thread, this), "SOCK_LISTEN"),
m_working_thread(std::bind(&SocketServer::worker_thread, this), "SOCK_WORKER"),
m_server_fd(-1),
m_port(0),
m_max_clients(0),
m_mode(mode),
m_listeners {},
m_handlers {}
{
   if (!m_working_thread.start(SERVER_THREAD_START_TIMEOUT))
   {
      UT_Stdout_Log(SOCK_DRV, ERROR, "cannot start thread %s", m_working_thread.getThreadName().c_str());
   }
}
bool SocketServer::start(uint16_t port, uint8_t max_clients)
{
   bool result = false;
   m_port = port;
   m_max_clients = max_clients;

   if (port == 0)
   {
      UT_Stdout_Log(SOCK_DRV, ERROR, "Invalid port: %u", port);
      return false;
   }

   m_server_fd = system_call::socket(AF_INET, SOCK_STREAM, 0);
   UT_Stdout_Log_If(m_server_fd == -1, SOCK_DRV, ERROR, "[%d] cannot open server fd! error [%s]", m_port, strerror(errno));
   if (m_server_fd != -1)
   {
      struct timeval tv = {};
      tv.tv_usec = SERVER_RECEIVE_TIMEOUT * 1000; /* convert milliseconds to microseconds*/
      UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] setting sockopt with timeout %u", m_port, SERVER_RECEIVE_TIMEOUT);
      if (system_call::setsockopt(m_server_fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval)) != -1)
      {
         int opt = 1;
         UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] setting sockopt for IP and port reuse", m_port);
         if (system_call::setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != -1)
         {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(m_port);

            UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] Binding server to port", m_port);
            if (system_call::bind(m_server_fd, (struct sockaddr *)&address, sizeof(address)) != -1)
            {
               UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] starting server thread", m_port);
               result = m_listening_thread.start(SERVER_THREAD_START_TIMEOUT);
            }
         }
      }
   }

   UT_Stdout_Log_If(!result, SOCK_DRV, ERROR, "[%d] cannot open server", m_port);
   return result;
}

void SocketServer::stop()
{
   m_listening_thread.stop();
   m_working_thread.stop();

   if (m_server_fd != -1)
   {
      system_call::close(m_server_fd);
      m_server_fd = -1;
   }
   closeAllClients();
}
uint32_t SocketServer::clientsCount()
{
   std::lock_guard<std::mutex> lock(m_handlers_mutex);
   return (uint32_t)m_handlers.size();
}

void SocketServer::closeAllClients()
{
   UT_Stdout_Log(SOCK_DRV, LOW, "[%d] closing all clients", m_port);
   std::lock_guard<std::mutex> lock(m_handlers_mutex);

   for (auto& handler : m_handlers)
   {
      handler->stop();
      system_call::close(handler->getClientID());
      notifyListeners(handler->getClientID(), ServerEvent::CLIENT_DISCONNECTED, {}, 0);
   }
   UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] %u clients closed", m_port, m_handlers.size());
   m_handlers.clear();
}
void SocketServer::listening_thread()
{
   /* This method is calling from different thread! */
   UT_Stdout_Log(SOCK_DRV, LOW, "[%d] starting listening thread", m_port);
   struct sockaddr_in address;
   int addrlen = sizeof(address);

   while(m_listening_thread.isRunning())
   {
      if (system_call::listen(m_server_fd, (int)m_max_clients) != -1)
      {
         int new_socket = system_call::accept(m_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
         if (new_socket != -1)
         {
            UT_Stdout_Log(SOCK_DRV, HIGH, "[%d] new client connected", m_port);
            bool handler_ready = true;
            {
               std::lock_guard<std::mutex> lock(m_handlers_mutex);
               m_handlers.emplace_back(std::unique_ptr<ISocketClientHandler>(new ClientHandler(new_socket, m_mode, this)));
               if (!m_handlers.back()->start(SERVER_THREAD_START_TIMEOUT))
               {
                  UT_Stdout_Log(SOCK_DRV, ERROR, "[%d] cannot start thread for client!", m_port);
                  m_handlers.pop_back();
                  system_call::close(new_socket);
                  handler_ready = false;
               }
            }
            if (handler_ready)
            {
               notifyListeners(new_socket, ServerEvent::CLIENT_CONNECTED, {}, 0);
            }
         }
      }
   }
}

void SocketServer::worker_thread()
{
   std::unique_lock<std::mutex> lock(m_client_mtx);
   while(m_working_thread.isRunning())
   {
      if(m_cond_var.wait_for(lock, std::chrono::milliseconds(SERVER_RECEIVE_TIMEOUT), [&](){return m_client_events.size() > 0;}))
      {
         while(m_client_events.size() > 0)
         {
            ClientEventData& data = m_client_events.front();
            switch(data.event)
            {
            case ClientEvent::DATA_RECEIVED:
               notifyListeners(data.client_id, ServerEvent::CLIENT_DATA_RECV, data.data, data.data.size());
               break;
            case ClientEvent::CLIENT_DISCONNECTED:
               closeClient(data.client_id);
               notifyListeners(data.client_id, ServerEvent::CLIENT_DISCONNECTED, {}, 0);
               break;
            }
            m_client_events.pop();
         }
      }
   }
}

void SocketServer::onClientEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_client_mtx);
   storeEvent(client_id, ev, data, size);
   m_cond_var.notify_all();
}
void SocketServer::closeClient(int client_id)
{
   std::lock_guard<std::mutex> lock(m_handlers_mutex);
   UT_Stdout_Log(SOCK_DRV, LOW, "[%d] closing client with ID%u", m_port, client_id);

   auto it = std::find_if(m_handlers.begin(), m_handlers.end(), [&](std::unique_ptr<ISocketClientHandler>& client){ return (client->getClientID() == client_id);});
   if (it != m_handlers.end())
   {
      (*it)->stop();
      system_call::close((*it)->getClientID());
      m_handlers.erase(it);
   }
}
void SocketServer::storeEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   ClientEventData ev_data;
   ev_data.event = ev;
   ev_data.client_id = client_id;
   ev_data.data = std::vector<uint8_t>(data.begin(), data.begin() + size);
   m_client_events.push(ev_data);
}
void SocketServer::addListener(ServerListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void SocketServer::removeListener(ServerListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}

void SocketServer::notifyListeners(int client_id, ServerEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   for (auto& listener : m_listeners)
   {
      if (listener)
      {
         listener->onServerEvent(client_id, ev, data, size);
      }
   }
}

bool SocketServer::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   std::lock_guard<std::mutex> lock(m_handlers_mutex);
   for (auto& handler : m_handlers)
   {
      if (handler)
      {
         result |= handler->write(data, size);
         if (!result)
         {
            printf("[%u] Cannot sent data to ClientID%d", m_port, handler->getClientID());
         }
      }
   }
   return result;
}
SocketServer::~SocketServer()
{
   stop();
}

}
}
