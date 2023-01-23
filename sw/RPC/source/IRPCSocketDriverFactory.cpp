#include "IRPCSocketDriverFactory.h"

#include "RPCSocketServer.h"
#include "RPCSocketClient.h"

namespace RPC
{

std::unique_ptr<SocketClient::ISocketClient> SocketFactory::createClient(SocketClient::ClientType type)
{
   return std::unique_ptr<SocketClient::ISocketClient>(new SocketClient::SocketClient());
}

std::unique_ptr<SocketServer::ISocketServer> SocketFactory::createServer(SocketServer::ServerType type)
{
   return std::unique_ptr<SocketServer::ISocketServer>(new SocketServer::SocketServer());
}

}
