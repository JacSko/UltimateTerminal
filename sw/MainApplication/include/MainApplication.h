#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>
#include <map>

#include "ui_MainWindow.h"
#include "ITimers.h"
#include "Port.h"
#include "LoggingSettingDialog.h"
#include "Persistence.h"
#include "UserButtonsTab.h"
#include "TraceFilter.h"
#include "IFileLogger.h"
#include "GUIController.h"

namespace MainApplication
{

class MainApplication : public PortListener,
                        public Utilities::Persistence::PersistenceListener,
                        public GUIController::ButtonEventListener,
                        public GUIController::ThemeListener
{
public:
    MainApplication();
    ~MainApplication();

    static const uint32_t MAX_COMMANDS_HISTORY_ITEMS = 100;
private:

   struct ButtonListener
   {
      uint32_t button_id;
      GUIController::ButtonEvent event;
      std::function<void()> listener;
   };
   std::unique_ptr<Utilities::ITimers> m_timers;
   std::unique_ptr<IFileLogger> m_file_logger;
   GUIController::GUIController m_gui_controller;
   std::vector<std::unique_ptr<Port>> m_ports;
   std::mutex m_ports_mutex;
   std::vector<std::unique_ptr<UserButtonsTab>> m_user_buttons_tabs;
   std::vector<std::unique_ptr<TraceFilter>> m_trace_filters;
   uint32_t m_marker_index;
   std::string m_file_logging_path;
   std::string m_log_file_name;
   Utilities::Persistence::Persistence m_persistence;
   uint32_t m_scroll_default_color;
   std::map<uint8_t, std::vector<std::string>> m_commands_history;
   std::vector<ButtonListener> m_button_listeners;

   uint32_t m_marker_button_id;
   uint32_t m_logging_button_id;
   uint32_t m_settings_button_id;
   uint32_t m_send_button_id;
   uint32_t m_scroll_button_id;
   uint32_t m_clear_button_id;
   uint32_t m_trace_clear_button;
   uint32_t m_trace_scroll_button;
   std::string m_current_port_name;

   /* ButtonEventListener */
   void onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event) override;
   /* ThemeChangedListener */
   void onThemeChange(Theme theme) override;

   void onPortEvent(const PortEvent&) override;
   bool sendToPort(int8_t portId, const std::string&);
   void addToTerminal(const std::string& port_name, const std::string& data, uint32_t background_color, uint32_t font_color);
   void setButtonState(uint32_t button_id, bool active);
   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void addToCommandHistory(uint8_t port_id, const std::string& text);
   void removeCommandHistoryDuplicates(std::vector<std::string>& history, const std::string& text);
   uint8_t portNameToId(const std::string& name);
   std::string createLogFileName();
   std::string getPersistenceFile();

   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onBottomButtonClicked();
   void onClearButtonClicked();
   void onTraceBottomButtonClicked();
   void onTraceClearButtonClicked();
   void onSendButtonClicked();
   void onLoggingButtonContextMenuRequested();
   bool onCurrentPortSelectionChanged(const std::string& name);
   void onPortSwitchRequest();
   void onSettingsButtonClicked();

};

}
#endif
