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
m_gui_controller(gui_controller),
m_settings({}),
m_connect_retry_period(DEFAULT_CONNECT_RETRY_PERIOD),
m_timers(timers),
m_socket(Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER)),
m_serial(Drivers::Serial::ISerialDriver::create()),
m_timer_id(TIMERS_INVALID_ID),
m_button_state(ButtonState::DISCONNECTED),
m_persistence(persistence),
m_throughputCalculatorTimerID(TIMERS_INVALID_ID)
{

   m_button_id = m_gui_controller.getButtonID(button_name);
   UT_Assert(m_button_id != UINT32_MAX);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);

   GenericListener::addListener(*listener);
   m_socket->addListener(this);
   m_serial->addListener(this);
   m_timer_id = m_timers.createTimer(this, m_connect_retry_period);
   m_throughputCalculatorTimerID = m_timers.createTimer(this, SETTING_GET_U32(Throughput_Calculation_Window));
   m_settings.port_id = id;
   m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Creating port handler", m_settings.port_id, m_settings.port_name.c_str());

   Utilities::Persistence::PersistenceListener::setName(m_settings.port_name);
   m_persistence.addListener(*this);
   setButtonState(ButtonState::DISCONNECTED);
   setButtonName(m_settings.port_name);
   notifyListeners(Event::DISCONNECTED);

   m_gui_controller.setPortLabelText(m_settings.port_id, m_settings.shortSettingsString().c_str());
}
Port::~Port()
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Destroying port handler", m_settings.port_id, m_settings.port_name.c_str());
   m_persistence.removeListener(*this);
   GenericListener::removeAllListeners();
   m_socket->removeListener(this);
   m_serial->removeListener(this);
   m_timers.removeTimer(m_timer_id);
   m_timer_id = TIMERS_INVALID_ID;
   m_timers.removeTimer(m_throughputCalculatorTimerID);
   m_throughputCalculatorTimerID = TIMERS_INVALID_ID;
   m_socket->disconnect();
   m_serial->close();
   setButtonState(ButtonState::DISCONNECTED);
}
void Port::onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event)
{
   if (button_id == m_button_id)
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
}
void Port::notifyListeners(Event event, const std::vector<uint8_t>& data)
{
   GenericListener::notifyChange([&](PortListener* l){l->onPortEvent({m_settings.port_id, m_settings.port_name, m_settings.trace_color, m_settings.font_color, event, data});});
}
Event Port::toPortEvent(Drivers::SocketClient::ClientEvent event)
{
   if (event == Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED)
   {
      return Event::DISCONNECTED;
   }
   else
   {
      return Event::NEW_DATA;
   }
}
Event Port::toPortEvent(Drivers::Serial::DriverEvent event)
{
   if (event == Drivers::Serial::DriverEvent::COMMUNICATION_ERROR)
   {
      return Event::SERIAL_DRIVER_ERROR;
   }
   else
   {
      return Event::NEW_DATA;
   }
}
const std::string& Port::getName()
{
   return m_settings.port_name;
}
bool Port::write(const std::vector<uint8_t>& data, size_t size)
{
   UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] writing %u bytes", m_settings.port_id, m_settings.port_name.c_str(), (uint32_t)data.size());
   if (m_socket->isConnected())
   {
      return m_socket->write(data, size);
   }
   else if (m_serial->isOpened())
   {
      return m_serial->write(data, size);
   }
   else
   {
      return false;
   }
}
void Port::refreshUi()
{
   handleNewSettings(m_settings);
}
bool Port::setSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   bool result = false;
   if (m_button_state == ButtonState::DISCONNECTED)
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
   return (m_button_state != ButtonState::DISCONNECTED);
}

void Port::onClientEvent(Drivers::SocketClient::ClientEvent event, const std::vector<uint8_t>& data, size_t)
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   m_events.push({m_settings.port_id, m_settings.port_name, m_settings.trace_color, m_settings.font_color, toPortEvent(event), data});
   onPortEvent();
}
void Port::onSerialEvent(Drivers::Serial::DriverEvent event, const std::vector<uint8_t>& data, size_t)
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   m_events.push({m_settings.port_id, m_settings.port_name, m_settings.trace_color, m_settings.font_color, toPortEvent(event), data});
   onPortEvent();
}
void Port::onPortEvent()
{
   while (m_events.size())
   {
      PortEvent& event = m_events.front();
      if (event.event == Event::DISCONNECTED)
      {
         UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] server disconnected, trying to reconnect", m_settings.port_id, m_settings.port_name.c_str());
         setButtonState(ButtonState::CONNECTING);
         notifyListeners(Event::CONNECTING);
         tryConnectToSocket();
      }
      else if (event.event == Event::NEW_DATA)
      {
         m_throughputCalculator.reportBytes(event.data.size());
         notifyListeners(Event::NEW_DATA, event.data);
      }
      else if(event.event == Event::SERIAL_DRIVER_ERROR)
      {
         UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] serial device disconnected, trying to reconnect", m_settings.port_id, m_settings.port_name.c_str());
         setButtonState(ButtonState::CONNECTING);
         notifyListeners(Event::CONNECTING);
         tryConnectToSerial();
      }
      else
      {
         UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Unknown event received %u", m_settings.port_id, m_settings.port_name.c_str(), (uint8_t) event.event);
      }
      m_events.pop();
   }
}
void Port::onTimeout(uint32_t timer_id)
{
   // another thread
   if (timer_id == m_timer_id)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Retrying connect...", m_settings.port_id, m_settings.port_name.c_str());
      if (m_settings.type.value == Dialogs::PortSettingDialog::PortType::SERIAL)
      {
         tryConnectToSerial();
      }
      else
      {
         tryConnectToSocket();
      }
   }

   if (timer_id == m_throughputCalculatorTimerID)
   {
      auto throughputCalculation = m_throughputCalculator.get();
      UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] Reading throughput, value %.5f unit %u", m_settings.port_id, m_settings.port_name.c_str(), throughputCalculation.value, (uint32_t)throughputCalculation.unit);
      m_throughputCalculator.reset();
      m_throughputCalculator.start();
      if (isOpened())
      {
         m_timers.startTimer(m_throughputCalculatorTimerID);
         reportThroughput(throughputCalculation);
      }
      else
      {
         UT_Log(PORT_HANDLER, HIGH, "PORT%u[%s] stopping throughput reporting", m_settings.port_id, m_settings.port_name.c_str());
         m_gui_controller.setThroughputText(m_settings.port_id, "");
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
   m_gui_controller.setThroughputText(m_settings.port_id, std::string(buffer));
}

void Port::handleNewSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   m_settings = settings;
   if (m_settings.port_name.empty())
   {
      /* override with default port name if not provided by user */
      m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);
   }

   m_gui_controller.setPortLabelText(m_settings.port_id, m_settings.shortSettingsString().c_str());

   char stylesheet [300];
   std::snprintf(stylesheet, 300, "background-color: #%.6x;color: #%.6x;border-width:2px;border-style:solid;border-radius:10px;border-color:gray;", settings.trace_color, settings.font_color);

   setButtonName(m_settings.port_name);
   setButtonState(m_button_state);
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] got new settings %s", m_settings.port_id, m_settings.port_name.c_str(), m_settings.shortSettingsString().c_str());
}
void Port::onPortButtonContextMenuRequested()
{
   Dialogs::PortSettingDialog dialog;
   Dialogs::PortSettingDialog::Settings new_settings = {};
   std::optional<bool> result = dialog.showDialog(m_gui_controller.getParent(), m_settings, new_settings, m_button_state == ButtonState::DISCONNECTED);
   if (result && result.value())
   {
      handleNewSettings(new_settings);
   }
}
void Port::onPortButtonClicked()
{
   if(m_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
   {
      handleButtonClickSerial();
   }
   else if (m_settings.type == Dialogs::PortSettingDialog::PortType::ETHERNET)
   {
      handleButtonClickEthernet();
   }
}
void Port::handleButtonClickSerial()
{
   UT_Assert(m_serial && "Serial client not created");

   if (m_serial->isOpened())
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to close serial port", m_settings.port_id, m_settings.port_name.c_str());
      m_serial->close();
      setButtonState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to open serial port", m_settings.port_id, m_settings.port_name.c_str());
      if (m_serial->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, m_settings.serialSettings))
      {
         UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] serial port opened correctly", m_settings.port_id, m_settings.port_name.c_str());
         setButtonState(ButtonState::CONNECTED);
         notifyListeners(Event::CONNECTED);
         m_throughputCalculator.start();
         m_timers.startTimer(m_throughputCalculatorTimerID);
      }
      else if (m_button_state == ButtonState::CONNECTING)
      {
         UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] aborting connection attempts!", m_settings.port_id, m_settings.port_name.c_str());
         m_timers.stopTimer(m_timer_id);
         setButtonState(ButtonState::DISCONNECTED);
         notifyListeners(Event::DISCONNECTED);
      }
      else
      {
         UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Cannot open serial", m_settings.port_id, m_settings.port_name.c_str());
         std::string message = "Cannot open " + m_settings.serialSettings.device + " [" + m_settings.port_name + "]\n" +
                               std::string(strerror(errno)) + " (" + std::to_string(errno) + ")";
         Dialogs::MessageDialog::show(Dialogs::MessageDialog::Icon::Critical, "Error", message, m_gui_controller.getApplicationPalette());
      }
   }
}
void Port::handleButtonClickEthernet()
{
   UT_Assert(m_socket && "Socket client not created");
   if (m_socket->isConnected())
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to disconnet socket", m_settings.port_id, m_settings.port_name.c_str());
      setButtonState(ButtonState::DISCONNECTED);
      m_socket->disconnect();
      notifyListeners(Event::DISCONNECTED);
   }
   else if (m_button_state == ButtonState::CONNECTING)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] aborting connection attempts!", m_settings.port_id, m_settings.port_name.c_str());
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to connect server", m_settings.port_id, m_settings.port_name.c_str());
      tryConnectToSocket();
   }
}
void Port::tryConnectToSocket()
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] %s", m_settings.port_id, m_settings.port_name.c_str(), __func__);
   m_socket->disconnect();
   if(m_socket->connect(m_settings.ip_address, m_settings.port))
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Successfully connected to %s:%u", m_settings.port_id,
                                                                              m_settings.port_name.c_str(),
                                                                              m_settings.ip_address.c_str(),
                                                                              m_settings.port);
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTED);
      notifyListeners(Event::CONNECTED);
      m_throughputCalculator.start();
      m_timers.startTimer(m_throughputCalculatorTimerID);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Cannot connect to %s:%u, scheduling retries with %u ms period",
                                                                                    m_settings.port_id,
                                                                                    m_settings.port_name.c_str(),
                                                                                    m_settings.ip_address.c_str(),
                                                                                    m_settings.port,
                                                                                    m_connect_retry_period);
      m_timers.setTimeout(m_timer_id, m_connect_retry_period);
      m_timers.startTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTING);
      notifyListeners(Event::CONNECTING);
   }
}
void Port::tryConnectToSerial()
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] %s", m_settings.port_id, m_settings.port_name.c_str(), __func__);
   m_serial->close();
   if(m_serial->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, m_settings.serialSettings))
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Successfully opened %s", m_settings.port_id, m_settings.port_name.c_str(), m_settings.serialSettings.device.c_str());
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTED);
      notifyListeners(Event::CONNECTED);
      m_throughputCalculator.start();
      m_timers.startTimer(m_throughputCalculatorTimerID);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Cannot connect to %s, scheduling retries with %u ms period",
                                                                                    m_settings.port_id,
                                                                                    m_settings.port_name.c_str(),
                                                                                    m_settings.serialSettings.device.c_str(),
                                                                                    m_connect_retry_period);
      m_timers.setTimeout(m_timer_id, m_connect_retry_period);
      m_timers.startTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTING);
      notifyListeners(Event::CONNECTING);
   }
}
void Port::setButtonState(ButtonState state)
{
   constexpr uint32_t GREEN = 0x00FF00;
   constexpr uint32_t BLUE = 0x0000FF;
   constexpr uint32_t BLACK = 0x000000;
   const uint32_t DEFAULT_BACKGROUND = m_gui_controller.getBackgroundColor();
   const uint32_t DEFAULT_FONT = m_gui_controller.getTextColor();

   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] setting button state %.6x", m_settings.port_id, m_settings.port_name.c_str(), (uint32_t)state);
   m_button_state = state;
   switch(m_button_state)
   {
      case ButtonState::CONNECTED:
      {
         m_gui_controller.setButtonBackgroundColor(m_button_id, GREEN);
         m_gui_controller.setButtonFontColor(m_button_id, BLACK);
         break;
      }
      case ButtonState::CONNECTING:
      {
         m_gui_controller.setButtonBackgroundColor(m_button_id, BLUE);
         m_gui_controller.setButtonFontColor(m_button_id, BLACK);
         break;
      }
      default:
      {
         m_gui_controller.setButtonBackgroundColor(m_button_id, DEFAULT_BACKGROUND);
         m_gui_controller.setButtonFontColor(m_button_id, DEFAULT_FONT);
         break;
      }
   }
}
void Port::setButtonName(const std::string name)
{
   m_gui_controller.setButtonText(m_button_id, name);
}
void Port::onPersistenceRead(const PersistenceItems& items)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] restoring persistence", m_settings.port_id, m_settings.port_name.c_str());

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
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] saving persistence [%s]", m_settings.port_id, m_settings.port_name.c_str(), m_settings.shortSettingsString().c_str());

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
