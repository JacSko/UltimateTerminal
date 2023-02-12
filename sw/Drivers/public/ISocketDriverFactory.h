#pragma once

#include <memory>
#include "ISocketClient.h"
#include "ISocketServer.h"


namespace Drivers
{
struct SocketFactory
{
   static std::unique_ptr<SocketClient::ISocketClient> createClient(SocketClient::DataMode mode);
   static std::unique_ptr<SocketServer::ISocketServer> createServer(SocketServer::DataMode mode);
};

}
