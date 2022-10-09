#ifndef ISOCKETCLIENTHANDLER_H_
#define ISOCKETCLIENTHANDLER_H_

#include <vector>
#include <stdint.h>


namespace Drivers
{
namespace SocketServer
{

enum class ClientEvent
{
   CLIENT_DISCONNECTED,
   DATA_RECEIVED,
};

class ClientHandlerListener
{
public:
   virtual ~ClientHandlerListener() {};
   virtual void onClientEvent(int client_id, ClientEvent ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class ISocketClientHandler
{
public:
   virtual bool start(uint32_t timeout) = 0;
   virtual void stop() = 0;
   virtual bool write(const std::vector<uint8_t>& data, size_t size) = 0;
   virtual int getClientID() = 0;
   virtual ~ISocketClientHandler(){};
};

}
}
#endif
