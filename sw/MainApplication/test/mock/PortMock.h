#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "Port.h"

namespace MainApplication
{

struct PortMock
{
   MOCK_METHOD1(getName,const std::string&(uint8_t id));
   MOCK_METHOD2(write, bool(const std::vector<uint8_t>& data, size_t size));
   MOCK_METHOD0(refreshUi, void());
   MOCK_METHOD2(setSettings, bool(uint8_t id, const Dialogs::PortSettingDialog::Settings&));
   MOCK_METHOD1(getSettings, const Dialogs::PortSettingDialog::Settings&(uint8_t id));
   MOCK_METHOD1(isOpened, bool(uint8_t id));
};

PortMock* g_port_handler_mock;

void PortMock_init()
{
   if (!g_port_handler_mock)
   {
      g_port_handler_mock = new PortMock();
   }
}
void PortMock_deinit()
{
   if (g_port_handler_mock)
   {
      delete g_port_handler_mock;
      g_port_handler_mock = nullptr;
   }
}
PortMock* PortMock_get()
{
   UT_Assert(g_port_handler_mock && "Create PortMock mock first!");
   return g_port_handler_mock;
}


Port::Port(uint8_t id,
                         GUIController::GUIController& gui_controller,
                         const std::string&,
                         Utilities::ITimers& timers,
                         PortListener*,
                         Utilities::Persistence::Persistence& persistence):
m_guiController(gui_controller),
m_timers(timers),
m_persistence(persistence)
{
   m_settings.port_id = id;
}
Port::~Port()
{}

bool Port::write(const std::vector<uint8_t>& data, size_t size)
{
   return PortMock_get()->write(data, size);
}

const std::string& Port::getName()
{
   return PortMock_get()->getName(m_settings.port_id);
}
void Port::refreshUi()
{
   return PortMock_get()->refreshUi();
}
bool Port::setSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   return PortMock_get()->setSettings(m_settings.port_id, settings);
}
const Dialogs::PortSettingDialog::Settings& Port::getSettings()
{
   return PortMock_get()->getSettings(m_settings.port_id);
}
bool Port::isOpened()
{
   return PortMock_get()->isOpened(m_settings.port_id);
}

void Port::onEvent(DriversProxy::Event, const std::vector<uint8_t>&, uint32_t){};
void Port::onTimeout(uint32_t){};
void Port::onPersistenceRead(const PersistenceItems&){};
void Port::onPersistenceWrite(PersistenceItems&){};
void Port::onButtonEvent(uint32_t, GUIController::ButtonEvent){};

}
