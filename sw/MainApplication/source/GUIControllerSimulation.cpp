#include "GUIController.h"
#include <QtCore/QVector>

#undef APPLICATION_THEME
#define APPLICATION_THEME(theme_name) #theme_name,
std::array<std::string, (uint32_t)Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME


GUIController::GUIController(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_current_theme(Theme::DEFAULT),
m_terminal_view_status{},
m_trace_view_status{},
m_current_command(nullptr)
{
}
GUIController::~GUIController()
{
}
void GUIController::run()
{
}
uint32_t GUIController::getButtonID(const std::string& name)
{
}
void GUIController::subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
}
void GUIController::unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
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
}
uint32_t GUIController::countTraceItems()
{
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
}
std::string GUIController::getCurrentLineEnding()
{
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
}
uint32_t GUIController::getTerminalBackgroundColor()
{
}
uint32_t GUIController::getTextColor()
{
}
QPalette GUIController::getApplicationPalette()
{
}
void GUIController::subscribeForThemeReloadEvent(ThemeListener* listener)
{
}
void GUIController::unsubscribeFromThemeReloadEvent(ThemeListener* listener)
{
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
}
uint32_t GUIController::countPorts()
{
}
uint32_t GUIController::countTraceFilters()
{
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
