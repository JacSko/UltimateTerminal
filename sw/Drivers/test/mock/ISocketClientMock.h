#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "ISerialDriver.h"


namespace Drivers
{
namespace SocketClient
{

struct ISocketClientMock : public ISocketClient
{
   MOCK_METHOD3(connect, bool(DataMode mode, std::string ip_address, uint16_t port));
   MOCK_METHOD0(disconnect, void());
   MOCK_METHOD0(isConnected, bool());
   MOCK_METHOD1(addListener, void(ClientListener*));
   MOCK_METHOD1(removeListener, void(ClientListener*));
   MOCK_METHOD2(write, bool(const std::vector<uint8_t>&, size_t));
};

}
}
