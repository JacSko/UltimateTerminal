#include "RPCServer.h"


namespace RPC
{

RPCServer::RPCServer():
m_server(Drivers::SocketServer::ISocketServer::create())

{
   m_server->addListener(this);
}

bool RPCServer::start(uint16_t port)
{
   return m_server->start(Drivers::SocketServer::DataMode::PAYLOAD_HEADER, port, 1);
}

void RPCServer::stop()
{
   m_server->stop();
}
void RPCServer::addCommandExecutor(Command cmd, std::function<bool(const std::vector<uint8_t>&)> executor)
{
   std::lock_guard<std::mutex> lock(m_executors_mutex);
   m_executors[cmd] = executor;
}

void RPCServer::removeCommandExecutor(Command cmd)
{
   std::lock_guard<std::mutex> lock(m_executors_mutex);
   m_executors.erase(cmd);
}

bool RPCServer::respond(const std::vector<uint8_t>& data)
{
   return m_server->write(data, data.size());
}
void RPCServer::onServerEvent(int client_id, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   switch(ev)
   {
   case Drivers::SocketServer::ServerEvent::CLIENT_DATA_RECV:
      if (data.size() > 0)
      {
         std::lock_guard<std::mutex> lock(m_executors_mutex);
         Command cmd = (Command)data[0];
         if (m_executors[cmd])
         {
            m_executors[cmd](data);
         }
         else
         {
            UT_Log(RPC_SERVER, ERROR, "No executor for command %u", (uint8_t)cmd);
         }
      }
      break;
   case Drivers::SocketServer::ServerEvent::CLIENT_DISCONNECTED:
      UT_Log(RPC_SERVER, INFO, "Client disconnected");
      break;
   }
}


}
