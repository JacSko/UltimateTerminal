#pragma once

#include <mutex>
#include <condition_variable>
#include <sstream>

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "GUIControllerHelpers.hpp"

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
   CommandExecutor* m_current_command;

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
   bool executeGUIRequest(CommandExecutor* request);

signals:
   void setButtonBackgroundColorSignal(qint32 id, qint32 color);
   void setButtonFontColorSignal(qint32 id, qint32 color);
   void setButtonCheckableSignal(qint32 id, bool checkable);
   void setButtonCheckedSignal(qint32 id, bool checked);
   void setButtonEnabledSignal(qint32 id, bool enabled);
   void setButtonTextSignal(qint32 id, QString text);
   void clearTerminalViewSignal();
   void clearTraceViewSignal();
   void addToTerminalViewSignal(const std::string& text, qint32 background_color, qint32 font_color);
   void addToTerminalViewSignal(QString text, qint32 background_color, qint32 font_color);
   void addToTraceViewSignal(QString text, qint32 background_color, qint32 font_color);
   void setTerminalScrollingEnabledSignal(bool enabled);
   void setTraceScrollingEnabledSignal(bool enabled);
   void registerPortOpenedSignal(QString name);
   void registerPortClosedSignal(QString name);
   void setCommandHistorySignal(QVector<QString> history);
   void addCommandToHistorySignal(QString command);
   void guiRequestSignal();
   void addLineEndingSignal(QString ending);
   void setCurrentLineEndingSignal(QString ending);
   void setTraceFilterSignal(qint8 id, QString filter);
   void setTraceFilterEnabledSignal(qint8 id, bool enabled);
   void setTraceFilterBackgroundColorSignal(qint32 id, qint32 color);
   void setTraceFilterFontColorSignal(qint32 id, qint32 color);
   void setPortLabelTextSignal(qint8 id, QString description);
   void setPortLabelStylesheetSignal(qint8 id, QString stylesheet);
   void reloadThemeSignal(qint8);
   void setStatusBarNotificationSignal(QString notification, qint32 timeout);
   void setInfoLabelTextSignal(QString text);
   void setApplicationTitle(QString title);
public slots:
   void onButtonClicked();
   void onButtonContextMenuRequested();
   void onCurrentPortSelectionChanged(int);
   void onPortSwitchRequest();
   void onButtonBackgroundColorSignal(qint32 id, qint32 color);
   void onButtonFontColorSignal(qint32 id, qint32 color);
   void onButtonCheckableSignal(qint32 id, bool checkable);
   void onButtonCheckedSignal(qint32 id, bool checked);
   void onButtonEnabledSignal(qint32 id, bool enabled);
   void onButtonTextSignal(qint32 id, QString text);
   void onClearTerminalViewSignal();
   void onClearTraceViewSignal();
   void onAddToTerminalViewSignal(QString text, qint32 background_color, qint32 font_color);
   void onAddToTraceViewSignal(QString text, qint32 background_color, qint32 font_color);
   void onSetTerminalScrollingEnabledSignal(bool enabled);
   void onSetTraceScrollingEnabledSignal(bool enabled);
   void onRegisterPortOpenedSignal(QString name);
   void onRegisterPortClosedSignal(QString name);
   void onSetCommandHistorySignal(QVector<QString> history);
   void onAddCommandToHistorySignal(QString command);
   void onGuiRequestSignal();
   void onAddLineEndingSignal(QString ending);
   void onSetCurrentLineEndingSignal(QString ending);
   void onSetTraceFilterSignal(qint8 id, QString filter);
   void onSetTraceFilterEnabledSignal(qint8 id, bool enabled);
   void onSetTraceFilterBackgroundColorSignal(qint32 id, qint32 color);
   void onSetTraceFilterFontColorSignal(qint32 id, qint32 color);
   void onSetPortLabelTextSignal(qint8 id, QString description);
   void onSetPortLabelStylesheetSignal(qint8 id, QString stylesheet);
   void onReloadThemeSignal(qint8);
   void onSetStatusBarNotificationSignal(QString notification, qint32 timeout);
   void onSetInfoLabelTextSignal(QString text);
   void onSetApplicationTitle(QString title);
};
