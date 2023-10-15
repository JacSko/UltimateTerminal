#pragma once

#include <functional>
#include <queue>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QShortcut>

#include "GenericListener.hpp"
#include "PortSettingDialog.h"
#include "ITimers.h"
#include "Persistence.h"
#include "GUIController.h"
#include "ThroughputCalculator.h"
#include "DriversProxy.hpp"

/**
 * @file Port.h
 *
 * @brief
 *       Class is responsible for handling communication over serial or socket backend basing on
 *    configuration, as well as controlling the GUI state of the port.
 *
 * @details
 *       Port handles the GUI elements directly via GUIController interface - it controls the button
 *    color, button text, label text and label stylesheet.
 *    Subscribes for clicked() and customContextMenuRequested() signals to get the events from user.
 *
 *    When context menu is requested, it shows the settings dialog that allows to configure the port.
 *       This dialog is not editable when port is already opened.
 *
 *    On push button clicked() signal it opens the port according to configuration retrieved from user.
 *    If port cannot be opened, the behavior is different depending on type:
 *
 *    -> Ethernet - port is switched into auto-reconnecting mode. This means that it keeps the reconnection
 *                  trials in loop until successfull connection or user abort
 *    -> Serial port - Popup with detailed error displayed.
 *
 *    When port is already connected and remote endpoint get disconnected (server or serial port disconnected)
 *    port is switching into auto-reconnecting mode despite the communicaton type.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

namespace MainApplication
{

/** Event from Port */
enum class Event
{
   DISCONNECTED,            /**< Port is disconnected        */
   CONNECTING,              /**< Port connection started     */
   CONNECTED,               /**< Port is connected           */
   NEW_DATA,                /**< New data received from port */
};

/** Data structure used for notifying listeners */
struct PortEvent
{
   uint8_t port_id;           /**< unique ID of Port - incremented with every new instance of the object */
   std::string name;          /**< user-defined port name */
   uint32_t background_color; /**< Trace background color */
   uint32_t font_color;       /**< Trace font color       */
   Event event;               /**< ID of the event        */
   std::vector<uint8_t> data; /**< Data related to event  */
};

/** Interface that provides the Port events */
class PortListener
{
public:
   virtual ~PortListener(){};
   virtual void onPortEvent(const PortEvent&) = 0;
};

class Port : public QObject,
                    public Utilities::GenericListener<PortListener>,
                    public Utilities::ITimerClient,
                    public DriversProxy::Listener,
                    public Utilities::Persistence::PersistenceListener,
                    public GUIController::ButtonEventListener
{
   Q_OBJECT
public:
   /**
    * @brief Creates object.
    * @param[in] id - ID of the port.
    * @param[in] gui_controller - GUI controlling object.
    * @param[in] button_name - name of the button.
    * @param[in] timer - timers instance.
    * @param[in] listener - pointer to event listener.
    * @param[in] persistence - persistence instance.
    *
    * @return None.
    */
   Port(uint8_t id,
        GUIController::GUIController& gui_controller,
        const std::string& button_name,
        Utilities::ITimers& timers,
        PortListener* listener,
        Utilities::Persistence::Persistence& persistence);

   ~Port();
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
   /**
    * @brief Sets new settings to Port.
    * @details Settings can be set only if port is not opened.
    * @return True if settings accepted.
    */
   bool setSettings(const Dialogs::PortSettingDialog::Settings& settings);
   /**
    * @brief Return the current Port settings.
    * @return Const reference to setting.
    */
   const Dialogs::PortSettingDialog::Settings& getSettings();
   /**
    * @brief Checks if port is opened.
    * @return True if port is opened (or even in Connecting state).
    */
   bool isOpened();

private:

   enum class ButtonState
   {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
   };

   uint32_t m_buttonId;
   GUIController::GUIController& m_guiController;
   Dialogs::PortSettingDialog::Settings m_settings;
   uint32_t m_connectRetryPeriod;
   Utilities::ITimers& m_timers;
   std::unique_ptr<DriversProxy> m_proxy;
   uint32_t m_timerId;
   std::atomic<ButtonState> m_buttonState;
   std::mutex m_listenerMutex;
   Utilities::Persistence::Persistence& m_persistence;
   Utilities::ThroughputCalculator m_throughputCalculator;
   uint32_t m_throughputCalculatorTimerID;
   /* ButtonEventListener */
   void onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event) override;

   void onEvent(DriversProxy::Event event, const std::vector<uint8_t>& data, uint32_t size) override;
   void onTimeout(uint32_t timer_id) override;
   void handleNewSettings(const Dialogs::PortSettingDialog::Settings&);
   bool tryConnect();
   void onSerialConnectionFailed();
   void onEthernetConnectionFailed();
   void setButtonColors(ButtonState);
   void setButtonName(const std::string name);
   void setState(ButtonState state);
   void notifyListeners(Event event, const std::vector<uint8_t>& data = {});
   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void reportThroughput(const Utilities::ThroughputResult& throughput);
   void scheduleConnectionTrial();
   void onPortButtonContextMenuRequested();
   void onPortButtonClicked();
};



}
