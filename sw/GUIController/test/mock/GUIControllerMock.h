#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "GUIController.h"


struct GUIControllerMock
{
   MOCK_METHOD0(run, void());
   MOCK_METHOD1(getButtonID, uint32_t(const std::string&));
   MOCK_METHOD3(subscribeForButtonEvent, void(uint32_t, ButtonEvent, ButtonEventListener*));
   MOCK_METHOD3(unsubscribeFromButtonEvent, void(uint32_t, ButtonEvent, ButtonEventListener*));
   MOCK_METHOD2(setButtonBackgroundColor, void(uint32_t, uint32_t));
   MOCK_METHOD2(setButtonFontColor, void(uint32_t, uint32_t));
   MOCK_METHOD2(setButtonCheckable, void(uint32_t, bool));
   MOCK_METHOD2(setButtonChecked, void(uint32_t, bool));
   MOCK_METHOD2(setButtonEnabled, void(uint32_t, bool));
   MOCK_METHOD2(setButtonText, void(uint32_t, const std::string&));
   MOCK_METHOD0(clearTerminalView, void());
   MOCK_METHOD0(clearTraceView, void());
   MOCK_METHOD3(addToTerminalView, void(const std::string&, uint32_t, uint32_t));
   MOCK_METHOD3(addToTraceView, void(const std::string&, uint32_t, uint32_t));
   MOCK_METHOD0(countTerminalItems, uint32_t());
   MOCK_METHOD0(countTraceItems, uint32_t());
   MOCK_METHOD0(scrollTerminalToBottom, void());
   MOCK_METHOD0(scrollTraceViewToBottom, void());
   MOCK_METHOD1(subscribeForActivePortChangedEvent, void(std::function<bool(const std::string&)>));
   MOCK_METHOD1(registerPortOpened, void(const std::string&));
   MOCK_METHOD1(registerPortClosed, void(const std::string&));
   MOCK_METHOD1(setCommandsHistory, void(const std::vector<std::string>&));
   MOCK_METHOD1(addCommandToHistory, void(const std::string&));
   MOCK_METHOD0(getCurrentCommand, std::string());
   MOCK_METHOD0(clearCommand, void());
   MOCK_METHOD0(getCurrentLineEnding, std::string());
   MOCK_METHOD1(addLineEnding, void(const std::string&));
   MOCK_METHOD1(setCurrentLineEnding, void(const std::string&));
   MOCK_METHOD2(setTraceFilter, void(uint8_t id, const std::string&));
   MOCK_METHOD1(getTraceFilter, std::string(uint8_t id));
   MOCK_METHOD2(setTraceFilterEnabled, void(uint8_t, bool));
   MOCK_METHOD2(setTraceFilterBackgroundColor, void(uint32_t, uint32_t));
   MOCK_METHOD2(setTraceFilterFontColor, void(uint32_t, uint32_t));
   MOCK_METHOD2(setPortLabelText, void(uint8_t, const std::string&));
   MOCK_METHOD1(reloadTheme, void(Theme));
   MOCK_METHOD0(currentTheme, Theme());
   MOCK_METHOD1(themeToName, std::string(Theme));
   MOCK_METHOD1(nameToTheme, Theme(const std::string&));
   MOCK_METHOD0(getBackgroundColor, uint32_t());
   MOCK_METHOD0(getTerminalBackgroundColor, uint32_t());
   MOCK_METHOD0(getTextColor, uint32_t());
   MOCK_METHOD0(getApplicationPalette, QPalette());
   MOCK_METHOD1(subscribeForThemeReloadEvent, void(ThemeListener*));
   MOCK_METHOD1(unsubscribeFromThemeReloadEvent, void(ThemeListener*));
   MOCK_METHOD0(getParent, QWidget*());
   MOCK_METHOD2(setStatusBarNotification, void(const std::string&, uint32_t));
   MOCK_METHOD1(setInfoLabelText, void(const std::string&));
   MOCK_METHOD1(setApplicationTitle, void(const std::string&));
   MOCK_METHOD0(countUserButtons, uint32_t());
   MOCK_METHOD0(countPorts, uint32_t());
   MOCK_METHOD0(countTraceFilters, uint32_t());
};

GUIControllerMock* g_gui_controller_mock;

void GUIControllerMock_init()
{
   if (!g_gui_controller_mock)
   {
      g_gui_controller_mock = new GUIControllerMock();
   }
}
void GUIControllerMock_deinit()
{
   if (g_gui_controller_mock)
   {
      delete g_gui_controller_mock;
      g_gui_controller_mock = nullptr;
   }
}
GUIControllerMock* GUIControllerMock_get()
{
   UT_Assert(g_gui_controller_mock && "Create GUIControllerMock mock first!");
   return g_gui_controller_mock;
}


GUIController::GUIController(QWidget *)
{
}
GUIController::~GUIController()
{
}
void GUIController::run()
{
   g_gui_controller_mock->run();
}
uint32_t GUIController::getButtonID(const std::string& name)
{
   return g_gui_controller_mock->getButtonID(name);
}
void GUIController::subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   g_gui_controller_mock->subscribeForButtonEvent(button_id, event, listener);
}
void GUIController::unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   g_gui_controller_mock->unsubscribeFromButtonEvent(button_id, event, listener);
}
void GUIController::setButtonBackgroundColor(uint32_t button_id, uint32_t color)
{
   g_gui_controller_mock->setButtonBackgroundColor(button_id, color);
}
void GUIController::setButtonFontColor(uint32_t button_id, uint32_t color)
{
   g_gui_controller_mock->setButtonFontColor(button_id, color);
}
void GUIController::setButtonCheckable(uint32_t button_id, bool checkable)
{
   g_gui_controller_mock->setButtonCheckable(button_id, checkable);
}
void GUIController::setButtonChecked(uint32_t button_id, bool checked)
{
   g_gui_controller_mock->setButtonChecked(button_id, checked);
}
void GUIController::setButtonEnabled(uint32_t button_id, bool enabled)
{
   g_gui_controller_mock->setButtonEnabled(button_id, enabled);
}
void GUIController::setButtonText(uint32_t button_id, const std::string& text)
{
   g_gui_controller_mock->setButtonText(button_id, text);
}
void GUIController::clearTerminalView()
{
   g_gui_controller_mock->clearTerminalView();
}
void GUIController::clearTraceView()
{
   g_gui_controller_mock->clearTraceView();
}
void GUIController::addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   g_gui_controller_mock->addToTerminalView(text, background_color, font_color);
}
void GUIController::addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   g_gui_controller_mock->addToTraceView(text, background_color, font_color);
}
uint32_t GUIController::countTerminalItems()
{
   return g_gui_controller_mock->countTerminalItems();
}
uint32_t GUIController::countTraceItems()
{
   return g_gui_controller_mock->countTraceItems();
}
void GUIController::scrollTerminalToBottom()
{
   g_gui_controller_mock->scrollTerminalToBottom();
}
void GUIController::scrollTraceViewToBottom()
{
   g_gui_controller_mock->scrollTraceViewToBottom();
}
void GUIController::subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback)
{
   g_gui_controller_mock->subscribeForActivePortChangedEvent(callback);
}
void GUIController::registerPortOpened(const std::string& port_name)
{
   g_gui_controller_mock->registerPortOpened(port_name);
}
void GUIController::registerPortClosed(const std::string& port_name)
{
   g_gui_controller_mock->registerPortClosed(port_name);
}
void GUIController::setCommandsHistory(const std::vector<std::string>& history)
{
   g_gui_controller_mock->setCommandsHistory(history);
}
void GUIController::addCommandToHistory(const std::string& history)
{
   g_gui_controller_mock->addCommandToHistory(history);
}
std::string GUIController::getCurrentCommand()
{
   return g_gui_controller_mock->getCurrentCommand();
}
void GUIController::clearCommand()
{
   return g_gui_controller_mock->clearCommand();
}
std::string GUIController::getCurrentLineEnding()
{
   return g_gui_controller_mock->getCurrentLineEnding();
}
void GUIController::addLineEnding(const std::string& ending)
{
   g_gui_controller_mock->addLineEnding(ending);
}
void GUIController::setCurrentLineEnding(const std::string& ending)
{
   g_gui_controller_mock->setCurrentLineEnding(ending);
}
void GUIController::setTraceFilter(uint8_t id, const std::string& filter)
{
   g_gui_controller_mock->setTraceFilter(id, filter);
}
std::string GUIController::getTraceFilter(uint8_t id)
{
   return g_gui_controller_mock->getTraceFilter(id);
}
void GUIController::setTraceFilterEnabled(uint8_t id, bool enabled)
{
   g_gui_controller_mock->setTraceFilterEnabled(id, enabled);
}
void GUIController::setTraceFilterBackgroundColor(uint32_t id, uint32_t color)
{
   g_gui_controller_mock->setTraceFilterBackgroundColor(id, color);
}
void GUIController::setTraceFilterFontColor(uint32_t id, uint32_t color)
{
   g_gui_controller_mock->setTraceFilterFontColor(id, color);
}
void GUIController::setPortLabelText(uint8_t id, const std::string& description)
{
   g_gui_controller_mock->setPortLabelText(id, description);
}
void GUIController::reloadTheme(Theme theme)
{
   g_gui_controller_mock->reloadTheme(theme);
}
Theme GUIController::currentTheme()
{
   return g_gui_controller_mock->currentTheme();
}
std::string GUIController::themeToName(Theme theme)
{
   return g_gui_controller_mock->themeToName(theme);
}
Theme GUIController::nameToTheme(const std::string& name)
{
   return g_gui_controller_mock->nameToTheme(name);
}
uint32_t GUIController::getBackgroundColor()
{
   return g_gui_controller_mock->getBackgroundColor();
}
uint32_t GUIController::getTerminalBackgroundColor()
{
   return g_gui_controller_mock->getTerminalBackgroundColor();
}
uint32_t GUIController::getTextColor()
{
   return g_gui_controller_mock->getTextColor();
}
QPalette GUIController::getApplicationPalette()
{
   return g_gui_controller_mock->getApplicationPalette();
}
void GUIController::subscribeForThemeReloadEvent(ThemeListener* listener)
{
   g_gui_controller_mock->subscribeForThemeReloadEvent(listener);
}
void GUIController::unsubscribeFromThemeReloadEvent(ThemeListener* listener)
{
   g_gui_controller_mock->unsubscribeFromThemeReloadEvent(listener);
}
QWidget* GUIController::getParent()
{
   return g_gui_controller_mock->getParent();
}
void GUIController::setStatusBarNotification(const std::string& notification, uint32_t timeout)
{
   g_gui_controller_mock->setStatusBarNotification(notification, timeout);
}
void GUIController::setInfoLabelText(const std::string& text)
{
   g_gui_controller_mock->setInfoLabelText(text);
}
void GUIController::setApplicationTitle(const std::string& title)
{
   g_gui_controller_mock->setApplicationTitle(title);
}
uint32_t GUIController::countUserButtons()
{
   return g_gui_controller_mock->countUserButtons();
}
uint32_t GUIController::GUIController::countPorts()
{
   return g_gui_controller_mock->countPorts();
}
uint32_t GUIController::GUIController::countTraceFilters()
{
   return g_gui_controller_mock->countTraceFilters();
}
