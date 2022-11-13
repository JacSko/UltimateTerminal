#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <optional>
#include <string.h>
#include <errno.h>

#include "Logger.h"
#include "PortHandler.h"
#include "Serialize.hpp"

namespace GUI
{

static uint8_t PORT_ID = 0;
constexpr uint32_t DEFAULT_CONNECT_RETRY_PERIOD = 1000;


PortHandler::PortHandler(QPushButton* object, QLabel* label, Utilities::ITimers& timers, PortHandlerListener* listener, QWidget* parent, Persistence::PersistenceHandler& persistence):
m_object(object),
m_summary_label(label),
m_parent(parent),
m_settings({}),
m_connect_retry_period(DEFAULT_CONNECT_RETRY_PERIOD),
m_timers(timers),
m_socket(Drivers::SocketFactory::createClient()),
m_serial(Drivers::Serial::ISerialDriver::create()),
m_timer_id(TIMERS_INVALID_ID),
m_persistence(persistence)
{
   PORT_ID++;

   UT_Assert(object && "invalid QObject pointer");
   GenericListener::addListener(*listener);
   m_socket->addListener(this);
   m_serial->addListener(this);
   m_timer_id = m_timers.createTimer(this, m_connect_retry_period);

   m_settings.port_id = PORT_ID;
   m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);
   UT_Log(PORT_HANDLER, INFO, "Creating port handler for PORT%u", m_settings.port_id);

   Persistence::PersistenceListener::setName(m_settings.port_name);
   m_persistence.addListener(*this);
   setButtonState(ButtonState::DISCONNECTED);
   setButtonName(m_settings.port_name);
   notifyListeners(Event::DISCONNECTED);

   object->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   m_summary_label->setAutoFillBackground(true);
   m_summary_label->setAlignment(Qt::AlignCenter);
   m_summary_label->setText(m_settings.shortSettingsString().c_str());
   QPalette palette = m_parent->palette();
   palette.setColor(QPalette::WindowText, Qt::black);
   m_summary_label->setPalette(palette);


   connect(object, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(object, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
   connect(this, SIGNAL(portEvent()), this, SLOT(onPortEvent()));
}
PortHandler::~PortHandler()
{
   m_persistence.removeListener(*this);
   GenericListener::removeAllListeners();
   m_socket->removeListener(this);
   m_serial->removeListener(this);
   m_timers.removeTimer(m_timer_id);
   m_timer_id = TIMERS_INVALID_ID;
   m_socket->disconnect();
   m_serial->close();
   setButtonState(ButtonState::DISCONNECTED);
}
void PortHandler::notifyListeners(Event event, const std::vector<uint8_t>& data)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] - Notifying listeners, event %u", m_settings.port_id, m_settings.port_name.c_str(), (uint8_t)event);
   GenericListener::notifyChange([&](PortHandlerListener* l){l->onPortHandlerEvent({m_settings.port_id, m_settings.port_name, m_settings.trace_color, event, data});});
}
Event PortHandler::toPortHandlerEvent(Drivers::SocketClient::ClientEvent event)
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
Event PortHandler::toPortHandlerEvent(Drivers::Serial::DriverEvent)
{
   return Event::NEW_DATA;
}
const std::string& PortHandler::getName()
{
   return m_settings.port_name;
}
bool PortHandler::write(const std::vector<uint8_t>& data, size_t size)
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
void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t)
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   m_events.push({m_settings.port_id, m_settings.port_name, m_settings.trace_color, toPortHandlerEvent(ev), data});
   emit portEvent();
}
void PortHandler::onSerialEvent(Drivers::Serial::DriverEvent ev, const std::vector<uint8_t>& data, size_t)
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   m_events.push({m_settings.port_id, m_settings.port_name, m_settings.trace_color, toPortHandlerEvent(ev), data});
   emit portEvent();
}
void PortHandler::onPortEvent()
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   while (m_events.size())
   {
      PortHandlerEvent& event = m_events.front();
      if (event.event == Event::DISCONNECTED)
      {
         UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] server disconnected, trying to reconnect", m_settings.port_id, m_settings.port_name.c_str());
         m_socket->disconnect();
         setButtonState(ButtonState::CONNECTING);
         notifyListeners(Event::CONNECTING);
         tryConnectToSocket();
      }
      else if (event.event == Event::NEW_DATA)
      {
         notifyListeners(Event::NEW_DATA, event.data);
      }
      else
      {
         UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Unknown event received %u", m_settings.port_id, m_settings.port_name.c_str(), (uint8_t) event.event);
      }
      m_events.pop();
   }
}
void PortHandler::onTimeout(uint32_t timer_id)
{
   // another thread
   if (timer_id == m_timer_id)
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Retrying connect...", m_settings.port_id, m_settings.port_name.c_str());
      tryConnectToSocket();
   }
}
void PortHandler::handleNewSettings(const Dialogs::PortSettingDialog::Settings& settings)
{
   m_settings = settings;
   if (m_settings.port_name.empty())
   {
      /* override with default port name if not provided by user */
      m_settings.port_name = std::string("PORT") + std::to_string(m_settings.port_id);
   }

   m_summary_label->setText(m_settings.shortSettingsString().c_str());

   char stylesheet [100];
   std::snprintf(stylesheet, 100, "background-color: #%.6x;border-width:2px;border-style:solid;border-radius:10px;border-color:gray;", settings.trace_color);

   m_summary_label->setStyleSheet(QString(stylesheet));
   setButtonName(m_settings.port_name);
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] got new settings %s", m_settings.port_id, m_settings.port_name.c_str(), m_settings.shortSettingsString().c_str());
}
void PortHandler::onPortButtonContextMenuRequested()
{
   Dialogs::PortSettingDialog dialog;
   Dialogs::PortSettingDialog::Settings new_settings = {};
   std::optional<bool> result = dialog.showDialog(m_parent, m_settings, new_settings, m_button_state == ButtonState::DISCONNECTED);
   if (result)
   {
      if (result.value())
      {
         handleNewSettings(new_settings);
      }
      else
      {
         QMessageBox messageBox;
         QString error_message = "";
         auto errors = new_settings.getErrorStrings();
         for (const auto& error : errors)
         {
            error_message += error.c_str();
            error_message += "\n";
         }
         messageBox.critical(m_parent,"Error", error_message, "OK");
      }
   }
}
void PortHandler::onPortButtonClicked()
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
void PortHandler::handleButtonClickSerial()
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
      }
      else
      {
         UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Cannot open serial", m_settings.port_id, m_settings.port_name.c_str());
         QMessageBox messageBox;
         QString error_message = QString().asprintf("Cannot open %s [%s]\n%s (%u)", m_settings.serialSettings.device.c_str(), m_settings.port_name.c_str(), strerror(errno), errno);
         messageBox.critical(m_parent,"Error", error_message, "OK");
      }
   }
}
void PortHandler::handleButtonClickEthernet()
{
   UT_Assert(m_socket && "Socket client not created");
   if (m_socket->isConnected())
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] trying to disconnet socket", m_settings.port_id, m_settings.port_name.c_str());
      m_socket->disconnect();
      setButtonState(ButtonState::DISCONNECTED);
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
void PortHandler::tryConnectToSocket()
{
   if(m_socket->connect(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER, m_settings.ip_address, m_settings.port))
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Successfully connected to %s:%u", m_settings.port_id, m_settings.port_name.c_str(), m_settings.ip_address.c_str(), m_settings.port);
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTED);
      notifyListeners(Event::CONNECTED);
   }
   else
   {
      UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] Cannot connect to %s:%u, scheduling retries with %u ms period", m_settings.port_id, m_settings.port_name.c_str(), m_settings.ip_address.c_str(), m_settings.port, m_connect_retry_period);
      m_timers.setTimeout(m_timer_id, m_connect_retry_period);
      m_timers.startTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTING);
      notifyListeners(Event::CONNECTING);
   }
}
void PortHandler::setButtonState(ButtonState state)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] setting button state %.6x", m_settings.port_id, m_settings.port_name.c_str(), (uint32_t)state);
   m_button_state = state;
   switch(m_button_state)
   {
      case ButtonState::CONNECTED:
      {
         QPalette palette = m_parent->palette();
         palette.setColor(QPalette::Button, Qt::green);
         palette.setColor(QPalette::ButtonText, Qt::black);
         m_object->setPalette(palette);
         m_object->update();
         break;
      }
      case ButtonState::CONNECTING:
      {
         QPalette palette = m_parent->palette();
         palette.setColor(QPalette::Button, Qt::blue);
         palette.setColor(QPalette::ButtonText, Qt::black);
         m_object->setPalette(palette);
         m_object->update();
         break;
      }
      default:
      {
         m_object->setPalette(m_parent->palette());
         m_object->update();
         break;
      }
   }
}
void PortHandler::setButtonName(const std::string name)
{
   m_object->setText(QString(name.c_str()));
}
void PortHandler::onPersistenceRead(const std::vector<uint8_t>& data)
{
   uint32_t offset = 0;
   Dialogs::PortSettingDialog::Settings new_settings = {};

   std::string baudrate_name;
   std::string databits_name;
   std::string paritybits_name;
   std::string stopbits_name;
   std::string porttype_name;

   ::deserialize(data, offset, new_settings.ip_address);
   ::deserialize(data, offset, new_settings.port);
   ::deserialize(data, offset, new_settings.port_id);
   ::deserialize(data, offset, new_settings.port_name);
   ::deserialize(data, offset, baudrate_name);
   ::deserialize(data, offset, databits_name);
   ::deserialize(data, offset, new_settings.serialSettings.device);
   ::deserialize(data, offset, (uint8_t&)new_settings.serialSettings.mode);
   ::deserialize(data, offset, paritybits_name);
   ::deserialize(data, offset, stopbits_name);
   ::deserialize(data, offset, new_settings.trace_color);
   ::deserialize(data, offset, porttype_name);

   new_settings.serialSettings.baudRate.fromName(baudrate_name);
   new_settings.serialSettings.dataBits.fromName(databits_name);
   new_settings.serialSettings.parityBits.fromName(paritybits_name);
   new_settings.serialSettings.stopBits.fromName(stopbits_name);
   new_settings.type.fromName(porttype_name);

   if (new_settings.areValid())
   {
      handleNewSettings(new_settings);
   }
   else
   {
      UT_Log(PORT_HANDLER, ERROR, "PORT%u[%s] Error restoring from persistence, errors:", m_settings.port_id, m_settings.port_name.c_str());
      auto errors = new_settings.getErrorStrings();
      for (auto& error : errors)
      {
         UT_Log(PORT_HANDLER, ERROR, "%s", error.c_str());
      }
   }

}
void PortHandler::onPersistenceWrite(std::vector<uint8_t>& data)
{
   UT_Log(PORT_HANDLER, LOW, "PORT%u[%s] saving persistence", m_settings.port_id, m_settings.port_name.c_str());
   serialize(data);
}
void PortHandler::serialize(std::vector<uint8_t>& buffer)
{
   ::serialize(buffer, m_settings.ip_address);
   ::serialize(buffer, m_settings.port);
   ::serialize(buffer, m_settings.port_id);
   ::serialize(buffer, m_settings.port_name);
   ::serialize(buffer, m_settings.serialSettings.baudRate.toName());
   ::serialize(buffer, m_settings.serialSettings.dataBits.toName());
   ::serialize(buffer, m_settings.serialSettings.device);
   ::serialize(buffer, (uint8_t)m_settings.serialSettings.mode);
   ::serialize(buffer, m_settings.serialSettings.parityBits.toName());
   ::serialize(buffer, m_settings.serialSettings.stopBits.toName());
   ::serialize(buffer, m_settings.trace_color);
   ::serialize(buffer, m_settings.type.toName());
}


};
