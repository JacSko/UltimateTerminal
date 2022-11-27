#include "ISocketDriverFactory.h"

#include "ISocketServer.h"
#include "ISocketClient.h"

namespace Drivers
{

std::unique_ptr<SocketClient::ISocketClient> SocketFactory::createClient()
{
   return SocketClient::ISocketClient::create();
}

std::unique_ptr<SocketServer::ISocketServer> SocketFactory::createServer()
{
   return SocketServer::ISocketServer::create();
}

}
