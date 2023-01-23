#include "GUIControllerSimulation.h"
#include <QtCore/QVector>
#include "RPCServer.h"
#include "RPCCommon.h"
#include "TestFrameworkAPI.h"

#undef APPLICATION_THEME
#define APPLICATION_THEME(theme_name) #theme_name,
std::array<std::string, (uint32_t)Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME

/* global objects declaration */
std::unique_ptr<RPC::RPCServer> rpc_server;


GUIController::GUIController(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_current_theme(Theme::DEFAULT),
m_terminal_view_status{},
m_trace_view_status{}
{
   rpc_server = std::unique_ptr<RPC::RPCServer>(new RPC::RPCServer());
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetButtonState, std::bind(&GUIController::onGetButtonStateRequest, this, std::placeholders::_1));
}
GUIController::~GUIController()
{
}
void GUIController::run()
{
   ui->setupUi(this);
   rpc_server->start(8888);
   m_buttons_color_cache.resize(ui->getButtons().size());
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
}
void GUIController::setButtonFontColor(uint32_t button_id, uint32_t color)
{
}
void GUIController::setButtonCheckable(uint32_t button_id, bool checkable)
{
}
void GUIController::setButtonChecked(uint32_t button_id, bool checked)
{
}
void GUIController::setButtonEnabled(uint32_t button_id, bool enabled)
{
}
void GUIController::setButtonText(uint32_t button_id, const std::string& text)
{
}
void GUIController::clearTerminalView()
{
}
void GUIController::clearTraceView()
{
}
void GUIController::addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
}
void GUIController::addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
}
uint32_t GUIController::countTerminalItems()
{
   return 0;
}
uint32_t GUIController::countTraceItems()
{
   return 0;
}
void GUIController::setTerminalScrollingEnabled(bool enabled)
{
}
void GUIController::setTraceScrollingEnabled(bool enabled)
{
}
void GUIController::subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback)
{
}
void GUIController::registerPortOpened(const std::string& port_name)
{
}
void GUIController::registerPortClosed(const std::string& port_name)
{
}
void GUIController::setCommandsHistory(const std::vector<std::string>& history)
{
}
void GUIController::addCommandToHistory(const std::string& history)
{
}
std::string GUIController::getCurrentCommand()
{
   return "";
}
std::string GUIController::getCurrentLineEnding()
{
   return "\n";
}
void GUIController::addLineEnding(const std::string& ending)
{
}
void GUIController::setCurrentLineEnding(const std::string& ending)
{
}
void GUIController::setTraceFilter(uint8_t id, const std::string& filter)
{
}
std::string GUIController::getTraceFilter(uint8_t id)
{
   return "";
}
void GUIController::setTraceFilterEnabled(uint8_t id, bool enabled)
{
}
void GUIController::setTraceFilterBackgroundColor(uint32_t id, uint32_t color)
{
}
void GUIController::setTraceFilterFontColor(uint32_t id, uint32_t color)
{
}
void GUIController::setPortLabelText(uint8_t id, const std::string& description)
{
}
void GUIController::setPortLabelStylesheet(uint8_t id, const std::string& stylesheet)
{
}
void GUIController::reloadTheme(Theme theme)
{
}
Theme GUIController::currentTheme()
{
   return Theme::DEFAULT;
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
}
void GUIController::setInfoLabelText(const std::string& text)
{
}
void GUIController::setApplicationTitle(const std::string& title)
{
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
void GUIController::onButtonClicked()
{
}
void GUIController::onButtonContextMenuRequested()
{
}
void GUIController::onCurrentPortSelectionChanged(int index)
{
}
void GUIController::onPortSwitchRequest()
{
}

/* TestFrameworkAPI handling*/
bool GUIController::onGetButtonStateRequest(const std::vector<uint8_t>& data)
{
   RPC::GetButtonStateRequest request = RPC::convert<RPC::GetButtonStateRequest>(data);
   QPushButton* button = getButtonByName(request.button_name);

   RPC::GetButtonStateReply reply = {};

   return rpc_server->respond<RPC::GetButtonStateReply>(reply);
}

QPushButton* GUIController::getButtonByName(const std::string& name)
{
   QPushButton* result = nullptr;
   for (auto& button : ui->getButtons())
   {
      if (button->objectName().toStdString() == name)
      {
         result = button;
         break;
      }
   }
   return result;
}

