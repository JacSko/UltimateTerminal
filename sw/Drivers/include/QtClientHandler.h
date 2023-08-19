#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_

/**
 * @file ClientHandler.h
 *
 * @brief
 *    Implementation of ClientHandler. This class is responsible for serving communication with defined endpoint.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */
/* =============================
 *   Includes of common headers
 * =============================*/
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <atomic>
#include <QtNetwork/QTcpSocket>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "ThreadWorker.h"
#include "ISocketServer.h"
#include "ISocketClientHandler.h"
#include "SocketHeaderHandler.hpp"
/* =============================
 *           Defines
 * =============================*/

namespace Drivers
{
namespace SocketServer
{

class QtClientHandler : public ISocketClientHandler
{
public:
   QtClientHandler(int client_id, DataMode mode, ClientHandlerListener* listener);
   virtual ~QtClientHandler();
private:
   enum class State
   {
      IDLE,
      STARTING,
      RUNNING,
      STOPPING,
   };

   bool start(uint32_t timeout) override;
   void stop() override;
   bool write(const std::vector<uint8_t>& data, size_t size) override;
   int getClientID() override;

   void execute();
   const std::string getThreadName(int client_id);
   void startDelimiterMode();
   void startHeaderMode();
   void writePendingData();

   QTcpSocket* m_socket;
   Utilities::ThreadWorker m_worker;
   int m_client_id;
   ClientHandlerListener* m_listener;
   std::vector<uint8_t> m_recv_buffer;
   std::mutex m_write_buffer_mutex;
   std::condition_variable m_write_buffer_condvar;
   std::vector<uint8_t> m_write_buffer;
   uint32_t m_recv_buffer_idx;
   DataMode m_mode;
   HeaderHandler m_header_handler;

   State m_state;
   std::mutex m_starting_mutex;
   std::condition_variable m_starting_cond_var;

};

}
}
#endif
