#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <optional>

#include "Logger.h"
#include "PortHandler.h"

namespace GUI
{

constexpr uint32_t DEFAULT_CONNECT_RETRY_PERIOD = 1000;

PortHandler::PortHandler(QPushButton* object, Utilities::ITimers& timers, QWidget* parent):
m_object(object),
m_parent(parent),
m_settings({}),
m_connect_retry_period(DEFAULT_CONNECT_RETRY_PERIOD),
m_timers(timers),
m_socket(Drivers::SocketFactory::createClient()),
m_timer_id(TIMERS_INVALID_ID)
{
   UT_Assert(object && "invalid QObject pointer");
   m_socket->addListener(this);
   m_timer_id = m_timers.createTimer(this, m_connect_retry_period);
   setButtonColor(ButtonColor::DISCONNECTED);

   object->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(object, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(object, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
}
PortHandler::~PortHandler()
{
   m_socket->removeListener(this);
   m_timers.removeTimer(m_timer_id);
   m_timer_id = TIMERS_INVALID_ID;
   m_socket->disconnect();
   setButtonColor(ButtonColor::DISCONNECTED);
}
void PortHandler::onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   //another thread
   if (ev == Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED)
   {
      m_socket->disconnect();
      setButtonColor(ButtonColor::DISCONNECTED);
   }
   else if (ev == Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV)
   {
      UT_Log(MAIN_GUI, LOW, "got data, size %u", data.size());
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
}
void PortHandler::onPortButtonContextMenuRequested()
{
   PortSettingDialog dialog;
   PortSettingDialog::Settings new_settings = {};
   std::optional<bool> result = dialog.showDialog(m_parent, m_settings, new_settings, !m_socket->isConnected());
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
      setButtonColor(ButtonColor::CONNECTED);
   }
   else
   {
      UT_Log(MAIN_GUI, LOW, "Cannot connect to %s:%u, scheduling retries with %u ms period", m_settings.ip_address.c_str(), m_settings.port, m_connect_retry_period);
      m_timers.setTimeout(m_timer_id, m_connect_retry_period);
      m_timers.startTimer(m_timer_id);
      setButtonColor(ButtonColor::CONNECTING);
   }
}

void PortHandler::setButtonColor(ButtonColor color)
{
   static constexpr uint32_t stylesheet_buffer_size = 50;
   char stylesheet [stylesheet_buffer_size] = {};
   std::snprintf(stylesheet, stylesheet_buffer_size, "background: #%.6x;'", (uint32_t)color);

   m_object->setStyleSheet(QString(stylesheet));
}

};
