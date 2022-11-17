#ifndef _SOCKET_DRIVER_FACTORY_H
#define _SOCKET_DRIVER_FACTORY_H

#include <memory>
#include "ISocketClient.h"
#include "ISocketServer.h"

/**
 * @file ISocketDriverFactory.h
 *
 * @brief
 *    Factory to create socket driver entities - client or server.
 *
 *
 * @author Jacek Skowronek
 * @date   15/11/2022
 *
 */

namespace Drivers
{
struct SocketFactory
{
   /**
    * @brief Creates socket client.
    * @return unique_ptr to socket client object.
    */
   static std::unique_ptr<SocketClient::ISocketClient> createClient();
   /**
    * @brief Creates socket server.
    * @return unique_ptr to socket server object.
    */
   static std::unique_ptr<SocketServer::ISocketServer> createServer();
};

}


#endif
