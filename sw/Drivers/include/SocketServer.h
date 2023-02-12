#ifndef _SocketServer_H_
#define _SocketServer_H_

/**
 * @file SocketServer.h
 *
 * @brief
 *    Implementation of ISocketServer interface.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */
/* =============================
 *   Includes of common headers
 * =============================*/
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "ISocketServer.h"
#include "ThreadWorker.h"
#include "ClientHandler.h"
/* =============================
 *           Defines
 * =============================*/
namespace Drivers
{
namespace SocketServer
{

class SocketServer : public ISocketServer,
                     public ClientHandlerListener
{
public:
   SocketServer(DataMode mode = DataMode::NEW_LINE_DELIMITER);
   ~SocketServer();
private:

   struct ClientEventData
   {
      int client_id;
      ClientEvent event;
      std::vector<uint8_t> data;
   };


   /* ISocketServer */
   bool start(uint16_t port, uint8_t max_clients = 1) override;
   void stop() override;
   uint32_t clientsCount() override;
   void addListener(ServerListener* callback) override;
   void removeListener(ServerListener* callback) override;
   bool write(const std::vector<uint8_t>& data, size_t size = 0) override;

   void listening_thread();
   void worker_thread();
   void storeEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size);

   /* ClientHandlerListener */
   void onClientEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size) override;

   void notifyListeners(int client_id, ServerEvent ev, const std::vector<uint8_t>& data, size_t size);
   void closeClient(int client_id);
   void closeAllClients();

   Utilities::ThreadWorker m_listening_thread;
   Utilities::ThreadWorker m_working_thread;
   std::atomic<int> m_server_fd;
   uint16_t m_port;
   std::atomic<uint8_t> m_max_clients;
   DataMode m_mode;

   std::vector<ServerListener*> m_listeners;
   std::mutex m_listeners_mutex;
   std::vector<std::unique_ptr<ISocketClientHandler>> m_handlers;
   std::mutex m_handlers_mutex;
   std::mutex m_client_mtx;

   std::queue<ClientEventData> m_client_events;
   std::condition_variable m_cond_var;

};

}
}
#endif
