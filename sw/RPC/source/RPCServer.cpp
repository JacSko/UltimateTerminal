#include "RPCServer.h"
#include "IRPCSocketDriverFactory.h"

namespace RPC
{

RPCServer::RPCServer():
m_server(RPC::SocketFactory::createServer(SocketServer::ServerType::RAW_DATA))

{
   m_server->addListener(this);
}

bool RPCServer::start(uint16_t port)
{
   return m_server->start(SocketServer::DataMode::PAYLOAD_HEADER, port, 1);
}

void RPCServer::stop()
{
   m_server->stop();
}
void RPCServer::addCommandExecutor(uint8_t cmd, std::function<bool(const std::vector<uint8_t>&)> executor)
{
   std::lock_guard<std::mutex> lock(m_executors_mutex);
   m_executors[cmd] = executor;
}

void RPCServer::removeCommandExecutor(uint8_t cmd)
{
   std::lock_guard<std::mutex> lock(m_executors_mutex);
   m_executors.erase(cmd);
}

void RPCServer::onServerEvent(int client_id, SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   switch(ev)
   {
   case SocketServer::ServerEvent::CLIENT_DATA_RECV:
      if (data.size() > 0)
      {
         std::lock_guard<std::mutex> lock(m_executors_mutex);
         uint8_t command = data[RPC_COMMAND_BYTE_OFFSET];
         UT_Log(RPC_SERVER, HIGH, "Got client data with command %u", command);
         if (m_executors[command])
         {
            m_executors[command](std::vector<uint8_t>(data.begin() + RPC_MESSAGE_HEADER_SIZE, data.end()));
         }
         else
         {
            UT_Log(RPC_SERVER, ERROR, "No executor for command %u", command);
         }
      }
      break;
   case SocketServer::ServerEvent::CLIENT_DISCONNECTED:
      UT_Log(RPC_SERVER, INFO, "Client disconnected");
      break;
   }
}


}
