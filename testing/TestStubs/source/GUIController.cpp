#include "GUIController.h"
#include <QtCore/QVector>
#include "TestFrameworkAPI.h"
#include "TestFrameworkCommon.h"
#include "RPCServer.h"
#include "RPCCommon.h"


#undef APPLICATION_THEME
#define APPLICATION_THEME(theme_name) #theme_name,
std::array<std::string, (uint32_t)Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME

/* global objects declaration */
std::unique_ptr<RPC::RPCServer> rpc_server;
std::vector<Dialogs::PortSettingDialog::Settings> g_port_settings;
std::vector<Dialogs::TraceFilterSettingDialog::Settings> g_trace_filter_settings;
std::vector<Dialogs::UserButtonDialog::Settings> g_user_button_settings;
GUIController::MessageBoxDetails g_messagebox_details;
std::string g_logging_path;

/* this is to remove the alpha factor from color hash */
static constexpr uint32_t COLOR_MASK = 0xFFFFFF;

GUIController::GUIController(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_current_theme(Theme::DEFAULT),
m_terminal_view_status{},
m_trace_view_status{}
{
   rpc_server = std::unique_ptr<RPC::RPCServer>(new RPC::RPCServer());
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetButtonState, std::bind(&GUIController::onGetButtonStateRequest, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ButtonClick, std::bind(&GUIController::onButtonClicked, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ButtonContextMenuClick, std::bind(&GUIController::onButtonContextMenuRequested, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetCommand, std::bind(&GUIController::onSetCommandRequest, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetCommand, std::bind(&GUIController::onGetCommandRequest, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetPortLabel, std::bind(&GUIController::onGetPortLabel, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetLineEnding, std::bind(&GUIController::onGetLineEnding, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetLineEnding, std::bind(&GUIController::onSetLineEnding, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetAllLineEndings, std::bind(&GUIController::onGetAllLineEndings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTargetPort, std::bind(&GUIController::onGetTargetPort, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTargetPort, std::bind(&GUIController::onSetTargetPort, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetAllTargetPorts, std::bind(&GUIController::onGetAllTargetPorts, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceFilterState, std::bind(&GUIController::onGetTraceFilterState, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTraceFilter, std::bind(&GUIController::onSetTraceFilter, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetCommandHistory, std::bind(&GUIController::onGetCommandHistory, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTerminalViewContent, std::bind(&GUIController::onGetTerminalViewContent, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceViewContent, std::bind(&GUIController::onGetTraceViewContent, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetPortSettings, std::bind(&GUIController::onSetPortSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetPortSettings, std::bind(&GUIController::onGetPortSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTraceFilterSettings, std::bind(&GUIController::onSetTraceFilterSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceFilterSettings, std::bind(&GUIController::onGetTraceFilterSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetUserButtonSettings, std::bind(&GUIController::onSetUserButtonSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetUserButtonSettings, std::bind(&GUIController::onGetUserButtonSettings, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetMessageBox, std::bind(&GUIController::onGetMessageBox, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ResetMessageBox, std::bind(&GUIController::onResetMessageBox, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetLoggingPath, std::bind(&GUIController::onSetLoggingPath, this, std::placeholders::_1));
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetLoggingPath, std::bind(&GUIController::onGetLoggingPath, this, std::placeholders::_1));
}
GUIController::~GUIController()
{
   rpc_server->stop();
}
void GUIController::run()
{
   UT_Log(MAIN_GUI, INFO, "Starting simulated GUI Controller");
   ui->setupUi(this);
   rpc_server->start(SERVER_PORT);

   auto& buttons = ui->getButtons();
   for (uint32_t i = 0; i < buttons.size(); i++)
   {
      m_buttons_cache.push_back({i, buttons[i]->objectName().toStdString(), {}, buttons[i]->isEnabled(), buttons[i]->isChecked(), buttons[i]->isCheckable(), buttons[i]->text().toStdString()});
   }
   auto& filters = ui->getTraceFilters();
   for (uint32_t i = 0; i < filters.size(); i++)
   {
      m_trace_filters.push_back({i, filters[i].button->objectName().toStdString(), true, {}, {}});
      g_trace_filter_settings.push_back({});
   }
   auto& ports = ui->getPorts();
   for (uint32_t i = 0; i < filters.size(); i++)
   {
      m_port_labels.push_back({i, {},{}});
      g_port_settings.push_back({});
   }
   g_user_button_settings.resize(ui->countUserButtons());
}
uint32_t GUIController::getButtonID(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   auto elements = ui->getButtons();
   for (uint32_t i = 0; i < elements.size(); i++)
   {
      UT_Assert(elements[i]);
      if (elements[i]->objectName().toStdString() == name)
      {
         result = i;
         break;
      }
   }
   UT_Log_If(result == UINT32_MAX, MAIN_GUI, ERROR, "Element with name %s not found", name.c_str());
   return result;
}
void GUIController::subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) && (item.event == event);});
   if (it != m_button_listeners.end())
   {
      UT_Log(MAIN_GUI, ERROR, "event %u for buttonID %u already registered, replacing!");
      *it = ButtonEventItem{button_id, event, listener};
   }
   else
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u registered!", (uint32_t)event, button_id);
      m_button_listeners.push_back(ButtonEventItem{button_id, event, listener});
   }
}
void GUIController::unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) &&
                                                                                                                        (item.event == event) &&
                                                                                                                        (item.listener == listener);});
   UT_Log_If(it == m_button_listeners.end(), MAIN_GUI, ERROR, "event %u for buttonID %u not found!");
   if (it != m_button_listeners.end())
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u unregistered", (uint32_t)event, button_id);
      m_button_listeners.erase(it);
   }
}
void GUIController::setButtonBackgroundColor(uint32_t button_id, uint32_t color)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].colors.background_color = color & COLOR_MASK;
}
void GUIController::setButtonFontColor(uint32_t button_id, uint32_t color)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].colors.font_color = color & COLOR_MASK;
}
void GUIController::setButtonCheckable(uint32_t button_id, bool checkable)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].checkable = checkable;
}
void GUIController::setButtonChecked(uint32_t button_id, bool checked)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].checked = checked;
}
void GUIController::setButtonEnabled(uint32_t button_id, bool enabled)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].enabled = enabled;
}
void GUIController::setButtonText(uint32_t button_id, const std::string& text)
{
   UT_Assert(button_id < m_buttons_cache.size());
   std::lock_guard<std::mutex> lock(m_mutex);
   m_buttons_cache[button_id].text = text;
}
void GUIController::clearTerminalView()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_terminal_items.clear();
}
void GUIController::clearTraceView()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_trace_items.clear();
}
void GUIController::addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   m_terminal_items.push_back({text, {background_color & COLOR_MASK, font_color & COLOR_MASK}});
}
void GUIController::addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   m_trace_items.push_back({text, {background_color & COLOR_MASK, font_color & COLOR_MASK}});
}
uint32_t GUIController::countTerminalItems()
{
   return (uint32_t)m_terminal_items.size();
}
uint32_t GUIController::countTraceItems()
{
   return (uint32_t)m_trace_items.size();
}
void GUIController::setTerminalScrollingEnabled(bool enabled)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_terminal_scrolling_enabled = enabled;
}
void GUIController::setTraceScrollingEnabled(bool enabled)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_trace_scrolling_enabled = enabled;
}
void GUIController::subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback)
{
   m_active_port_listener = callback;
}
void GUIController::registerPortOpened(const std::string& port_name)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_active_ports.push_back(port_name);
   m_current_active_port = m_active_ports.begin();
}
void GUIController::registerPortClosed(const std::string& port_name)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   auto it = std::find(m_active_ports.begin(), m_active_ports.end(), port_name);
   if (it != m_active_ports.end())
   {
      m_active_ports.erase(it);
   }
   m_current_active_port = m_active_ports.begin();
}
void GUIController::setCommandsHistory(const std::vector<std::string>& history)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_commands_history = history;
}
void GUIController::addCommandToHistory(const std::string& history)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_commands_history.push_back(history);
}
std::string GUIController::getCurrentCommand()
{
   return m_current_command;
}
std::string GUIController::getCurrentLineEnding()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert((m_line_endings.size()) && (m_current_line_ending != m_line_endings.end()));
   return *m_current_line_ending;
}
void GUIController::addLineEnding(const std::string& ending)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_line_endings.push_back(ending);
   m_current_line_ending = m_line_endings.begin();
}
void GUIController::setCurrentLineEnding(const std::string& ending)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   m_current_line_ending = std::find(m_line_endings.begin(), m_line_endings.end(), ending);
}
void GUIController::setTraceFilter(uint8_t id, const std::string& filter)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_trace_filters[id].text = filter;
}
std::string GUIController::getTraceFilter(uint8_t id)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   return m_trace_filters[id].text;
}
void GUIController::setTraceFilterEnabled(uint8_t id, bool enabled)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_trace_filters[id].enabled = enabled;
}
void GUIController::setTraceFilterBackgroundColor(uint32_t id, uint32_t color)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_trace_filters[id].colors.background_color = color & COLOR_MASK;
}
void GUIController::setTraceFilterFontColor(uint32_t id, uint32_t color)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_trace_filters[id].colors.font_color = color & COLOR_MASK;
}
void GUIController::setPortLabelText(uint8_t id, const std::string& description)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_port_labels[id].text = description;
}
void GUIController::setPortLabelStylesheet(uint8_t id, const std::string& stylesheet)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(id < m_trace_filters.size());
   m_port_labels[id].stylesheet = stylesheet;
}
void GUIController::reloadTheme(Theme theme)
{
   UT_Assert(theme < Theme::APPLICATION_THEMES_MAX);
   UT_Log(MAIN_GUI, HIGH, "%s id %u name %s", __func__, (uint8_t)theme, themeToName(theme).c_str());
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   ui->loadTheme(theme);
   for (auto& listener : m_theme_listeners)
   {
      if (listener) listener->onThemeChange(theme);
   }
}
Theme GUIController::currentTheme()
{
   return ui->theme;
}
std::string GUIController::themeToName(Theme theme)
{
   UT_Assert(theme < Theme::APPLICATION_THEMES_MAX);
   return m_themes_names[(uint32_t)theme];
}
Theme GUIController::nameToTheme(const std::string& name)
{
   Theme result = Theme::APPLICATION_THEMES_MAX;
   auto it = std::find(m_themes_names.begin(), m_themes_names.end(), name);
   if (it != m_themes_names.end())
   {
      result = (Theme)(std::distance(m_themes_names.begin(), it));
   }
   return result;
}
uint32_t GUIController::getBackgroundColor()
{
   return ui->mainWindow->palette().color(QPalette::Window).rgb();
}
uint32_t GUIController::getTerminalBackgroundColor()
{
   return ui->mainWindow->palette().color(QPalette::Base).rgb();
}
uint32_t GUIController::getTextColor()
{
   return ui->mainWindow->palette().color(QPalette::Text).rgb();
}
QPalette GUIController::getApplicationPalette()
{
   return ui->mainWindow->palette();
}
void GUIController::subscribeForThemeReloadEvent(ThemeListener* listener)
{
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   auto it = std::find_if(m_theme_listeners.begin(), m_theme_listeners.end(), [&](ThemeListener* theme_listener){return theme_listener == listener;});
   if (it == m_theme_listeners.end())
   {
      UT_Log(MAIN_GUI, ERROR, "new theme listener registered!");
      m_theme_listeners.push_back(listener);
   }
   else
   {
      UT_Log(MAIN_GUI, ERROR, "theme listener already registered!");
   }
}
void GUIController::unsubscribeFromThemeReloadEvent(ThemeListener* listener)
{
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   auto it = std::find_if(m_theme_listeners.begin(), m_theme_listeners.end(), [&](ThemeListener* theme_listener){return (theme_listener == listener);});
   UT_Log_If(it == m_theme_listeners.end(), MAIN_GUI, ERROR, "Theme listener not found!");
   if (it != m_theme_listeners.end())
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u unregistered");
      m_theme_listeners.erase(it);
   }
}
QWidget* GUIController::getParent()
{
   return this;
}
void GUIController::setStatusBarNotification(const std::string& notification, uint32_t timeout)
{
   m_status_bar_notification = notification;
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, notification.c_str());
}
void GUIController::setInfoLabelText(const std::string& text)
{
   m_info_label = text;
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, text.c_str());
}
void GUIController::setApplicationTitle(const std::string& title)
{
   m_application_title = title;
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, title.c_str());
}
uint32_t GUIController::countUserButtons()
{
   return ui->countUserButtons();
}
uint32_t GUIController::countPorts()
{
   return ui->countPorts();
}
uint32_t GUIController::countTraceFilters()
{
   return ui->countTraceFilters();
}
bool GUIController::onButtonClicked(const std::vector<uint8_t>& data)
{
   RPC::ButtonClickRequest request = RPC::convert<RPC::ButtonClickRequest>(data);
   RPC::ButtonClickReply reply = {};
   reply.clicked = false;

   uint32_t id = getButtonIDByName(request.button_name);
   if (id != UINT32_MAX)
   {
      std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
      for (auto& listener : m_button_listeners)
      {
         if (listener.id == id && listener.event == ButtonEvent::CLICKED && listener.listener)
         {
            listener.listener->onButtonEvent(id, ButtonEvent::CLICKED);
            reply.clicked = true;
         }
      }
   }

   UT_Log(MAIN_GUI, LOW, "%s clicked %u", __func__, reply.clicked);
   return rpc_server->respond<RPC::ButtonClickReply>(reply);
}
bool GUIController::onButtonContextMenuRequested(const std::vector<uint8_t>& data)
{
   RPC::ButtonContextMenuClickRequest request = RPC::convert<RPC::ButtonContextMenuClickRequest>(data);
   RPC::ButtonContextMenuClickReply reply = {};
   reply.clicked = false;

   uint32_t id = getButtonIDByName(request.button_name);
   if (id != UINT32_MAX)
   {
      std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
      for (auto& listener : m_button_listeners)
      {
         if (listener.id == id && listener.event == ButtonEvent::CONTEXT_MENU_REQUESTED && listener.listener)
         {
            listener.listener->onButtonEvent(id, ButtonEvent::CONTEXT_MENU_REQUESTED);
            reply.clicked = true;
         }
      }
   }

   UT_Log(MAIN_GUI, LOW, "%s clicked %u", __func__, reply.clicked);
   return rpc_server->respond<RPC::ButtonContextMenuClickReply>(reply);
}
bool GUIController::onSetCommandRequest(const std::vector<uint8_t>& data)
{
   RPC::SetCommandRequest request = RPC::convert<RPC::SetCommandRequest>(data);
   RPC::SetCommandReply reply = {};
   reply.reply = true;
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, request.command.c_str());

   std::lock_guard<std::mutex> lock(m_mutex);
   m_current_command = request.command;

   return rpc_server->respond<RPC::SetCommandReply>(reply);
}
bool GUIController::onGetCommandRequest(const std::vector<uint8_t>&)
{
   std::lock_guard<std::mutex> lock(m_mutex);

   RPC::GetCommandReply reply = {};
   reply.command = m_current_command;

   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, m_current_command.c_str());
   return rpc_server->respond<RPC::GetCommandReply>(reply);
}
bool GUIController::onGetPortLabel(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetPortLabelRequest request = RPC::convert<RPC::GetPortLabelRequest>(data);
   RPC::GetPortLabelReply reply = {};

   UT_Assert(request.id < m_port_labels.size());
   reply.id = request.id;
   reply.stylesheet = m_port_labels[request.id].stylesheet;

   reply.text = m_port_labels[request.id].text;
   UT_Log(MAIN_GUI, LOW, "%s id %u %s %s", __func__, reply.id, reply.text.c_str(), reply.stylesheet.c_str());
   return rpc_server->respond<RPC::GetPortLabelReply>(reply);
}
bool GUIController::onGetLineEnding(const std::vector<uint8_t>&)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   UT_Assert(m_current_line_ending != m_line_endings.end());
   RPC::GetLineEndingReply reply = {};
   reply.lineending = *m_current_line_ending;
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, reply.lineending.c_str());
   return rpc_server->respond<RPC::GetLineEndingReply>(reply);
}
bool GUIController::onGetAllLineEndings(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetAllLineEndingsReply reply = {};
   reply.lineendings = m_line_endings;

   UT_Log(MAIN_GUI, LOW, "%s size %u", __func__, (uint32_t)reply.lineendings.size());
   return rpc_server->respond<RPC::GetAllLineEndingsReply>(reply);
}
bool GUIController::onSetLineEnding(const std::vector<uint8_t>& data)
{
   RPC::SetLineEndingRequest request = RPC::convert<RPC::SetLineEndingRequest>(data);
   RPC::SetLineEndingReply reply = {};

   std::lock_guard<std::mutex> lock(m_mutex);
   m_current_line_ending = std::find(m_line_endings.begin(), m_line_endings.end(), request.lineending);
   reply.result = m_current_line_ending != m_line_endings.end();

   UT_Log(MAIN_GUI, LOW, "%s [%s] found %u ", __func__, request.lineending.c_str(), reply.result);
   return rpc_server->respond<RPC::SetLineEndingReply>(reply);
}
bool GUIController::onGetTargetPort(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetTargetPortReply reply = {};
   reply.port_name = m_active_ports.size()? *m_current_active_port : "";
   UT_Log(MAIN_GUI, LOW, "%s %s", __func__, reply.port_name.c_str());
   return rpc_server->respond<RPC::GetTargetPortReply>(reply);
}

bool GUIController::onGetAllTargetPorts(const std::vector<uint8_t>& data)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetAllTargetPortsReply reply = {};
   reply.port_names = m_active_ports;

   UT_Log(MAIN_GUI, LOW, "%s size %u", __func__, (uint32_t)reply.port_names.size());
   return rpc_server->respond<RPC::GetAllTargetPortsReply>(reply);
}
bool GUIController::onSetTargetPort(const std::vector<uint8_t>& data)
{
   RPC::SetTargetPortRequest request = RPC::convert<RPC::SetTargetPortRequest>(data);
   RPC::SetTargetPortReply reply = {};

   {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_current_active_port = std::find(m_active_ports.begin(), m_active_ports.end(), request.port_name);
      reply.result = m_current_active_port != m_active_ports.end();
   }

   //Notify listeners
   if (m_active_port_listener && reply.result)
   {
      UT_Log(MAIN_GUI, LOW, "%s [%s] notifying listeners", __func__, request.port_name.c_str());
      m_active_port_listener(request.port_name);
   }

   UT_Log(MAIN_GUI, LOW, "%s [%s] found %u ", __func__, request.port_name.c_str(), reply.result);
   return rpc_server->respond<RPC::SetTargetPortReply>(reply);
}
bool GUIController::onGetTraceFilterState(const std::vector<uint8_t>& data)
{
   RPC::GetTraceFilterStateRequest request = RPC::convert<RPC::GetTraceFilterStateRequest>(data);
   uint32_t id = getTraceFilterIDByName(request.filter_name);
   UT_Assert(id < m_trace_filters.size());

   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetTraceFilterStateReply reply = {};
   reply.filter_name = request.filter_name;
   reply.enabled = m_trace_filters[id].enabled;
   reply.background_color = m_trace_filters[id].colors.background_color;
   reply.font_color = m_trace_filters[id].colors.font_color;
   reply.text = m_trace_filters[id].text;

   UT_Log(MAIN_GUI, LOW, "%s id %u bg %u fb %u text %s enabled %u", __func__, id, reply.background_color, reply.font_color, reply.text.c_str(), reply.enabled);
   return rpc_server->respond<RPC::GetTraceFilterStateReply>(reply);
}
bool GUIController::onSetTraceFilter(const std::vector<uint8_t>& data)
{
   RPC::SetTraceFilterRequest request = RPC::convert<RPC::SetTraceFilterRequest>(data);
   uint32_t id = getTraceFilterIDByName(request.filter_name);
   UT_Assert(id < m_trace_filters.size());

   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::SetTraceFilterReply reply = {};

   reply.result = m_trace_filters[id].enabled;
   if (reply.result)
   {
      m_trace_filters[id].text = request.text;
   }

   UT_Log(MAIN_GUI, LOW, "%s id %u text %s result %u", __func__, id, request.text.c_str(), reply.result);
   return rpc_server->respond<RPC::SetTraceFilterReply>(reply);
}
bool GUIController::onGetCommandHistory(const std::vector<uint8_t>&)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   RPC::GetCommandHistoryReply reply = {};
   reply.history = m_commands_history;

   UT_Log(MAIN_GUI, LOW, "%s history size %u", __func__, reply.history.size());
   return rpc_server->respond<RPC::GetCommandHistoryReply>(reply);
}
bool GUIController::onGetTerminalViewContent(const std::vector<uint8_t>& data)
{
   RPC::GetTerminalViewContentReply reply = {};

   std::lock_guard<std::mutex> lock(m_mutex);
   for (auto& item : m_terminal_items)
   {
      reply.content.push_back({item.text, item.colors.background_color, item.colors.font_color});
   }

   UT_Log(MAIN_GUI, LOW, "%s size %u", __func__, reply.content.size());
   return rpc_server->respond<RPC::GetTerminalViewContentReply>(reply);
}
bool GUIController::onGetTraceViewContent(const std::vector<uint8_t>& data)
{
   RPC::GetTraceViewContentReply reply = {};

   std::lock_guard<std::mutex> lock(m_mutex);
   for (auto& item : m_trace_items)
   {
      reply.content.push_back({item.text, item.colors.background_color, item.colors.font_color});
   }

   UT_Log(MAIN_GUI, LOW, "%s size %u", __func__, reply.content.size());
   return rpc_server->respond<RPC::GetTraceViewContentReply>(reply);
}
bool GUIController::onSetPortSettings(const std::vector<uint8_t>& data)
{
   RPC::SetPortSettingsRequest request = RPC::convert<RPC::SetPortSettingsRequest>(data);
   uint32_t id = request.port_id;
   UT_Assert(id < g_port_settings.size());

   RPC::SetPortSettingsReply reply = {};
   reply.result = true;
   g_port_settings[id] = request.settings;

   UT_Log(MAIN_GUI, LOW, "%s result %u id %u settings %s", __func__, reply.result, id, request.settings.shortSettingsString().c_str());
   return rpc_server->respond<RPC::SetPortSettingsReply>(reply);
}
bool GUIController::onGetPortSettings(const std::vector<uint8_t>& data)
{
   RPC::GetPortSettingsRequest request = RPC::convert<RPC::GetPortSettingsRequest>(data);
   uint32_t id = request.port_id;
   UT_Assert(id < g_port_settings.size());

   RPC::GetPortSettingsReply reply = {};
   reply.port_id = request.port_id;
   reply.settings = g_port_settings[id];

   UT_Log(MAIN_GUI, LOW, "%s id %u settings %s", __func__, id, reply.settings.shortSettingsString().c_str());
   return rpc_server->respond<RPC::GetPortSettingsReply>(reply);
}
bool GUIController::onSetTraceFilterSettings(const std::vector<uint8_t>& data)
{
   RPC::SetTraceFilterSettingsRequest request = RPC::convert<RPC::SetTraceFilterSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_trace_filter_settings.size());

   RPC::SetTraceFilterSettingsReply reply = {};
   reply.result = true;
   g_trace_filter_settings[id] = request.settings;

   UT_Log(MAIN_GUI, LOW, "%s result %u id %u regex %s bg %.6x fg %.6x", __func__, reply.result, id, request.settings.regex.c_str(), request.settings.background, request.settings.font);
   return rpc_server->respond<RPC::SetTraceFilterSettingsReply>(reply);
}
bool GUIController::onGetTraceFilterSettings(const std::vector<uint8_t>& data)
{
   RPC::GetTraceFilterSettingsRequest request = RPC::convert<RPC::GetTraceFilterSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_trace_filter_settings.size());

   RPC::GetTraceFilterSettingsReply reply = {};
   reply.id = request.id;
   reply.settings = g_trace_filter_settings[id];

   UT_Log(MAIN_GUI, LOW, "%s id %u regex %s bg %.6x fg %.6x", __func__, reply.id, reply.settings.regex.c_str(), reply.settings.background, reply.settings.font);
   return rpc_server->respond<RPC::GetTraceFilterSettingsReply>(reply);
}
bool GUIController::onSetUserButtonSettings(const std::vector<uint8_t>& data)
{
   RPC::SetUserButtonSettingsRequest request = RPC::convert<RPC::SetUserButtonSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_user_button_settings.size());

   RPC::SetUserButtonSettingsReply reply = {};
   reply.result = true;
   g_user_button_settings[id] = request.settings;

   UT_Log(MAIN_GUI, LOW, "%s result %u id %u name %s", __func__, reply.result, id, request.settings.button_name.c_str());
   return rpc_server->respond<RPC::SetUserButtonSettingsReply>(reply);
}
bool GUIController::onGetUserButtonSettings(const std::vector<uint8_t>& data)
{
   RPC::GetUserButtonSettingsRequest request = RPC::convert<RPC::GetUserButtonSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_user_button_settings.size());

   RPC::GetUserButtonSettingsReply reply = {};
   reply.id = request.id;
   reply.settings = g_user_button_settings[id];

   UT_Log(MAIN_GUI, LOW, "%s id %u name %s", __func__, id, reply.settings.button_name.c_str());
   return rpc_server->respond<RPC::GetUserButtonSettingsReply>(reply);
}
bool GUIController::onGetMessageBox(const std::vector<uint8_t>& data)
{
   RPC::GetMessageBoxReply reply = {};
   reply.text = g_messagebox_details.text;
   reply.icon = g_messagebox_details.icon;
   reply.title = g_messagebox_details.title;

   UT_Log(MAIN_GUI, LOW, "%s", __func__);
   return rpc_server->respond<RPC::GetMessageBoxReply>(reply);
}
bool GUIController::onResetMessageBox(const std::vector<uint8_t>&)
{
   RPC::ResetMessageBoxReply reply = {};
   reply.result = g_messagebox_details.reported;
   g_messagebox_details = {};

   UT_Log(MAIN_GUI, LOW, "%s", __func__);
   return rpc_server->respond<RPC::ResetMessageBoxReply>(reply);
}
bool GUIController::onSetLoggingPath(const std::vector<uint8_t>& data)
{
   RPC::SetLoggingPathRequest request = RPC::convert<RPC::SetLoggingPathRequest>(data);
   g_logging_path = request.path;
   RPC::SetLoggingPathReply reply = {};
   reply.result = true;

   UT_Log(MAIN_GUI, LOW, "%s path %s", __func__, g_logging_path.c_str());
   return rpc_server->respond<RPC::SetLoggingPathReply>(reply);
}
bool GUIController::onGetLoggingPath(const std::vector<uint8_t>& data)
{
   RPC::GetLoggingPathReply reply = {};
   reply.path = g_logging_path;
   UT_Log(MAIN_GUI, LOW, "%s path %s", __func__, g_logging_path.c_str());
   return rpc_server->respond<RPC::GetLoggingPathReply>(reply);
}
void GUIController::onCurrentPortSelectionChanged(int index)
{
}
uint32_t GUIController::getButtonIDByName(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   for (uint32_t i = 0; i < m_buttons_cache.size(); i++)
   {
      if (m_buttons_cache[i].name == name)
      {
         result = i;
         break;
      }
   }
   return result;
}
uint32_t GUIController::getTraceFilterIDByName(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   for (uint32_t i = 0; i < m_trace_filters.size(); i++)
   {
      if (m_trace_filters[i].name == name)
      {
         result = i;
         break;
      }
   }
   return result;
}
std::string GUIController::onLoggingPathDialogShow(QWidget* parent, const std::string& current_path, bool allow_edit)
{
   std::string result = g_logging_path;
   g_logging_path = current_path;
   UT_Log(MAIN_GUI, LOW, "%s current %s new %s", __func__, current_path.c_str(), result.c_str());
   return result;
}
void GUIController::onMessageBoxShow(Dialogs::MessageBox::Icon icon, const std::string& window_title, const std::string& text, QPalette)
{
   g_messagebox_details = {true, window_title, icon, text};
}
std::optional<bool> GUIController::onPortSettingsDialogShow(QWidget* parent, const Dialogs::PortSettingDialog::Settings& current_settings, Dialogs::PortSettingDialog::Settings& out_settings, bool allow_edit)
{
   UT_Assert(current_settings.port_id < g_port_settings.size());
   out_settings = g_port_settings[current_settings.port_id];
   UT_Log(MAIN_GUI, LOW, "%s %u[%s]", __func__, current_settings.port_id, current_settings.port_name.c_str());
   return true;
}
std::optional<bool> GUIController::onTraceFilterSettingDialogShow(QWidget* parent, const Dialogs::TraceFilterSettingDialog::Settings& current_settings, Dialogs::TraceFilterSettingDialog::Settings& out_settings, bool allow_edit)
{
   UT_Assert(current_settings.id < g_trace_filter_settings.size());
   out_settings = g_trace_filter_settings[current_settings.id];
   UT_Log(MAIN_GUI, LOW, "%s id %u", __func__, current_settings.id);
   return true;
}
std::optional<bool> GUIController::onUserButtonSettingsDialogShow(QWidget* parent, const Dialogs::UserButtonDialog::Settings& current_settings, Dialogs::UserButtonDialog::Settings& out_settings, bool allow_edit)
{
   UT_Assert(current_settings.id < g_user_button_settings.size());
   out_settings = g_user_button_settings[current_settings.id];
   UT_Log(MAIN_GUI, LOW, "%s id %u", __func__, current_settings.id);
   return true;
}

bool GUIController::onGetButtonStateRequest(const std::vector<uint8_t>& data)
{
   RPC::GetButtonStateRequest request = RPC::convert<RPC::GetButtonStateRequest>(data);

   std::lock_guard<std::mutex> lock(m_mutex);
   auto it = std::find_if(m_buttons_cache.begin(), m_buttons_cache.end(), [&](const ButtonCache& button){return button.name == request.button_name;});
   UT_Assert(it != m_buttons_cache.end());

   RPC::GetButtonStateReply reply = {};
   reply.button_name = it->name;
   reply.background_color = it->colors.background_color;
   reply.font_color = it->colors.font_color;
   reply.checkable = it->checkable;
   reply.checked = it->checked;
   reply.enabled = it->enabled;
   reply.text = it->text;

   UT_Log(MAIN_GUI, LOW, "%s name %s bg %.8x font %.8x", __func__, reply.button_name.c_str(), reply.background_color, reply.font_color);
   return rpc_server->respond<RPC::GetButtonStateReply>(reply);
}

