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
#include "MessageDialog.h"
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
   char result[MAX_PATH_LENGTH + 1] = {};
   ssize_t count = readlink("/proc/self/exe", result, MAX_PATH_LENGTH);
   const char *path = nullptr;
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

namespace MainApplication
{

MainApplication::MainApplication():
m_timers(Utilities::ITimersFactory::create()),
m_file_logger(IFileLogger::create()),
m_gui_controller(nullptr),
m_marker_index(0)
{
   Utilities::Persistence::PersistenceListener::setName("MAIN_APPLICATION");
   if (!m_persistence.loadFile(getPersistenceFile()))
   {
      UT_Log(MAIN, ERROR, "Cannot open persistence file!");
   }

   m_persistence.addListener(*this);
   Settings::SettingsHandler::create(m_persistence);
   Settings::SettingsHandler::get()->start();
   Logger::LoggerEngine::get()->startFrontends(system_call::getExecutablePath() + '/' + LOGS_FILE);
   UT_Log(MAIN, ALWAYS, "UltimateTerminal version %s", std::string(APPLICATION_VERSION).c_str());

   m_gui_controller.run();
   m_gui_controller.reloadTheme(Theme::DARK);
   m_gui_controller.setApplicationTitle(std::string("UltimateTerminal"));
   m_gui_controller.setInfoLabelText(std::string("UltimateTerminal v") + std::string(APPLICATION_VERSION));

   m_timers->start();

   m_marker_button_id  = m_gui_controller.getButtonID("markerButton");
   m_logging_button_id = m_gui_controller.getButtonID("loggingButton");
   m_settings_button_id = m_gui_controller.getButtonID("settingsButton");
   m_send_button_id = m_gui_controller.getButtonID("sendButton");
   m_scroll_button_id = m_gui_controller.getButtonID("bottomButton");
   m_clear_button_id = m_gui_controller.getButtonID("clearButton");
   m_trace_clear_button = m_gui_controller.getButtonID("traceClearButton");
   m_trace_scroll_button = m_gui_controller.getButtonID("traceBottomButton");

   m_gui_controller.subscribeForButtonEvent(m_marker_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_logging_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_logging_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.subscribeForButtonEvent(m_settings_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_send_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_scroll_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_clear_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_trace_clear_button, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_trace_scroll_button, GUIController::ButtonEvent::CLICKED, this);

   m_button_listeners.push_back({m_marker_button_id, GUIController::ButtonEvent::CLICKED, [&](){onMarkerButtonClicked();}});
   m_button_listeners.push_back({m_logging_button_id, GUIController::ButtonEvent::CLICKED, [&](){onLoggingButtonClicked();}});
   m_button_listeners.push_back({m_logging_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, [&](){onLoggingButtonContextMenuRequested();}});
   m_button_listeners.push_back({m_settings_button_id, GUIController::ButtonEvent::CLICKED, [&](){onSettingsButtonClicked();}});
   m_button_listeners.push_back({m_send_button_id, GUIController::ButtonEvent::CLICKED, [&](){onSendButtonClicked();}});
   m_button_listeners.push_back({m_scroll_button_id, GUIController::ButtonEvent::CLICKED, [&](){onBottomButtonClicked();}});
   m_button_listeners.push_back({m_clear_button_id, GUIController::ButtonEvent::CLICKED, [&](){onClearButtonClicked();}});
   m_button_listeners.push_back({m_trace_clear_button, GUIController::ButtonEvent::CLICKED, [&](){onTraceClearButtonClicked();}});
   m_button_listeners.push_back({m_trace_scroll_button, GUIController::ButtonEvent::CLICKED, [&](){onTraceBottomButtonClicked();}});

   setButtonState(m_logging_button_id, false);

   m_gui_controller.subscribeForActivePortChangedEvent([&](const std::string& name)->bool{return onCurrentPortSelectionChanged(name);});
   m_gui_controller.subscribeForThemeReloadEvent(this);

   uint32_t port_numbers = m_gui_controller.countPorts();
   for (uint32_t i = 0; i < port_numbers; i++)
   {
      std::string button_name = "portButton_" + std::to_string(i);
      m_ports.emplace_back(std::unique_ptr<Port>(
          new Port(i, m_gui_controller, button_name, *m_timers, this, m_persistence)));
   }
   UT_Log(MAIN, LOW, "%u Ports created", port_numbers);

   /* create handlers for user buttons */
   uint32_t count_tabs = m_gui_controller.countTabs();
   for (uint32_t i = 0; i < count_tabs; i++)
   {
      m_user_buttons_tabs.emplace_back(std::unique_ptr<UserButtonsTab>(
          new UserButtonsTab(m_gui_controller, i, m_gui_controller.countButtonsPerTab(), m_persistence,
                                         std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
   }
   UT_Log(MAIN, LOW, "%u UserButtonsTabs created", count_tabs);

   uint32_t filters_count = m_gui_controller.countTraceFilters();
   for (uint32_t i = 0; i < filters_count; i++)
   {
      std::string button_name = "traceFilterButton_" + std::to_string(i);
      m_trace_filters.emplace_back(std::unique_ptr<TraceFilter>(new TraceFilter(i, m_gui_controller, button_name, m_persistence)));
   }
   UT_Log(MAIN, LOW, "%u TraceFilters created", filters_count);

   m_gui_controller.addLineEnding("\\r\\n");
   m_gui_controller.addLineEnding("\\n");
   m_gui_controller.addLineEnding("EMPTY");

   m_file_logging_path = system_call::getExecutablePath();
   m_persistence.restore();
}
MainApplication::~MainApplication()
{
   UT_Log(MAIN, ALWAYS, "Closing application!");

   m_gui_controller.unsubscribeFromButtonEvent(m_marker_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_logging_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_settings_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_send_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_scroll_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_clear_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_trace_clear_button, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_trace_scroll_button, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_logging_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.unsubscribeFromThemeReloadEvent(this);

   m_button_listeners.clear();

   if (!m_persistence.save(getPersistenceFile()))
   {
      UT_Log(MAIN, ERROR, "Cannot write persistence!");
   }

   m_user_buttons_tabs.clear();
   m_trace_filters.clear();
   m_ports.clear();

   m_timers->stop();
   m_persistence.removeListener(*this);
   Settings::SettingsHandler::get()->stop();
   Settings::SettingsHandler::destroy();
   Logger::LoggerEngine::get()->stopFrontends();
}
void MainApplication::onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event)
{
   UT_Log(MAIN, LOW, "%s: btn %u ev %u", __func__, button_id, (uint32_t)event);

   for (auto& listener : m_button_listeners)
   {
      if (listener.button_id == button_id && listener.event == event)
      {
         listener.listener();
      }
   }
}
void MainApplication::onThemeChange(Theme theme)
{
   UT_Log(MAIN, LOW, "%s: theme %s", __func__, m_gui_controller.themeToName(theme).c_str());
   if (!m_file_logger->isActive())
   {
      setButtonState(m_logging_button_id, false);
   }
   for (auto& handler : m_ports)
   {
      handler->refreshUi();
   }
   for (auto& filter : m_trace_filters)
   {
      filter->refreshUi();
   }
}
void MainApplication::onPortEvent(const PortEvent& event)
{
   std::lock_guard<std::mutex> lock(m_ports_mutex);
   if (event.event == Event::NEW_DATA)
   {
      addToTerminal(event.name, std::string(event.data.begin(), event.data.end()), event.background_color, event.font_color);
   }
   else if (event.event == Event::CONNECTED)
   {
      m_port_id_name_map[event.port_id] = event.name;
      m_gui_controller.registerPortOpened(event.name);
      UT_Log(MAIN, INFO, "Port opened: %u [%s]", event.port_id, event.name.c_str());
   }
   else if (event.event == Event::DISCONNECTED)
   {
      m_gui_controller.registerPortClosed(event.name);
      m_port_id_name_map[event.port_id] = "";
      UT_Log(MAIN, INFO, "Port closed: %u [%s]", event.port_id, event.name.c_str());
   }
   else if (event.event == Event::CONNECTING)
   {
      m_gui_controller.registerPortClosed(event.name);
      m_port_id_name_map[event.port_id] = "";
      UT_Log(MAIN, INFO, "Port connecting: %u [%s]", event.port_id, event.name.c_str());
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

   if(m_gui_controller.countTerminalItems() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in main terminal, cleaning trace view");
      m_gui_controller.clearTerminalView();
   }

   if(m_gui_controller.countTraceItems() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in trace view, cleaning trace view");
      m_gui_controller.clearTraceView();
   }

   m_gui_controller.addToTerminalView(new_line.trimmed().toStdString(), background_color, font_color);
   m_file_logger->putLog(new_line.toStdString());

   for (auto& filter : m_trace_filters)
   {
      std::optional<Dialogs::TraceFilterSettingDialog::Settings> color_set = filter->tryMatch(data);
      if (color_set.has_value())
      {
         m_gui_controller.addToTraceView(new_line.trimmed().toStdString(), color_set->background, color_set->font);
      }
   }
}
void MainApplication::onMarkerButtonClicked()
{
   m_marker_index++;
   std::string log = "MARKER";
   log += std::to_string(m_marker_index);
   log += '\n';
   addToTerminal("MARKER", log, TRACE_MARKER_COLOR, m_gui_controller.getTextColor());
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
         m_gui_controller.setStatusBarNotification("New log file: " + m_log_file_name, SETTING_GET_U32(MainApplication_statusBarTimeout));
         setButtonState(m_logging_button_id, true);
      }
      else
      {
         std::string message = "Cannot create logfile @ " + m_file_logging_path + "\n" +
                               strerror(errno) + " (" + std::to_string(errno) + ")";
         Dialogs::MessageDialog::show(Dialogs::MessageDialog::Icon::Critical, "Error", message, m_gui_controller.getApplicationPalette());
      }
   }
   else
   {
      UT_Log(MAIN, LOW, "stopping file logging");
      m_gui_controller.setStatusBarNotification("Log file ready: " + m_log_file_name, SETTING_GET_U32(MainApplication_statusBarTimeout));
      m_file_logger->closeFile();
      setButtonState(m_logging_button_id, false);
   }
}
void MainApplication::onBottomButtonClicked()
{
   m_gui_controller.scrollTerminalToBottom();
}
void MainApplication::onTraceBottomButtonClicked()
{
   m_gui_controller.scrollTraceViewToBottom();
}
void MainApplication::onTraceClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing trace window requested");
   m_gui_controller.clearTraceView();
}
void MainApplication::onLoggingButtonContextMenuRequested()
{
   Dialogs::LoggingSettingDialog dialog;
   UT_Log(MAIN, LOW, "Opening log setting dialog with path %s", m_file_logging_path.c_str());
   auto result = dialog.showDialog(m_gui_controller.getParent(), m_file_logging_path, !m_file_logger->isActive());
   if (result)
   {
      UT_Log(MAIN, LOW, "got new logging path %s", result.value().c_str());
      m_file_logging_path = result.value();
   }
}
void MainApplication::onClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing terminal window requested");
   m_gui_controller.clearTerminalView();
}
bool MainApplication::onCurrentPortSelectionChanged(const std::string& port_name)
{
   uint8_t port_id = portNameToId(port_name);
   UT_Log(MAIN, LOW, "Selected port changed, new port %s with id %u", port_name.c_str(), port_id);

   std::vector<std::string>& commands_history = m_commands_history[port_id];
   m_gui_controller.setCommandsHistory(commands_history);
   m_current_port_name = port_name;

   return true;
}
void MainApplication::onSettingsButtonClicked()
{
   UT_Log(MAIN, LOW, "Setting windows show");
   Dialogs::ApplicationSettingsDialog dialog (m_gui_controller, m_ports, m_trace_filters, m_file_logger, m_file_logging_path, getPersistenceFile());
   dialog.showDialog(m_gui_controller.getParent());
}

bool MainApplication::sendToPort(const std::string& string)
{
   bool result = false;
   uint8_t port_id = portNameToId(m_current_port_name);
   std::string data_to_send = string;
   UT_Log(MAIN, HIGH, "Trying to send data to port %u[%s] - %s", port_id, m_current_port_name.c_str(), string.c_str());
   std::string current_ending = m_gui_controller.getCurrentLineEnding();
   if (current_ending == "\\r\\n")
   {
      data_to_send += "\r\n";
   }
   else if (current_ending == "\\n")
   {
      data_to_send += '\n';
   }

   for (const auto& handler : m_ports)
   {
      if (handler->getName() == m_current_port_name)
      {
         if (handler->write({data_to_send.begin(), data_to_send.end()}, data_to_send.size()))
         {
            result = true;
            addToCommandHistory(port_id, string);
         }
         else
         {
            std::string error = "Cannot send data to port ";
            error += m_current_port_name;
            UT_Log(MAIN, ERROR, "%s", error.c_str());
            error += '\n';
            Dialogs::MessageDialog::show(Dialogs::MessageDialog::Icon::Critical, "Error", error, m_gui_controller.getApplicationPalette());
         }
      }
   }
   return result;
}

void MainApplication::onSendButtonClicked()
{
   UT_Log(MAIN, LOW, "Send button clicked");
   (void)sendToPort(m_gui_controller.getCurrentCommand());
   m_gui_controller.clearCommand();
}

void MainApplication::setButtonState(uint32_t button_id, bool state)
{
   constexpr uint32_t GREEN = 0x00FF00;
   constexpr uint32_t BLACK = 0x000000;
   const uint32_t DEFAULT_BACKGROUND = m_gui_controller.getBackgroundColor();
   const uint32_t DEFAULT_FONT = m_gui_controller.getTextColor();
   if (state)
   {
      m_gui_controller.setButtonBackgroundColor(button_id, GREEN);
      m_gui_controller.setButtonFontColor(button_id, BLACK);

   }
   else
   {
      m_gui_controller.setButtonBackgroundColor(button_id, DEFAULT_BACKGROUND);
      m_gui_controller.setButtonFontColor(button_id, DEFAULT_FONT);
   }
   UT_Log(MAIN, LOW, "%s id %u state %u", __func__, button_id, state);
}
void MainApplication::onPersistenceRead(const PersistenceItems& items)
{
   std::string line_ending;
   std::string application_version;
   std::string logging_path;
   uint8_t theme = 0;
   std::string portsWithHistory;

   Utilities::Persistence::readItem(items, "applicationVersion", application_version);
   UT_Log_If(application_version != std::string(APPLICATION_VERSION), MAIN, INFO, "Application update detected %s -> %s", application_version.c_str(), std::string(APPLICATION_VERSION).c_str());

   Utilities::Persistence::readItem(items, "portsWithHistory", portsWithHistory);
   std::stringstream ss (portsWithHistory);
   std::string portID;
   while (std::getline(ss, portID, '.'))
   {
      std::string commandsHistoryInLine;
      UT_Log(MAIN, HIGH, "Looking for command history for portID %s", portID.c_str());
      Utilities::Persistence::readItem(items, "history." + portID, commandsHistoryInLine);
      std::stringstream commandHistoryStream(commandsHistoryInLine);
      std::string command;
      while(std::getline(commandHistoryStream, command, ':'))
      {
         UT_Log(MAIN, HIGH, "Got command for portID %s [%s]", portID.c_str(), command.c_str());
         m_commands_history[std::stoi(portID)].push_back(command);
      }
   }

   if (Utilities::Persistence::readItem(items, "lineEnding", line_ending))
   {
      UT_Log(MAIN, HIGH, "Restored line ending");
      m_gui_controller.setCurrentLineEnding(line_ending);
   }
   if(Utilities::Persistence::readItem(items, "currentTheme", theme))
   {
      UT_Log(MAIN, HIGH, "Restored theme with ID %u", theme);
      m_gui_controller.reloadTheme(static_cast<Theme>(theme));
   }
   if(Utilities::Persistence::readItem(items, "loggingFile", logging_path))
   {
      UT_Log(MAIN, HIGH, "Restored ligging file [%s]", logging_path.c_str());
      m_file_logging_path = logging_path;
   }

}
void MainApplication::onPersistenceWrite(PersistenceItems& buffer)
{
   Utilities::Persistence::writeItem(buffer, "applicationVersion", std::string(APPLICATION_VERSION));
   Utilities::Persistence::writeItem(buffer, "loggingFile", m_file_logging_path);
   Utilities::Persistence::writeItem(buffer, "lineEnding", m_gui_controller.getCurrentLineEnding());
   Utilities::Persistence::writeItem(buffer, "currentTheme", static_cast<uint8_t>(m_gui_controller.currentTheme()));
   std::string portsWithHistory = "";
   for (const auto& item : m_commands_history)
   {
      UT_Log(MAIN, HIGH, "Saving commands history for port %u, size %u", item.first, item.second.size());
      std::string commandHistoryInString;
      for (const std::string& history_item : item.second)
      {
         commandHistoryInString += (history_item + ':');
      }
      Utilities::Persistence::writeItem(buffer, "history." + std::to_string(item.first), commandHistoryInString);
      portsWithHistory += std::to_string(item.first) + '.';
      UT_Log(MAIN, HIGH, "Serialized %u commands for port %u", item.second.size(), item.first);
   }
   Utilities::Persistence::writeItem(buffer, "portsWithHistory", portsWithHistory);
}

void MainApplication::addToCommandHistory(uint8_t port_id, const std::string& text)
{
   if (!text.empty())
   {
      /* put in history */
      std::vector<std::string>& history = m_commands_history[port_id];
      removeCommandHistoryDuplicates(history, text);
      history.push_back(text);
      while(history.size() >= MAX_COMMANDS_HISTORY_ITEMS)
      {
         history.erase(history.begin());
      }
      m_gui_controller.setCommandsHistory(history);
   }
}

void MainApplication::removeCommandHistoryDuplicates(std::vector<std::string>& history, const std::string& text)
{
   history.erase(std::remove(history.begin(), history.end(), text), history.end());
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

std::string MainApplication::getPersistenceFile()
{
   return system_call::getExecutablePath() + "/" + std::string(PERSISTENCE_FILE);
}

}
