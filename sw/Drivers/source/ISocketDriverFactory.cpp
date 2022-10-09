#include "ISocketDriverFactory.h"

#include "SocketServer.h"
#include "SocketClient.h"

namespace Drivers
{

std::unique_ptr<SocketClient::ISocketClient> SocketFactory::createClient()
{
   return std::unique_ptr<SocketClient::ISocketClient>(new SocketClient::SocketClient());
}

std::unique_ptr<SocketServer::ISocketServer> SocketFactory::createServer()
{
   return std::unique_ptr<SocketServer::ISocketServer>(new SocketServer::SocketServer());
}

}
