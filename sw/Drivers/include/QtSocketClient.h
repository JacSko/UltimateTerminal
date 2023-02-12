#ifndef _QTSOCKETCLIENT_H_
#define _QTSOCKETCLIENT_H_

/**
 * @file SocketClient.h
 *
 * @brief
 *    Implementation of ISocketClient interface.
 *
 * @author Jacek Skowronek
 * @date   08/04/2021
 *
 */
/* =============================
 *   Includes of common headers
 * =============================*/
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <QtNetwork/QTcpSocket>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "ISocketClient.h"
#include "ThreadWorker.h"
/* =============================
 *           Defines
 * =============================*/
namespace Drivers
{
namespace SocketClient
{

class QtSocketClient : public ISocketClient
{
public:
   QtSocketClient();
   ~QtSocketClient();
private:
   enum class State
   {
      IDLE,
      CONNECTION_REQUESTED,
      CONNECTED,
      DISCONNECTING,
      DESTROYING,
   };
   /* ISocketClient */
   bool connect(std::string ip_address, uint16_t port) override;
   void disconnect() override;
   bool isConnected() override;
   void addListener(ClientListener* callback) override;
   void removeListener(ClientListener* callback) override;
   bool write(const std::vector<uint8_t>& data, size_t size) override;

   void receivingThread();
   void notifyListeners(ClientEvent ev, const std::vector<uint8_t>& data, size_t size);

   QTcpSocket* m_socket;
   Utilities::ThreadWorker m_worker;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;
   State m_state;
   DataMode m_mode;
   std::string m_ip_address;
   uint16_t m_port;

   std::vector<uint8_t> m_write_buffer;
   std::vector<uint8_t> m_recv_buffer;
   uint32_t m_recv_buffer_idx;

   std::vector<ClientListener*> m_listeners;
   std::mutex m_listeners_mutex;

};

}
}
#endif
