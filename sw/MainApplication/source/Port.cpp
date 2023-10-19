#include <QtWidgets/QPushButton>
#include <optional>
#include <string.h>
#include <errno.h>

#include "Logger.h"
#include "Port.h"
#include "MessageDialog.h"
#include "Serialize.hpp"

namespace MainApplication
{

constexpr uint32_t DEFAULT_CONNECT_RETRY_PERIOD = 1000;

Port::Port(uint8_t id,
           GUIController::GUIController& gui_controller,
           const std::string& button_name,
           Utilities::ITimers& timers,
           PortListener* listener,
           Utilities::Persistence::Persistence& persistence):
m_guiController(gui_controller),
m_settings({}),
m_connectRetryPeriod(DEFAULT_CONNECT_RETRY_PERIOD),
m_timers(timers),
m_proxy(nullptr),
m_timerId(TIMERS_INVALID_ID),
m_buttonState(ButtonState::DISCONNECTED),
m_persistence(persistence),
m_throughputCalculatorTimerID(TIMERS_INVALID_ID)
{
   UT_Assert(m_buttonId != UINT32_MAX);
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Creating port handler", m_settings.port_id,
                                                                 m_settings.port_name.c_str());
   m_buttonId = m_guiController.getButtonID(button_name);
   m_guiController.subscribeForButtonEvent(m_buttonId, GUIController::ButtonEvent::CLICKED, this);
   m_guiController.subscribeForButtonEvent(m_buttonId, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);

   GenericListener::addListener(*listener);
   m_timerId = m_timers.createTimer(this, m_connectRetryPeriod);
   m_throughputCalculatorTimerID = m_timers.createTimer(this, SETTING_GET_U32(Throughput_Calculation_Window));
   m_settings.port_id = id;
   m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);

   Utilities::Persistence::PersistenceListener::setName(m_settings.port_name);
   m_persistence.addListener(*this);
   handleNewSettings(m_settings);
   notifyListeners(Event::DISCONNECTED);
}
Port::~Port()
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Destroying port handler", m_settings.port_id,
                                                                   m_settings.port_name.c_str());
   m_persistence.removeListener(*this);
   GenericListener::removeAllListeners();
   m_timers.removeTimer(m_timerId);
   m_timers.removeTimer(m_throughputCalculatorTimerID);
   m_timerId = TIMERS_INVALID_ID;
   m_throughputCalculatorTimerID = TIMERS_INVALID_ID;
   m_proxy.reset(nullptr);
   setState(ButtonState::DISCONNECTED);
}
void Port::onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event)
{
   if (button_id == m_buttonId)
   {
      if (event == GUIController::ButtonEvent::CLICKED)
      {
         onPortButtonClicked();
      }
      else if (event == GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED)
      {
         onPortButtonContextMenuRequested();
      }
   }
   UT_Log_If(button_id != m_buttonId, PORT_HANDLER, ERROR,
             "Unexpected event for for button %u, while subscribed for %u", button_id, m_buttonId);
}
void Port::notifyListeners(Event event, const std::vector<uint8_t>& data)
{
   GenericListener::notifyChange([&](PortListener* l){l->onPortEvent({m_settings.port_id,
                                                                      m_settings.port_name,
                                                                      m_settings.trace_color,
                                                                      m_settings.font_color,
                                                                      event,
                                                                      data});});
}
const std::string& Port::getName()
{
   return m_settings.port_name;
}
bool Port::write(const std::vector<uint8_t>& data, size_t size)
{
   UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] writing %u bytes", m_settings.port_id,
                                                             m_settings.port_name.c_str(),
                                                             (uint32_t)data.size());
   UT_Assert(m_proxy && "Create drivers proxy!");
   return m_proxy->write(data, size);
}
void Port::refreshUi()
{
   setState(m_buttonState);
}
bool Port::setSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   bool result = false;
   if (m_buttonState == ButtonState::DISCONNECTED)
   {
      handleNewSettings(settings);
      result = true;
   }
   return result;
}
const Dialogs::PortSettingDialog::Settings& Port::getSettings()
{
   return m_settings;
}
bool Port::isOpened()
{
   return (m_buttonState != ButtonState::DISCONNECTED);
}
void Port::onEvent(DriversProxy::Event event, const std::vector<uint8_t>& data, uint32_t size)
{
   if (event == DriversProxy::Event::ENDPOINT_DISCONNECTED)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] endpoint disconnected, trying to reconnect",
                                                                              m_settings.port_id,
                                                                              m_settings.port_name.c_str());
      setState(ButtonState::CONNECTING);
      notifyListeners(Event::CONNECTING);
      if (!tryConnect())
      {
         scheduleConnectionTrial();
      }
   }
   else if (event == DriversProxy::Event::DATA_RECEIVED)
   {
      m_throughputCalculator.reportBytes(size);
      notifyListeners(Event::NEW_DATA, data);
   }
   else
   {
      UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Unknown event received %u", m_settings.port_id,
                                                                          m_settings.port_name.c_str(),
                                                                          (uint8_t) event);
   }
}
void Port::onTimeout(uint32_t timer_id)
{
   // another thread
   if (timer_id == m_timerId)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Retrying connect...", m_settings.port_id,
                                                                  m_settings.port_name.c_str());
      if (!tryConnect())
      {
         scheduleConnectionTrial();
      }
   }

   if (timer_id == m_throughputCalculatorTimerID)
   {
      auto throughputCalculation = m_throughputCalculator.get();
      UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] Reading throughput, value %.5f unit %u",
                                                                     m_settings.port_id,
                                                                     m_settings.port_name.c_str(),
                                                                     throughputCalculation.value,
                                                                     (uint32_t)throughputCalculation.unit);
      m_throughputCalculator.reset();
      m_throughputCalculator.start();
      if (isOpened())
      {
         m_timers.startTimer(m_throughputCalculatorTimerID);
         reportThroughput(throughputCalculation);
      }
      else
      {
         UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] stopping throughput reporting", m_settings.port_id,
                                                                                m_settings.port_name.c_str());
         m_guiController.setThroughputText(m_settings.port_id, "");
      }
   }
}
void Port::reportThroughput(const Utilities::ThroughputResult& throughput)
{
   static const std::map<Utilities::ByteUnit, std::string> unitMap = {{Utilities::ByteUnit::bytes_s, "B/s"},
                                                                  {Utilities::ByteUnit::kilobytes_s, "KB/s"},
                                                                  {Utilities::ByteUnit::megabytes_s, "MB/s"}};
   char buffer [50];
   std::snprintf(buffer, 50, "%.2f %s", throughput.value, unitMap.at(throughput.unit).c_str());
   m_guiController.setThroughputText(m_settings.port_id, std::string(buffer));
}
void Port::scheduleConnectionTrial()
{
   m_timers.setTimeout(m_timerId, m_connectRetryPeriod);
   m_timers.startTimer(m_timerId);
   setState(ButtonState::CONNECTING);
   notifyListeners(Event::CONNECTING);
}
void Port::handleNewSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   m_settings = settings;
   if (m_settings.port_name.empty())
   {
      m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);
   }
   if (m_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] creating serial driver proxy, settings %s",
                                                                     m_settings.port_id,
                                                                     m_settings.port_name.c_str(),
                                                                     m_settings.settingsString().c_str());
      m_proxy.reset(new DriversProxy(m_settings.serialSettings, this));
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] creating ethrnet driver proxy, settings %s",
                                                                     m_settings.port_id,
                                                                     m_settings.port_name.c_str(),
                                                                     m_settings.settingsString().c_str());
      m_proxy.reset(new DriversProxy(m_settings.ip_address, m_settings.port, this));
   }
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] got new settings %s", m_settings.port_id,
                                                               m_settings.port_name.c_str(),
                                                               m_settings.settingsString().c_str());
   setState(m_buttonState);
}
void Port::setState(ButtonState state)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] new button state %u -> %u", m_settings.port_id,
                                                                     m_settings.port_name.c_str(),
                                                                     m_buttonState.load(),
                                                                     state);
   m_buttonState = state;
   m_guiController.setPortLabelText(m_settings.port_id, m_settings.summaryString());
   setButtonName(m_settings.port_name);
   setButtonColors(m_buttonState);
}
void Port::onPortButtonContextMenuRequested()
{
   Dialogs::PortSettingDialog dialog;
   Dialogs::PortSettingDialog::Settings new_settings = {};
   std::optional<bool> result = dialog.showDialog(m_guiController.getParent(),
                                                  m_settings,
                                                  new_settings,
                                                  m_buttonState == ButtonState::DISCONNECTED);
   if (result && result.value())
   {
      handleNewSettings(new_settings);
   }
}
void Port::onPortButtonClicked()
{
   UT_Assert(m_proxy && "Drivers proxy not created");

   if (m_proxy->isStarted())
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to close proxy", m_settings.port_id,
                                                                    m_settings.port_name.c_str());
      m_proxy->stop();
      setState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else if (m_buttonState == ButtonState::CONNECTING)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] aborting connection attempts!", m_settings.port_id,
                                                                            m_settings.port_name.c_str());
      m_timers.stopTimer(m_timerId);
      setState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to start drivers proxy", m_settings.port_id,
                                                                            m_settings.port_name.c_str());
      if (!tryConnect())
      {
         if (m_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
         {
            onSerialConnectionFailed();
         }
         else
         {
            onEthernetConnectionFailed();
         }
      }
   }
}
bool Port::tryConnect()
{
   UT_Assert(m_proxy && "create drivers proxy!");
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] %s", m_settings.port_id, m_settings.port_name.c_str(), __func__);

   m_proxy->stop();
   bool result = m_proxy->start();
   if(result)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Successfully started drivers proxy", m_settings.port_id,
                                                                                 m_settings.port_name.c_str());
      m_timers.stopTimer(m_timerId);
      setState(ButtonState::CONNECTED);
      notifyListeners(Event::CONNECTED);
      m_throughputCalculator.start();
      m_timers.startTimer(m_throughputCalculatorTimerID);
   }
   return result;
}
void Port::onSerialConnectionFailed()
{
   UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Cannot open serial", m_settings.port_id,
                                                                m_settings.port_name.c_str());
   std::string message = "Cannot open " +
                         m_settings.serialSettings.device +
                         " [" +
                         m_settings.port_name +
                         "]\n" +
                         std::string(strerror(errno)) +
                         " (" +
                         std::to_string(errno) +
                         ")";
   Dialogs::MessageDialog::show(Dialogs::MessageDialog::Icon::Critical,
                                "Error",
                                message,
                                m_guiController.getApplicationPalette());
}
void Port::onEthernetConnectionFailed()
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Cannot start drivers proxy, scheduling retries with %u ms period",
                                                                                 m_settings.port_id,
                                                                                 m_settings.port_name.c_str(),
                                                                                 m_connectRetryPeriod);
   m_timers.setTimeout(m_timerId, m_connectRetryPeriod);
   m_timers.startTimer(m_timerId);
   setState(ButtonState::CONNECTING);
   notifyListeners(Event::CONNECTING);
}
void Port::setButtonColors(ButtonState state)
{
   constexpr uint32_t GREEN = 0x00FF00;
   constexpr uint32_t BLUE = 0x0000FF;
   constexpr uint32_t BLACK = 0x000000;
   const uint32_t DEFAULT_BACKGROUND = m_guiController.getBackgroundColor();
   const uint32_t DEFAULT_FONT = m_guiController.getTextColor();

   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] setting button state %.6x", m_settings.port_id,
                                                                     m_settings.port_name.c_str(),
                                                                     (uint32_t)state);
   switch(m_buttonState)
   {
      case ButtonState::CONNECTED:
      {
         m_guiController.setButtonBackgroundColor(m_buttonId, GREEN);
         m_guiController.setButtonFontColor(m_buttonId, BLACK);
         break;
      }
      case ButtonState::CONNECTING:
      {
         m_guiController.setButtonBackgroundColor(m_buttonId, BLUE);
         m_guiController.setButtonFontColor(m_buttonId, BLACK);
         break;
      }
      default:
      {
         m_guiController.setButtonBackgroundColor(m_buttonId, DEFAULT_BACKGROUND);
         m_guiController.setButtonFontColor(m_buttonId, DEFAULT_FONT);
         break;
      }
   }
}
void Port::setButtonName(const std::string name)
{
   m_guiController.setButtonText(m_buttonId, name);
}
void Port::onPersistenceRead(const PersistenceItems& items)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] restoring persistence", m_settings.port_id,
                                                                 m_settings.port_name.c_str());

   Dialogs::PortSettingDialog::Settings new_settings = m_settings;
   std::string baudrate_name = m_settings.serialSettings.baudRate.toName();
   std::string databits_name = m_settings.serialSettings.dataBits.toName();
   std::string paritybits_name = m_settings.serialSettings.parityBits.toName();
   std::string stopbits_name = m_settings.serialSettings.stopBits.toName();
   std::string porttype_name = m_settings.type.toName();

   Utilities::Persistence::readItem(items, "ipAddress", new_settings.ip_address);
   Utilities::Persistence::readItem(items, "ipPort", new_settings.port);
   Utilities::Persistence::readItem(items, "portId", new_settings.port_id);
   Utilities::Persistence::readItem(items, "portName", new_settings.port_name);
   Utilities::Persistence::readItem(items, "baudRate", baudrate_name);
   Utilities::Persistence::readItem(items, "dataBits", databits_name);
   Utilities::Persistence::readItem(items, "parityBits", paritybits_name);
   Utilities::Persistence::readItem(items, "stopBits", stopbits_name);
   Utilities::Persistence::readItem(items, "device", new_settings.serialSettings.device);
   Utilities::Persistence::readItem(items, "mode", (uint8_t&)new_settings.serialSettings.mode);
   Utilities::Persistence::readItem(items, "traceColor", new_settings.trace_color);
   Utilities::Persistence::readItem(items, "fontColor", new_settings.font_color);
   Utilities::Persistence::readItem(items, "type", porttype_name);

   new_settings.serialSettings.baudRate.fromName(baudrate_name);
   new_settings.serialSettings.dataBits.fromName(databits_name);
   new_settings.serialSettings.parityBits.fromName(paritybits_name);
   new_settings.serialSettings.stopBits.fromName(stopbits_name);
   new_settings.type.fromName(porttype_name);

   handleNewSettings(new_settings);
}
void Port::onPersistenceWrite(PersistenceItems& buffer)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] saving persistence [%s]", m_settings.port_id,
                                                                   m_settings.port_name.c_str(),
                                                                   m_settings.settingsString().c_str());

   Utilities::Persistence::writeItem(buffer, "ipAddress", m_settings.ip_address);
   Utilities::Persistence::writeItem(buffer, "ipPort", m_settings.port);
   Utilities::Persistence::writeItem(buffer, "portId", m_settings.port_id);
   Utilities::Persistence::writeItem(buffer, "portName", m_settings.port_name);
   Utilities::Persistence::writeItem(buffer, "baudRate", m_settings.serialSettings.baudRate.toName());
   Utilities::Persistence::writeItem(buffer, "dataBits", m_settings.serialSettings.dataBits.toName());
   Utilities::Persistence::writeItem(buffer, "parityBits", m_settings.serialSettings.parityBits.toName());
   Utilities::Persistence::writeItem(buffer, "stopBits", m_settings.serialSettings.stopBits.toName());
   Utilities::Persistence::writeItem(buffer, "device", m_settings.serialSettings.device);
   Utilities::Persistence::writeItem(buffer, "mode", (uint8_t)m_settings.serialSettings.mode);
   Utilities::Persistence::writeItem(buffer, "traceColor", m_settings.trace_color);
   Utilities::Persistence::writeItem(buffer, "fontColor", m_settings.font_color);
   Utilities::Persistence::writeItem(buffer, "type", m_settings.type.toName());

}

}
