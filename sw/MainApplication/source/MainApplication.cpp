   #include <QtWidgets/QMessageBox>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "MainApplication.h"
#include "SettingsHandler.h"
#include "Settings.h"
#include "LoggerEngine.h"
#include "Serialize.hpp"
#include "ApplicationSettingsDialog.h"
#if defined _WIN32
#include <Shlwapi.h>
#include <locale>
#include <codecvt>
#endif

constexpr uint32_t TRACE_MARKER_COLOR = 0xFF0000;
constexpr uint8_t PORT_HANDLERS_COUNT = 5;


namespace system_call
{
__attribute__((weak)) std::string getExecutablePath()
{
#if defined unix
   constexpr uint32_t MAX_PATH_LENGTH = 4096;
   char result[MAX_PATH_LENGTH];
   ssize_t count = readlink("/proc/self/exe", result, MAX_PATH_LENGTH);
   const char *path;
   if (count != -1) {
       path = dirname(result);
   }
   return std::string(path);
#elif defined _WIN32
//#pragma comment(lib, "shlwapi.lib")
    TCHAR path [MAX_PATH];
    (void) GetModuleFileName( NULL, path, MAX_PATH );
    PathRemoveFileSpec(path);
    using converter_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<converter_type, wchar_t> converter;
    return converter.to_bytes(path);
#endif
}
}

MainApplication::MainApplication(QWidget *parent):
m_timers(Utilities::ITimersFactory::create()),
m_file_logger(IFileLogger::create()),
m_gui_contoller(IGUIController::create(parent)),
m_marker_index(0),
m_scrolling_active(false),
m_trace_scrolling_active(false)
{
   Persistence::PersistenceListener::setName("MAIN_APPLICATION");
   m_persistence.addListener(*this);
   Settings::SettingsHandler::create();
   Settings::SettingsHandler::get()->start(system_call::getExecutablePath() + '/' + CONFIG_FILE);
   LoggerEngine::get()->startFrontends();

   if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Dark_Theme_ID))
   {
      m_gui_contoller->reloadTheme(IGUIController::Theme::DARK);
   }
   else if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Default_Theme_ID))
   {
      m_gui_contoller->reloadTheme(IGUIController::Theme::DEFAULT);
   }
   //TODO set window title
   UT_Log(MAIN, ALWAYS, "UltimateTerminal version %s", std::string(APPLICATION_VERSION).c_str());
   //TODO add application version string to infoLabel

   Settings::SettingsHandler::get()->printSettings();
   m_timers->start();

   m_gui_contoller->setTerminalScrollingEnabled(true);
   m_scrolling_active = true;
   m_gui_contoller->setTraceScrollingEnabled(true);
   m_trace_scrolling_active = true;

   m_logging_button_id = m_gui_contoller->getButtonID("loggingButton");
   setButtonState(m_logging_button_id, false);

   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(*m_gui_contoller, "portButton_1", "portLabel_1", *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(*m_gui_contoller, "portButton_2", "portLabel_2", *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(*m_gui_contoller, "portButton_3", "portLabel_3", *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(*m_gui_contoller, "portButton_4", "portLabel_4", *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(*m_gui_contoller, "portButton_5", "portLabel_5", *m_timers, this, this, m_persistence)));

   /* create handlers for user buttons */
   uint32_t user_buttons_count = SETTING_GET_U32(GUI_UserButtons_Tabs) * SETTING_GET_U32(GUI_UserButtons_RowsPerTab) * SETTING_GET_U32(GUI_UserButtons_ButtonsPerRow);
   for (uint32_t i = 0; i < user_buttons_count; i++)
   {
      std::string name = "BUTTON" + std::to_string(i);
      m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(*m_gui_contoller, name, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
   }

   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_1", "traceFilter_1", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_2", "traceFilter_2", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_3", "traceFilter_3", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_4", "traceFilter_4", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_5", "traceFilter_5", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_6", "traceFilter_6", this, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(*m_gui_contoller, "traceFilterButton_7", "traceFilter_7", this, m_persistence)));

   m_gui_contoller->addLineEnding("\\r\\n");
   m_gui_contoller->addLineEnding("\\n");
   m_gui_contoller->addLineEnding("EMPTY");

   m_file_logging_path = system_call::getExecutablePath();

   for (auto& handler : m_user_button_handlers)
   {
      handler->startThread();
   }
   if (!m_persistence.restore(PERSISTENCE_PATH))
   {
      UT_Log(MAIN, ERROR, "Cannot restore persistence!");
   }

   //TODO register for:
   // -> button clicked
   // -> button context menu
   // -> active port changed
   // -> theme changed
}
MainApplication::~MainApplication()
{
   UT_Log(MAIN, INFO, "Closing application!");
   if (!m_persistence.save(PERSISTENCE_PATH))
   {
      UT_Log(MAIN, ERROR, "Cannot write persistence!");
   }

   m_user_button_handlers.clear();
   m_trace_filter_handlers.clear();
   m_port_handlers.clear();

   m_timers->stop();
   m_persistence.removeListener(*this);
   Settings::SettingsHandler::get()->stop();
   Settings::SettingsHandler::destroy();
}
void MainApplication::onPortHandlerEvent(const GUI::PortHandlerEvent& event)
{
   if (event.event == GUI::Event::NEW_DATA)
   {
      addToTerminal(event.name, std::string(event.data.begin(), event.data.end()), event.background_color, event.font_color);
   }
   else if (event.event == GUI::Event::CONNECTED)
   {
      m_port_id_name_map[event.port_id] = event.name;
      m_gui_contoller->registerPortOpened(event.name);
      UT_Log(MAIN, INFO, "Port opened: %u [%s]", event.port_id, event.name.c_str());
   }
   else if (event.event == GUI::Event::DISCONNECTED)
   {
      m_gui_contoller->registerPortClosed(event.name);      m_port_id_name_map[event.port_id] = "";
      UT_Log(MAIN, INFO, "Port closed: %u [%s]", event.port_id, event.name.c_str());
   }
}
void MainApplication::addToTerminal(const std::string& port_name, const std::string& data, uint32_t background_color, uint32_t font_color)
{
   auto currentTime = std::chrono::system_clock::now();
   auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);
   QString new_line = QString().asprintf("[%s]<%u-%.2u-%u %u:%.2u:%.2u:%.3lu>: %s", port_name.c_str(),
                                   ts->tm_mday, ts->tm_mon, ts->tm_year + 1900,
                                   ts->tm_hour, ts->tm_min, ts->tm_sec, millis,
                                   data.c_str());

   m_file_logger->putLog(new_line.toStdString());

   // remove trailing newline
   if (new_line.back() == '\n')
   {
      new_line.chop(1);
   }

   if(m_gui_contoller->countTerminalItems() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in main terminal, cleaning trace view");
      m_gui_contoller->clearTerminalView();
   }

   if(m_gui_contoller->countTraceItems() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in trace view, cleaning trace view");
      m_gui_contoller->clearTraceView();
   }

   m_gui_contoller->addToTerminalView(new_line.toStdString(), background_color, font_color);

   for (auto& filter : m_trace_filter_handlers)
   {
      std::optional<Dialogs::TraceFilterSettingDialog::Settings> color_set = filter->tryMatch(data);
      if (color_set.has_value())
      {
         m_gui_contoller->addToTraceView(new_line.toStdString(), background_color, font_color);
      }
   }
}
void MainApplication::onMarkerButtonClicked()
{
   m_marker_index++;
   std::string log = "MARKER";
   log += std::to_string(m_marker_index);
   log += '\n';
   addToTerminal("MARKER", log, TRACE_MARKER_COLOR, m_gui_contoller->getTextColor());
}
void MainApplication::onLoggingButtonClicked()
{
   if (!m_file_logger->isActive())
   {
      UT_Log(MAIN, LOW, "enabling file logging");
      m_log_file_name = m_file_logging_path + '/' + createLogFileName();
      if (m_file_logger->openFile(m_log_file_name))
      {
         UT_Log(MAIN, INFO, "Logging started, new logfile created: %s", m_log_file_name.c_str());
         //TODO display message on status bar
         //ui->statusbar->showMessage(QString("").asprintf("New log file: %s", m_log_file_name.c_str()), SETTING_GET_U32(MainApplication_statusBarTimeout));
         setButtonState(m_logging_button_id, true);
      }
      else
      {
         QString error_message = QString().asprintf("Cannot create logfile @ %s\n%s (%u)", m_file_logging_path.c_str(), strerror(errno), errno);
         QMessageBox messageBox;
         messageBox.setText(error_message);
         messageBox.setWindowTitle("Error");
         messageBox.setIcon(QMessageBox::Critical);
         messageBox.setPalette(m_gui_contoller->getApplicationPalette());
         messageBox.exec();
      }
   }
   else
   {
      UT_Log(MAIN, LOW, "stopping file logging");
      //TODO display message on status bar
      //ui->statusbar->showMessage(QString("").asprintf("Log file ready: %s", m_log_file_name.c_str()), SETTING_GET_U32(MainApplication_statusBarTimeout));
      m_file_logger->closeFile();
      setButtonState(m_logging_button_id, false);
   }
}
void MainApplication::onScrollButtonClicked()
{
   m_gui_contoller->setTerminalScrollingEnabled(!m_scrolling_active);
   m_scrolling_active = !m_scrolling_active;
}
void MainApplication::onTraceScrollButtonClicked()
{
   m_gui_contoller->setTraceScrollingEnabled(!m_trace_scrolling_active);
   m_trace_scrolling_active = !m_trace_scrolling_active;
}
void MainApplication::onTraceClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing trace window requested");
   m_gui_contoller->clearTraceView();
}
void MainApplication::onLoggingButtonContextMenuRequested()
{
   Dialogs::LoggingSettingDialog dialog;
   UT_Log(MAIN, MEDIUM, "Opening log setting dialog with path %s", m_file_logging_path.c_str());
   auto result = dialog.showDialog(this, m_file_logging_path, !m_file_logger->isActive());
   if (result)
   {
      UT_Log(MAIN, LOW, "got new logging path %s", result.value().c_str());
      m_file_logging_path = result.value();
   }
}
void MainApplication::onClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing terminal window requested");
   m_gui_contoller->clearTerminalView();
}
void MainApplication::onCurrentPortSelectionChanged(const std::string& port_name)
{
   uint8_t port_id = portNameToId(port_name);
   UT_Log(MAIN, LOW, "Selected port changed, new port %s with id %u", port_name.c_str(), port_id);

   std::vector<std::string>& commands_history = m_commands_history[port_id];
   m_gui_contoller->setCommandsHistory(commands_history);
   m_current_port_name = port_name;
}
void MainApplication::onSettingsButtonClicked()
{
   Dialogs::ApplicationSettingsDialog dialog (m_port_handlers, m_trace_filter_handlers, m_file_logger, m_file_logging_path, *this);
   dialog.showDialog(this);
}
bool MainApplication::sendToPort(const std::string& string)
{
   bool result = false;
   uint8_t port_id = portNameToId(m_current_port_name);
   std::string data_to_send = string;
   UT_Log(MAIN, HIGH, "Trying to send data to port %u[%s] - %s", port_id, m_current_port_name.c_str(), string.c_str());
   std::string current_ending = m_gui_contoller->getCurrentLineEnding();
   if (current_ending == "\\r\\n")
   {
      data_to_send += "\r\n";
   }
   else if (current_ending == "\\n")
   {
      data_to_send += '\n';
   }

   for (const auto& handler : m_port_handlers)
   {
      if (handler->getName() == m_current_port_name)
      {
         if (handler->write({data_to_send.begin(), data_to_send.end()}, data_to_send.size()))
         {
            result = true;
            addToTerminal(m_current_port_name, data_to_send, m_gui_contoller->getBackgroundColor(), m_gui_contoller->getTextColor());
            addToCommandHistory(port_id, string);
         }
         else
         {
            std::string error = "Cannot send data to port ";
            error += m_current_port_name;
            UT_Log(MAIN, ERROR, "%s", error.c_str());
            error += '\n';
            addToTerminal(m_current_port_name, error, m_gui_contoller->getBackgroundColor(), m_gui_contoller->getTextColor());
         }
      }
   }
   return result;
}
void MainApplication::onSendButtonClicked()
{
   UT_Log(MAIN, LOW, "Send button clicked");
   (void)sendToPort(m_gui_contoller->getCurrentCommand());
}
void MainApplication::setButtonState(uint32_t button_id, bool state)
{
   constexpr uint32_t GREEN = 0x00FF00;
   constexpr uint32_t BLACK = 0x000000;
   const uint32_t DEFAULT_BACKGROUND = m_gui_contoller->getBackgroundColor();
   const uint32_t DEFAULT_FONT = m_gui_contoller->getTextColor();
   if (state)
   {
      m_gui_contoller->setButtonBackgroundColor(button_id, GREEN);
      m_gui_contoller->setButtonFontColor(button_id, BLACK);

   }
   else
   {
      m_gui_contoller->setButtonBackgroundColor(button_id, DEFAULT_BACKGROUND);
      m_gui_contoller->setButtonFontColor(button_id, DEFAULT_FONT);
   }
}
void MainApplication::onPersistenceRead(const std::vector<uint8_t>& data)
{
   std::string line_ending;
   std::string application_version;
   uint32_t offset = 0;
   uint8_t ports_count = 0;

   ::deserialize(data, offset, application_version);
   ::deserialize(data, offset, m_file_logging_path);
   ::deserialize(data, offset, m_scrolling_active);
   ::deserialize(data, offset, m_trace_scrolling_active);
   ::deserialize(data, offset, line_ending);

   ::deserialize(data, offset, ports_count);
   for (uint8_t i = 0; i < ports_count; i++)
   {
      uint8_t port_id;
      uint32_t commands_size;
      std::vector<std::string> port_commands;
      ::deserialize(data, offset, port_id);
      ::deserialize(data, offset, commands_size);
      for (uint32_t i = 0; i < commands_size; i++)
      {
         std::string command;
         ::deserialize(data, offset, command);
         port_commands.push_back(command);
      }

      if (port_id <= PORT_HANDLERS_COUNT)
      {
         m_commands_history[port_id] = port_commands;
      }
      UT_Log(MAIN, HIGH, "Restored %u commands for port %u", commands_size, port_id);
   }
   m_gui_contoller->setTerminalScrollingEnabled(m_scrolling_active);
   m_gui_contoller->setTraceScrollingEnabled(m_trace_scrolling_active);
   m_gui_contoller->setCurrentLineEnding(line_ending);
   UT_Log_If(application_version != std::string(APPLICATION_VERSION), MAIN, INFO, "Application update detected %s -> %s", application_version.c_str(), std::string(APPLICATION_VERSION).c_str());
}
void MainApplication::onPersistenceWrite(std::vector<uint8_t>& data)
{
   ::serialize(data, std::string(APPLICATION_VERSION));
   ::serialize(data, m_file_logging_path);
   ::serialize(data, m_scrolling_active);
   ::serialize(data, m_trace_scrolling_active);
   ::serialize(data, m_gui_contoller->getCurrentLineEnding());
   ::serialize(data, (uint8_t)m_commands_history.size());
   for (const auto& item : m_commands_history)
   {
      ::serialize(data, item.first);
      ::serialize(data, (uint32_t)item.second.size());
      for (const std::string& history_item : item.second)
      {
         ::serialize(data, history_item);
      }
      UT_Log(MAIN, HIGH, "Serialized %u commands for port %u", item.second.size(), item.first);
   }
}

void MainApplication::addToCommandHistory(uint8_t port_id, const std::string& text)
{
   if (!text.empty())
   {
      /* put in history */
      std::vector<std::string>& history = m_commands_history[port_id];
      history.push_back(text);
      while(history.size() >= MAX_COMMANDS_HISTORY_ITEMS)
      {
         history.erase(history.begin());
      }
      m_gui_contoller->setCommandsHistory(history);
   }
}
uint8_t MainApplication::portNameToId(const std::string& name)
{
   auto result = std::find_if(m_port_id_name_map.begin(), m_port_id_name_map.end(), [&](const auto& obj){return obj.second == name;});
   if (result != m_port_id_name_map.end())
   {
      return result->first;
   }
   else
   {
      return 0;
   }
}
std::string MainApplication::createLogFileName()
{
   std::string result;
   /* create new file */
   auto currentTime = std::chrono::system_clock::now();
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);
   result = QString().asprintf("log_%u%.2u%u_%.2u%.2u%.2u",
                                   ts->tm_mday, ts->tm_mon, ts->tm_year + 1900,
                                   ts->tm_hour, ts->tm_min, ts->tm_sec).toStdString();
   return result;
}
