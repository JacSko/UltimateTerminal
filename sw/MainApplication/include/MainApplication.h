#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>
#include <map>

#include "ui_MainWindow.h"
#include "ITimers.h"
#include "PortHandler.h"
#include "LoggingSettingDialog.h"
#include "PersistenceHandler.h"
#include "UserButtonHandler.h"
#include "TraceFilterHandler.h"
#include "IFileLogger.h"
#include "GUIController.h"

class MainApplication : public GUI::PortHandlerListener,
                        public Persistence::PersistenceListener,
                        public ButtonEventListener,
                        public ThemeListener
{
public:
    MainApplication();
    ~MainApplication();

    static const uint32_t MAX_COMMANDS_HISTORY_ITEMS = 100;
private:

   struct ButtonListener
   {
    uint32_t button_id;
    ButtonEvent event;
    std::function<void()> listener;
   };

   std::unique_ptr<Utilities::ITimers> m_timers;
   std::unique_ptr<IFileLogger> m_file_logger;
   GUIController m_gui_controller;
   std::vector<std::unique_ptr<GUI::PortHandler>> m_port_handlers;
   std::mutex m_port_handler_mutex;
   std::vector<std::unique_ptr<GUI::UserButtonHandler>> m_user_button_handlers;
   std::vector<std::unique_ptr<TraceFilterHandler>> m_trace_filter_handlers;
   uint32_t m_marker_index;
   std::string m_file_logging_path;
   std::string m_log_file_name;
   Persistence::PersistenceHandler m_persistence;
   bool m_scrolling_active;
   bool m_trace_scrolling_active;
   uint32_t m_scroll_default_color;
   std::map<uint8_t,std::string> m_port_id_name_map;
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
   void onButtonEvent(uint32_t button_id, ButtonEvent event) override;
   /* ThemeChangedListener */
   void onThemeChange(Theme theme) override;

   void onPortHandlerEvent(const GUI::PortHandlerEvent&);
   bool sendToPort(const std::string&);
   void addToTerminal(const std::string& port_name, const std::string& data, uint32_t background_color, uint32_t font_color);
   void setButtonState(uint32_t button_id, bool active);
   void onPersistenceRead(const std::vector<uint8_t>& data);
   void onPersistenceWrite(std::vector<uint8_t>& data);
   void addToCommandHistory(uint8_t port_id, const std::string& text);
   void removeCommandHistoryDuplicates(std::vector<std::string>& history, const std::string& text);
   uint8_t portNameToId(const std::string& name);
   std::string createLogFileName();

   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onScrollButtonClicked();
   void onClearButtonClicked();
   void onTraceScrollButtonClicked();
   void onTraceClearButtonClicked();
   void onSendButtonClicked();
   void onLoggingButtonContextMenuRequested();
   bool onCurrentPortSelectionChanged(const std::string& name);
   void onPortSwitchRequest();
   void onSettingsButtonClicked();

};
#endif
