#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "PortHandler.h"

namespace GUI
{

struct PortHandlerMock
{
   MOCK_METHOD1(getName,const std::string&(uint8_t id));
   MOCK_METHOD2(write, bool(const std::vector<uint8_t>& data, size_t size));
   MOCK_METHOD0(refreshUi, void());
};

PortHandlerMock* g_port_handler_mock;
static uint8_t PORT_ID = 0;

void PortHandlerMock_init()
{
   if (!g_port_handler_mock)
   {
      g_port_handler_mock = new PortHandlerMock();
      PORT_ID = 0;
   }
}
void PortHandlerMock_deinit()
{
   if (g_port_handler_mock)
   {
      delete g_port_handler_mock;
      g_port_handler_mock = nullptr;
   }
}
PortHandlerMock* PortHandlerMock_get()
{
   UT_Assert(g_port_handler_mock && "Create PortHandlerMock mock first!");
   return g_port_handler_mock;
}


PortHandler::PortHandler(QPushButton*, QLabel*, QShortcut*, Utilities::ITimers& timer, PortHandlerListener*, QWidget*, Persistence::PersistenceHandler& persistence)
:m_timers(timer),
 m_persistence(persistence)
{
   PORT_ID++;
   m_settings.port_id = PORT_ID;
}
PortHandler::~PortHandler()
{}

bool PortHandler::write(const std::vector<uint8_t>& data, size_t size)
{
   return PortHandlerMock_get()->write(data, size);
}

const std::string& PortHandler::getName()
{
   return PortHandlerMock_get()->getName(m_settings.port_id);
}
void PortHandler::refreshUi()
{
   return PortHandlerMock_get()->refreshUi();
}
void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>&, size_t){};
void PortHandler::onSerialEvent(Drivers::Serial::DriverEvent, const std::vector<uint8_t>&, size_t){};
void PortHandler::onTimeout(uint32_t){};
void PortHandler::onPersistenceRead(const std::vector<uint8_t>&){};
void PortHandler::onPersistenceWrite(std::vector<uint8_t>&){};


}
