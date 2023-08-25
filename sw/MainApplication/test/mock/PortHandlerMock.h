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
   MOCK_METHOD2(setSettings, bool(uint8_t id, const Dialogs::PortSettingDialog::Settings&));
   MOCK_METHOD1(getSettings, const Dialogs::PortSettingDialog::Settings&(uint8_t id));
   MOCK_METHOD1(isOpened, bool(uint8_t id));
};

PortHandlerMock* g_port_handler_mock;

void PortHandlerMock_init()
{
   if (!g_port_handler_mock)
   {
      g_port_handler_mock = new PortHandlerMock();
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


PortHandler::PortHandler(uint8_t id,
                         GUIController& gui_controller,
                         const std::string&,
                         Utilities::ITimers& timers,
                         PortHandlerListener*,
                         Persistence::PersistenceHandler& persistence)
: m_gui_controller(gui_controller),
m_timers(timers),
m_persistence(persistence)
{
   m_settings.port_id = id;
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
bool PortHandler::setSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   return PortHandlerMock_get()->setSettings(m_settings.port_id, settings);
}
const Dialogs::PortSettingDialog::Settings& PortHandler::getSettings()
{
   return PortHandlerMock_get()->getSettings(m_settings.port_id);
}
bool PortHandler::isOpened()
{
   return PortHandlerMock_get()->isOpened(m_settings.port_id);
}

void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>&, size_t){};
void PortHandler::onSerialEvent(Drivers::Serial::DriverEvent, const std::vector<uint8_t>&, size_t){};
void PortHandler::onTimeout(uint32_t){};
void PortHandler::onPersistenceRead(const PersistenceItems&){};
void PortHandler::onPersistenceWrite(PersistenceItems&){};
void PortHandler::onButtonEvent(uint32_t, ButtonEvent){};

}
