#pragma once

#include <functional>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "ISocketDriverFactory.h"
#include "ISerialDriver.h"
#include "PortSettingDialog.h"
#include "ITimers.h"

namespace GUI
{

class PortHandler : public QObject,
                    public Drivers::SocketClient::ClientListener,
                    public Drivers::Serial::SerialListener,
                    public Utilities::ITimerClient
{
   Q_OBJECT
public:
   enum class Event
   {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
      NEW_DATA,
   };

   struct PortHandlerEvent
   {
      const std::string& name;
      uint32_t trace_color;
      Event event;
      const std::vector<uint8_t>& data;
      size_t size;
   };

   typedef std::function<void(const PortHandlerEvent&)> PortHandlerListener;
   PortHandler(QPushButton* object, QLabel* label, Utilities::ITimers& timer, PortHandlerListener listener, QWidget* parent);
   ~PortHandler();

   const std::string& getName();
   bool write(const std::vector<uint8_t>& data, size_t size = 0);
private:

   enum class ButtonState
   {
      DISCONNECTED = 0xFF0000,
      CONNECTING = 0x0000FF,
      CONNECTED = 0x00FF00,
   };

   struct SocketClientEvent
   {
      Drivers::SocketClient::ClientEvent event;
      std::vector<uint8_t> data;
      size_t size;
   };

   struct SerialPortEvent
   {
      Drivers::Serial::DriverEvent event;
      std::vector<uint8_t> data;
      size_t size;
   };

   QPushButton* m_object;
   QLabel* m_summary_label;
   QWidget* m_parent;
   PortSettingDialog::Settings m_settings;
   uint32_t m_connect_retry_period;
   Utilities::ITimers& m_timers;
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socket;
   std::unique_ptr<Drivers::Serial::ISerialDriver> m_serial;
   int m_timer_id;
   ButtonState m_button_state;
   SocketClientEvent m_last_event;
   SerialPortEvent m_last_serial_event;
   std::mutex m_event_mutex;
   PortHandlerListener m_listener;
   std::mutex m_listener_mutex;
   uint8_t m_port_id;

   void onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size);
   void onSerialEvent(Drivers::Serial::DriverEvent ev, const std::vector<uint8_t>& data, size_t size);
   void onTimeout(uint32_t timer_id);
   void tryConnectToSocket();
   void handleNewSettings(const PortSettingDialog::Settings&);
   void handleButtonClickSerial();
   void handleButtonClickEthernet();
   void setButtonState(ButtonState);
   void setButtonName(const std::string name);
   void notifyListeners(Event event, const std::vector<uint8_t>& data = {}, size_t size = 0);
public slots:
   void onPortButtonContextMenuRequested();
   void onPortButtonClicked();
   void onPortEvent();
   void onSerialPortEvent();
signals:
   void portEvent();
   void serialPortEvent();
};



}
