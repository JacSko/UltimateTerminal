#pragma once

#include <functional>
#include <queue>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QShortcut>

#include "GenericListener.hpp"
#include "ISocketDriverFactory.h"
#include "ISerialDriver.h"
#include "PortSettingDialog.h"
#include "ITimers.h"
#include "PersistenceHandler.h"

/**
 * @file PortHandler.h
 *
 * @brief
 *    Class is responsible for handling communication over serial or socket backend basing on configuration.
 *
 * @details
 *    PortHandler handles the GUI elements directly related to port instance (QPushButton and QLabel) - it controls the button color, button text, label text and label stylesheet.
 *    Subscribes for clicked() and customContextMenuRequested() signals of push button.
 *    When context menu is requested, it shows the settings dialog that allows to configure the port. This dialog is not editable when port is already opened.
 *    On push button clicked() signal it opens the port according to configuration retrieved from user.
 *    If socket driver is used and remote server gets closed or it is not available at all, PortHandler keeps reconnecting trials until user action (disabling port).
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

namespace GUI
{

/** Event from PortHandler */
enum class Event
{
   DISCONNECTED, /**< Port is disconnected        */
   CONNECTING,   /**< Port connection started     */
   CONNECTED,    /**< Port is connected           */
   NEW_DATA,     /**< New data received from port */
};

/** Data structure used for notifying listeners */
struct PortHandlerEvent
{
   uint8_t port_id;           /**< unique ID of PortHandler - incremented with every new instance of the object */
   std::string name;          /**< user-defined port name */
   uint32_t background_color; /**< Trace background color */
   uint32_t font_color;       /**< Trace font color       */
   Event event;               /**< ID of the event        */
   std::vector<uint8_t> data; /**< Data related to event  */
};

/** Interface that provides the PortHandler events */
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
   /**
    * @brief Creates object.
    * @param[in] object - Button related to port.
    * @param[in] label - Label related to port.
    * @param[in] shortcut - Key shortcut object.
    * @param[in] timer - timers instance.
    * @param[in] listener - pointer to event listener.
    * @param[in] parent - parent widget.
    * @param[in] persistence - persistence instance.
    * @param[in] callback - function called on command execution finish
    *
    * @return None.
    */
   PortHandler(QPushButton* object,
               QLabel* label,
               QShortcut* shortcut,
               Utilities::ITimers& timer,
               PortHandlerListener* listener,
               QWidget* parent,
               Persistence::PersistenceHandler& persistence);
   ~PortHandler();
   /**
    * @brief Returns the user-defined name of port.
    *
    * @return User-defined name.
    */
   const std::string& getName();
   /**
    * @brief Writes data to port.
    * @param[in] data - buffer with data to send.
    * @param[in[ size - size of the buffer.
    * @return True if data written successfully.
    */
   bool write(const std::vector<uint8_t>& data, size_t size = 0);
   /**
    * @brief Refresh the related UI elements.
    * @return None.
    */
   void refreshUi();
private:

   enum class ButtonState
   {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
   };

   QPushButton* m_object;
   QLabel* m_summary_label;
   QShortcut* m_shortcut;
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
