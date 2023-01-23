#pragma once
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <sstream>

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class ButtonEvent
{
   CLICKED,
   CONTEXT_MENU_REQUESTED,
};

class ThemeListener
{
public:
   virtual ~ThemeListener(){};
   virtual void onThemeChange(Theme theme) = 0;
};

class ButtonEventListener
{
public:
   virtual ~ButtonEventListener(){};
   virtual void onButtonEvent(uint32_t button_id, ButtonEvent event) = 0;
};

class GUIController : public QMainWindow

{
   Q_OBJECT
public:
   GUIController(QWidget *parent);
   ~GUIController();

   struct ButtonEventItem
   {
      uint32_t id;
      ButtonEvent event;
      ButtonEventListener* listener;
   };

   struct LogViewStatus
   {
      uint32_t item_count;
      bool scrolling_enabled;
   };

   Ui::MainWindow *ui;
   Theme m_current_theme;
   LogViewStatus m_terminal_view_status;
   LogViewStatus m_trace_view_status;
   std::function<bool(const std::string&)> m_active_port_listener;

   std::mutex m_command_mutex;
   std::mutex m_theme_listeners_mutex;
   std::vector<ThemeListener*> m_theme_listeners;
   std::mutex m_button_listeners_mutex;
   std::vector<ButtonEventItem> m_button_listeners;
   //maps widgets to related buttons (i.e. traceFilter to traceFilterButton)
   std::map<QObject*, QPushButton*> m_shortcuts_map;

   void run();

   /* Generic buttons handling */
   uint32_t getButtonID(const std::string& name);
   void subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*);
   void unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*);
   void setButtonBackgroundColor(uint32_t button_id, uint32_t color);
   void setButtonFontColor(uint32_t button_id, uint32_t color);
   void setButtonCheckable(uint32_t button_id, bool checkable);
   void setButtonChecked(uint32_t button_id, bool checked);
   void setButtonEnabled(uint32_t button_id, bool enabled);
   void setButtonText(uint32_t button_id, const std::string& text);

   /* Terminal view */
   void addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color);
   void clearTerminalView();
   uint32_t countTerminalItems();
   void setTerminalScrollingEnabled(bool enabled);

   /* Trace view */
   void addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color);
   void clearTraceView();
   uint32_t countTraceItems();
   void setTraceScrollingEnabled(bool enabled);

   /* Port handling */
   void subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback);
   void registerPortOpened(const std::string& port_name);
   void registerPortClosed(const std::string& port_name);
   void setPortLabelText(uint8_t id, const std::string& description);
   void setPortLabelStylesheet(uint8_t id, const std::string& stylesheet);

   /* Command history */
   void setCommandsHistory(const std::vector<std::string>& history);
   void addCommandToHistory(const std::string& history);
   std::string getCurrentCommand();

   /* Line ending */
   std::string getCurrentLineEnding();
   void addLineEnding(const std::string& ending);
   void setCurrentLineEnding(const std::string& ending);

   /* Trace filters */
   uint32_t getTraceFilterID(const std::string& name);
   void setTraceFilter(uint8_t id, const std::string& filter);
   std::string getTraceFilter(uint8_t id);
   void setTraceFilterEnabled(uint8_t id, bool enabled);
   void setTraceFilterBackgroundColor(uint32_t id, uint32_t color);
   void setTraceFilterFontColor(uint32_t id, uint32_t color);

   /* Theme handling */
   void reloadTheme(Theme theme);
   Theme currentTheme();
   std::string themeToName(Theme theme);
   Theme nameToTheme(const std::string& name);
   void subscribeForThemeReloadEvent(ThemeListener*);
   void unsubscribeFromThemeReloadEvent(ThemeListener*);

   /* Application palette */
   uint32_t getBackgroundColor();
   uint32_t getTerminalBackgroundColor();
   uint32_t getTextColor();
   QPalette getApplicationPalette();
   QWidget* getParent();

   /* Others */
   void setStatusBarNotification(const std::string& notification, uint32_t timeout);
   void setInfoLabelText(const std::string& text);
   void setApplicationTitle(const std::string& title);
   uint32_t countUserButtons();
   uint32_t countPorts();
   uint32_t countTraceFilters();

private:
   struct ColorCache
   {
      uint32_t background_color;
      uint32_t font_color;
   };
   void onButtonClicked();
   void onButtonContextMenuRequested();
   void onCurrentPortSelectionChanged(int);
   void onPortSwitchRequest();

   QPushButton* getButtonByName(const std::string& name);

   /* TestFrameworkAPI handler */
   bool onGetButtonStateRequest(const std::vector<uint8_t>&);

   std::vector<std::pair<std::string, ColorCache>> m_buttons_color_cache;
};