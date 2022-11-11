#pragma once

#include <functional>
#include <queue>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "GenericListener.hpp"
#include "ISocketDriverFactory.h"
#include "ISerialDriver.h"
#include "PortSettingDialog.h"
#include "ITimers.h"
#include "PersistenceHandler.h"

namespace GUI
{

enum class Event
{
   DISCONNECTED,
   CONNECTING,
   CONNECTED,
   NEW_DATA,
};

struct PortHandlerEvent
{
   uint8_t port_id;
   std::string name;
   uint32_t trace_color;
   Event event;
   std::vector<uint8_t> data;
};

class PortHandlerListener
{
public:
   virtual ~PortHandlerListener(){};
   virtual void onPortHandlerEvent(const PortHandlerEvent&) = 0;
};

class PortHandler : public QObject,
                    public GenericListener<PortHandlerListener>,
                    public Drivers::SocketClient::ClientListener,
                    public Drivers::Serial::SerialListener,
                    public Utilities::ITimerClient,
                    public Persistence::PersistenceListener
{
   Q_OBJECT
public:
   PortHandler(QPushButton* object, QLabel* label, Utilities::ITimers& timer, PortHandlerListener* listener, QWidget* parent, Persistence::PersistenceHandler& persistence);
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

   QPushButton* m_object;
   QLabel* m_summary_label;
   QWidget* m_parent;
   Dialogs::PortSettingDialog::Settings m_settings;
   uint32_t m_connect_retry_period;
   Utilities::ITimers& m_timers;
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socket;
   std::unique_ptr<Drivers::Serial::ISerialDriver> m_serial;
   uint32_t m_timer_id;
   ButtonState m_button_state;
   std::queue<PortHandlerEvent> m_events;
   std::mutex m_event_mutex;
   std::mutex m_listener_mutex;
   Persistence::PersistenceHandler& m_persistence;

   void onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size);
   void onSerialEvent(Drivers::Serial::DriverEvent ev, const std::vector<uint8_t>& data, size_t size);
   void onTimeout(uint32_t timer_id);
   void tryConnectToSocket();
   void handleNewSettings(const Dialogs::PortSettingDialog::Settings&);
   void handleButtonClickSerial();
   void handleButtonClickEthernet();
   void setButtonState(ButtonState);
   void setButtonName(const std::string name);
   void notifyListeners(Event event, const std::vector<uint8_t>& data = {});
   Event toPortHandlerEvent(Drivers::SocketClient::ClientEvent);
   Event toPortHandlerEvent(Drivers::Serial::DriverEvent);
   void onPersistenceRead(const std::vector<uint8_t>& data) override;
   void onPersistenceWrite(std::vector<uint8_t>& data) override;
   void serialize(std::vector<uint8_t>& buffer);
public slots:
   void onPortButtonContextMenuRequested();
   void onPortButtonClicked();
   void onPortEvent();
signals:
   void portEvent();
};



}
