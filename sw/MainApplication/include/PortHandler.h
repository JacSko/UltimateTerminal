#pragma once

#include <QtCore/QObject>
#include "ISocketDriverFactory.h"
#include "PortSettingDialog.h"
#include "ITimers.h"

namespace GUI
{

class PortHandler : public QObject,
                    public Drivers::SocketClient::ClientListener,
                    public Utilities::ITimerClient
{
   Q_OBJECT
public:
   PortHandler(QPushButton* object, Utilities::ITimers& timer, QWidget* parent);
   ~PortHandler();

private:

   enum class ButtonColor
   {
      DISCONNECTED = 0xFF0000,
      CONNECTING = 0x0000FF,
      CONNECTED = 0x00FF00,
   };

   QPushButton* m_object;
   QWidget* m_parent;
   PortSettingDialog::Settings m_settings;
   uint32_t m_connect_retry_period;
   Utilities::ITimers& m_timers;
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socket;
   int m_timer_id;

   void onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size);
   void onTimeout(uint32_t timer_id);
   void tryConnectToSocket();
   void handleNewSettings(const PortSettingDialog::Settings&);
   void handleButtonClickSerial();
   void handleButtonClickEthernet();
   void setButtonColor(ButtonColor);
public slots:
   void onPortButtonContextMenuRequested();
   void onPortButtonClicked();
};



}
