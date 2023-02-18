/* =============================
 *   Includes of project headers
 * =============================*/
#include "QtSocketServer.h"
#include "Logger.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <string.h>

namespace Drivers
{
namespace SocketServer
{

constexpr uint32_t SERVER_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t SERVER_RECEIVE_TIMEOUT = 500;

std::unique_ptr<ISocketServer> ISocketServer::create(DataMode)
{
   return std::unique_ptr<ISocketServer>(new QtSocketServer());
}

QtSocketServer::QtSocketServer():
m_listening_thread(std::bind(&QtSocketServer::listening_thread, this), "SOCK_LISTEN"),
m_working_thread(std::bind(&QtSocketServer::worker_thread, this), "SOCK_WORKER"),
m_server_fd(-1),
m_port(0),
m_max_clients(0),
m_mode(DataMode::NEW_LINE_DELIMITER),
m_server(nullptr),
m_listeners {},
m_handlers {},
m_state(State::IDLE)
{
   if (!m_working_thread.start(SERVER_THREAD_START_TIMEOUT))
   {
      UT_Stdout_Log(SOCK_DRV, ERROR, "cannot start working thread %s", m_working_thread.getThreadName().c_str());
   }
}
bool QtSocketServer::start(uint16_t port, uint8_t max_clients)
{
   m_port = port;
   m_max_clients = max_clients;
   UT_Stdout_Log(SOCK_DRV, LOW, "%s", __func__);
   if (port == 0)
   {
      UT_Stdout_Log(SOCK_DRV, ERROR, "Invalid port %u", port);
      return false;
   }

   std::unique_lock<std::mutex> lock(m_starting_mutex);
   m_state = State::STARTING;
   m_listening_thread.start(SERVER_THREAD_START_TIMEOUT);
   m_starting_cond_var.wait_for(lock, std::chrono::milliseconds(500), [&](){return (m_state == State::RUNNING) || (m_state == State::IDLE); });
   return m_state == State::RUNNING;
}

void QtSocketServer::stop()
{
   closeAllClients();
   m_listening_thread.stop();
   m_working_thread.stop();

}
uint32_t QtSocketServer::clientsCount()
{
   std::lock_guard<std::mutex> lock(m_handlers_mutex);
   return (uint32_t)m_handlers.size();
}

void QtSocketServer::closeAllClients()
{
   std::lock_guard<std::mutex> lock(m_handlers_mutex);

   for (auto& handler : m_handlers)
   {
      handler->stop();
      notifyListeners(handler->getClientID(), ServerEvent::CLIENT_DISCONNECTED, {}, 0);
   }
   m_handlers.clear();
}
void QtSocketServer::listening_thread()
{
   {
      std::lock_guard<std::mutex> lock(m_starting_mutex);
      m_server = new QTcpServer();
      UT_Assert(m_server);
      m_state = State::RUNNING;
      UT_Stdout_Log(SOCK_DRV, INFO, "Listening on port %u", m_port);
      if (!m_server->listen(QHostAddress::Any, m_port))
      {
         UT_Stdout_Log(SOCK_DRV, LOW, "cannot listen for clients!");
         m_state = State::IDLE;
         delete m_server;
         m_server = nullptr;
         m_starting_cond_var.notify_all();
         return;
      }
      m_starting_cond_var.notify_all();
   }

   while(m_listening_thread.isRunning())
   {
      if (m_server->waitForNewConnection(200))
      {
         QTcpSocket* socket = m_server->nextPendingConnection();
         std::lock_guard<std::mutex> lock(m_handlers_mutex);
         UT_Stdout_Log(SOCK_DRV, LOW, "New client connected, starting thread");
         m_handlers.emplace_back(createClientHandler(socket->socketDescriptor()));
         if (!m_handlers.back()->start(SERVER_THREAD_START_TIMEOUT))
         {
            UT_Stdout_Log(SOCK_DRV, ERROR, "Cannot start thread, aborting client %u", socket->socketDescriptor());
            m_handlers.pop_back();
            socket->abort();
         }
         notifyListeners(socket->socketDescriptor(), ServerEvent::CLIENT_CONNECTED, {}, 0);
      }
   }

   UT_Stdout_Log(SOCK_DRV, LOW, "Exiting listening thread");
   delete m_server;
   m_server = nullptr;
}

std::unique_ptr<ISocketClientHandler> QtSocketServer::createClientHandler(int socket)
{
   return std::unique_ptr<ISocketClientHandler>(new QtClientHandler(socket, m_mode, this));
}
void QtSocketServer::worker_thread()
{
   std::unique_lock<std::mutex> lock(m_client_mtx);
   while(m_working_thread.isRunning())
   {
      if(m_cond_var.wait_for(lock, std::chrono::milliseconds(SERVER_RECEIVE_TIMEOUT), [&](){return m_client_events.size() > 0;}))
      {
         while(m_client_events.size() > 0)
         {
            ClientEventData& data = m_client_events.front();
            UT_Stdout_Log(SOCK_DRV, LOW, "got event in thread! event %u, client %d, data_size %u", (uint8_t)data.event, data.client_id, data.data.size());
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

void QtSocketServer::onClientEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_client_mtx);
   UT_Stdout_Log(SOCK_DRV, LOW, "event %u, client %d, data_size %u", (uint8_t)ev, client_id, (uint32_t)size);
   storeEvent(client_id, ev, data, size);
   m_cond_var.notify_all();
}
void QtSocketServer::closeClient(int client_id)
{
   std::lock_guard<std::mutex> lock(m_handlers_mutex);

   auto it = std::find_if(m_handlers.begin(), m_handlers.end(), [&](std::unique_ptr<ISocketClientHandler>& client){ return (client->getClientID() == client_id);});
   if (it != m_handlers.end())
   {
      (*it)->stop();
      m_handlers.erase(it);
   }
}
void QtSocketServer::storeEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   ClientEventData ev_data;
   ev_data.event = ev;
   ev_data.client_id = client_id;
   ev_data.data = std::vector<uint8_t>(data.begin(), data.begin() + size);
   m_client_events.push(ev_data);
}
void QtSocketServer::addListener(ServerListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void QtSocketServer::removeListener(ServerListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}

void QtSocketServer::notifyListeners(int client_id, ServerEvent ev, const std::vector<uint8_t>& data, size_t size)
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

bool QtSocketServer::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   std::lock_guard<std::mutex> lock(m_handlers_mutex);
   UT_Log(SOCK_DRV, HIGH, "Writing %u bytes to clients", (uint32_t)data.size());
   for (auto& handler : m_handlers)
   {
      if (handler)
      {
         result |= handler->write(data, size);
         if (!result)
         {
            UT_Stdout_Log(SOCK_DRV, ERROR, "Cannot sent data to ClientID%d", handler->getClientID());
         }
      }
   }
   return result;
}
QtSocketServer::~QtSocketServer()
{
   stop();
}

}
}
