#include "RPCServer.h"
#include "TestLogger.h"

namespace RPC
{

RPCServer::RPCServer():
m_server(Drivers::SocketServer::ISocketServer::create(Drivers::SocketServer::DataMode::PAYLOAD_HEADER))

{
   m_server->addListener(this);
}

bool RPCServer::start(uint16_t port)
{
   return m_server->start(port, 1);
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

void RPCServer::onServerEvent(int client_id, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t)
{
   switch(ev)
   {
   case Drivers::SocketServer::ServerEvent::CLIENT_DATA_RECV:
      if (data.size() > 0)
      {
         std::lock_guard<std::mutex> lock(m_executors_mutex);
         uint8_t command = data[RPC_COMMAND_BYTE_OFFSET];
         TF_Log(RPC_SERVER, "Got client data with command %u", command);
         if (m_executors[command])
         {
            m_executors[command](std::vector<uint8_t>(data.begin() + RPC_MESSAGE_HEADER_SIZE, data.end()));
         }
         else
         {
            TF_Log(RPC_SERVER, "No executor for command %u", command);
         }
      }
      break;
   case Drivers::SocketServer::ServerEvent::CLIENT_DISCONNECTED:
      TF_Log(RPC_SERVER, "Client disconnected");
      break;
   case Drivers::SocketServer::ServerEvent::CLIENT_CONNECTED:
      TF_Log(RPC_SERVER, "Client connected, id %d", client_id);
      break;
   }
}


}
