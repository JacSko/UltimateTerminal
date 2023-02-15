#ifndef _SOCKETCLIENT_H_
#define _SOCKETCLIENT_H_

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
/* =============================
 *   Includes of project headers
 * =============================*/
#include "ISocketClient.h"
#include "ThreadWorker.h"
#include "SocketHeaderHandler.hpp"
/* =============================
 *           Defines
 * =============================*/
namespace Drivers
{
namespace SocketClient
{

class SocketClient : public ISocketClient
{
public:
   SocketClient(DataMode mode = DataMode::NEW_LINE_DELIMITER);
   ~SocketClient();
private:

   enum class State
   {
      IDLE,
      CONNECTING,
      CONNECTED,
      DISCONNECTING,
      CLOSING,
   };

   /* ISocketClient */
   bool connect(std::string ip_address, uint16_t port) override;
   void disconnect() override;
   bool isConnected() override;
   void addListener(ClientListener* callback) override;
   void removeListener(ClientListener* callback) override;
   bool write(const std::vector<uint8_t>& data, size_t size) override;

   void receivingThread();
   void startDelimiterMode();
   void startHeaderMode();

   void notifyListeners(ClientEvent ev, const std::vector<uint8_t>& data, size_t size);

   Utilities::ThreadWorker m_worker;
   DataMode m_mode;
   std::string m_ip_address;
   uint16_t m_port;

   int m_sock_fd;
   std::vector<uint8_t> m_write_buffer;
   std::vector<uint8_t> m_recv_buffer;
   uint32_t m_recv_buffer_idx;

   std::vector<ClientListener*> m_listeners;
   std::mutex m_listeners_mutex;
   HeaderHandler m_header_handler;
   std::atomic<State> m_state;
   std::mutex m_mutex;
   std::condition_variable m_cond_var;
};

}
}
#endif
