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


PortHandler::PortHandler(QPushButton* object, QLabel* label, Utilities::ITimers& timer, PortHandlerListener* listener, QWidget* parent, Persistence::PersistenceHandler& persistence)
:m_persistence(persistence),
 m_timers(timer)
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

void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size){};
void PortHandler::onSerialEvent(Drivers::Serial::DriverEvent ev, const std::vector<uint8_t>& data, size_t size){};
void PortHandler::onTimeout(uint32_t timer_id){};
void PortHandler::onPersistenceRead(const std::vector<uint8_t>& data){};
void PortHandler::onPersistenceWrite(std::vector<uint8_t>& data){};


}
