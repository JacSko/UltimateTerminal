#pragma once

#include <memory>
#include "IRPCSocketClient.h"
#include "IRPCSocketServer.h"


namespace RPC
{
struct SocketFactory
{
   static std::unique_ptr<SocketClient::ISocketClient> createClient(SocketClient::ClientType type);
   static std::unique_ptr<SocketServer::ISocketServer> createServer(SocketServer::ServerType type);
};

}
