#pragma once


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
#include <sys/socket.h>
#include <algorithm>
#include <atomic>
/* =============================
 *   Includes of project headers
 * =============================*/
#include "ThreadWorker.h"
#include "IRPCSocketServer.h"
#include "RPCSocketHeaderHandler.hpp"
#include "IRPCSocketClientHandler.h"
/* =============================
 *           Defines
 * =============================*/

namespace RPC
{
namespace SocketServer
{

class ClientHandler : public ISocketClientHandler
{
public:
   ClientHandler(int client_id, DataMode mode, ClientHandlerListener* listener);
   ~ClientHandler();
private:
   bool start(uint32_t timeout) override;
   void stop() override;
   bool write(const std::vector<uint8_t>& data, size_t size) override;
   int getClientID() override;

   void execute();
   void startDelimiterMode();
   void startHeaderMode();
   const std::string getThreadName(int client_id);

   Utilities::ThreadWorker m_worker;
   int m_client_id;
   ClientHandlerListener* m_listener;
   std::vector<uint8_t> m_recv_buffer;
   std::vector<uint8_t> m_write_buffer;
   uint32_t m_recv_buffer_idx;
   DataMode m_mode;
   HeaderHandler m_header_handler;

};

}
}
