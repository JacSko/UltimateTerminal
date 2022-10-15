#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <optional>

#include "Logger.h"
#include "PortHandler.h"

namespace GUI
{

constexpr uint32_t DEFAULT_CONNECT_RETRY_PERIOD = 1000;

PortHandler::PortHandler(QPushButton* object, QLabel* label, Utilities::ITimers& timers, PortHandlerListener listener, QWidget* parent):
m_object(object),
m_summary_label(label),
m_parent(parent),
m_settings({}),
m_connect_retry_period(DEFAULT_CONNECT_RETRY_PERIOD),
m_timers(timers),
m_socket(Drivers::SocketFactory::createClient()),
m_timer_id(TIMERS_INVALID_ID),
m_listener(listener)
{
   UT_Assert(object && "invalid QObject pointer");
   m_socket->addListener(this);
   m_timer_id = m_timers.createTimer(this, m_connect_retry_period);
   setButtonState(ButtonState::DISCONNECTED);
   notifyListeners(Event::DISCONNECTED);

   object->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   m_summary_label->setAutoFillBackground(true);
   m_summary_label->setAlignment(Qt::AlignCenter);
   connect(object, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(object, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));

   connect(this, SIGNAL(portEvent()), this, SLOT(onPortEvent()));
}
PortHandler::~PortHandler()
{
   m_socket->removeListener(this);
   m_timers.removeTimer(m_timer_id);
   m_timer_id = TIMERS_INVALID_ID;
   m_socket->disconnect();
   setButtonState(ButtonState::DISCONNECTED);
   notifyListeners(Event::DISCONNECTED);
}
void PortHandler::notifyListeners(Event event)
{
   if (m_listener) m_listener({m_settings.port_name, m_settings.trace_color, event, m_last_event.data, m_last_event.size});
}
void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   m_last_event = {ev, data, size};
   emit portEvent();
}
void PortHandler::onPortEvent()
{
   std::lock_guard<std::mutex> lock(m_event_mutex);
   if (m_last_event.event == Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED)
   {
      m_socket->disconnect();
      setButtonState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else if (m_last_event.event == Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV)
   {
      notifyListeners(Event::NEW_DATA);
   }
   else
   {
      UT_Log(MAIN_GUI, ERROR, "Unknown event received %u", (uint8_t) m_last_event.event);
   }
}
void PortHandler::onTimeout(uint32_t timer_id)
{
   // another thread
   if (timer_id == m_timer_id)
   {
      UT_Log(MAIN_GUI, LOW, "Retrying connect...");
      tryConnectToSocket();
   }
}
void PortHandler::handleNewSettings(const PortSettingDialog::Settings& settings)
{
   m_settings = settings;
   m_summary_label->setText(m_settings.shortSettingsString().c_str());

   char stylesheet [200];
   std::snprintf(stylesheet, 100, "background-color: #%.6x;border-width:2px;border-style:solid;border-radius:10px;border-color:gray;", settings.trace_color);

   m_summary_label->setStyleSheet(QString(stylesheet));

//   QPalette palette = m_summary_label->palette();
//   palette.setColor(QPalette::Window, QColor(settings.trace_color));
//   m_summary_label->setPalette(palette);
//   m_summary_label->update();
}
void PortHandler::onPortButtonContextMenuRequested()
{
   PortSettingDialog dialog;
   PortSettingDialog::Settings new_settings = {};
   std::optional<bool> result = dialog.showDialog(m_parent, m_settings, new_settings, m_button_state == ButtonState::DISCONNECTED);
   if (result)
   {
      if (result.value())
      {
         UT_Log(MAIN, LOW, "New settings: %s", std::string(new_settings).c_str(), result.value());
         handleNewSettings(new_settings);
         m_settings = new_settings;
      }
      else
      {
         QMessageBox messageBox;
         QString error_message = "Invalid port settings:\n";
         error_message += QString(std::string(new_settings).c_str());
         messageBox.critical(m_parent,"Error", error_message);
      }
   }
}
void PortHandler::onPortButtonClicked()
{
   if(m_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      handleButtonClickSerial();
   }
   else if (m_settings.type == PortSettingDialog::PortType::ETHERNET)
   {
      handleButtonClickEthernet();
   }
}
void PortHandler::handleButtonClickSerial()
{
   UT_Log(MAIN_GUI, ERROR, "not implemented");
}
void PortHandler::handleButtonClickEthernet()
{
   UT_Assert(m_socket && "Socket client not created");
   if (m_socket->isConnected())
   {
      m_socket->disconnect();
      setButtonState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else if (m_button_state == ButtonState::CONNECTING)
   {
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::DISCONNECTED);
      notifyListeners(Event::DISCONNECTED);
   }
   else
   {
      tryConnectToSocket();
   }
}

void PortHandler::tryConnectToSocket()
{
   if(m_socket->connect(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER, m_settings.ip_address, m_settings.port))
   {
      UT_Log(MAIN_GUI, LOW, "Successfully connected to %s:%u", m_settings.ip_address.c_str(), m_settings.port);
      m_timers.stopTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTED);
      notifyListeners(Event::CONNECTED);
   }
   else
   {
      UT_Log(MAIN_GUI, LOW, "Cannot connect to %s:%u, scheduling retries with %u ms period", m_settings.ip_address.c_str(), m_settings.port, m_connect_retry_period);
      m_timers.setTimeout(m_timer_id, m_connect_retry_period);
      m_timers.startTimer(m_timer_id);
      setButtonState(ButtonState::CONNECTING);
      notifyListeners(Event::CONNECTING);
   }
}

void PortHandler::setButtonState(ButtonState state)
{
   if(state != m_button_state)
   {
      m_button_state = state;
      QPalette palette = m_object->palette();
      palette.setColor(QPalette::Button, QColor((uint32_t)state));
      m_object->setPalette(palette);
      m_object->update();
   }
}

};
