#include "ISocketDriverFactory.h"

#include "SocketServer.h"
#include "SocketClient.h"

namespace Drivers
{

std::unique_ptr<SocketClient::ISocketClient> SocketFactory::createClient(SocketClient::DataMode mode)
{
   return std::unique_ptr<SocketClient::ISocketClient>(new SocketClient::SocketClient(mode));
}

std::unique_ptr<SocketServer::ISocketServer> SocketFactory::createServer(SocketServer::DataMode mode)
{
   return std::unique_ptr<SocketServer::ISocketServer>(new SocketServer::SocketServer(mode));
}

}
