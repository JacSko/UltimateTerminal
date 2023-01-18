#pragma once

#include <mutex>
#include <condition_variable>
#include <sstream>

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CommandExecutor
{
public:
   CommandExecutor()
   {
      ready = false;
   }
   virtual ~CommandExecutor(){};
   virtual void execute(Ui::MainWindow* ui) = 0;
   virtual bool waitReady(uint32_t timeout)
   {
      std::unique_lock<std::mutex> lock(mutex);
      bool result = cond_var.wait_for(lock, std::chrono::milliseconds(timeout), [&](){return ready.load();});
      ready = false;
      return result;
   }
protected:
   std::atomic<bool> ready;
   std::mutex mutex;
   std::condition_variable cond_var;
};

class GetCommandRequest : public CommandExecutor
{
public:
   std::string command;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      command = ui->textEdit->currentText().toStdString();
      ready = true;
   }
};

class GetLineEndingRequest : public CommandExecutor
{
public:
   std::string line_ending;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      line_ending = ui->lineEndingComboBox->currentText().toStdString();
      ready = true;
   }
};

class GetTraceFilterRequest : public CommandExecutor
{
public:
   GetTraceFilterRequest(uint8_t id):
   id(id)
   {}
   uint8_t id;
   std::string trace_filter;
private:
   void execute(Ui::MainWindow* ui)
   {
      std::unique_lock<std::mutex> lock(mutex);
      auto filters = ui->getTraceFilters();
      UT_Assert(filters.size() > id);
      trace_filter = filters[id].line_edit->text().toStdString();
      ready = true;
   }
};

class Stylesheet
{
public:
    enum class Item
    {
        BACKGROUND_COLOR,
        COLOR
    };

    Stylesheet()
    {

    }
    Stylesheet(const std::string& stylesheet)
    {
       printf("%s stylesheet %s\n", __func__, stylesheet.c_str());
        m_colors_map[Item::BACKGROUND_COLOR] = 0;
        m_colors_map[Item::COLOR] = 0;
        m_stylesheet = stylesheet;
        parseStylesheet();
    }

    uint32_t getColor(Item type)
    {
        return m_colors_map[type];
    }

    void setColor(Item type, uint32_t color)
    {
        m_colors_map[type] = color;
        createStylesheet();
    }

    const std::string& stylesheet() {return m_stylesheet;}

private:
    void createStylesheet()
    {
        char stylesheet [512];
        std::snprintf(stylesheet, 512, "background-color: #%.6x;color: #%.6x;", m_colors_map[Item::BACKGROUND_COLOR], m_colors_map[Item::COLOR]);
        m_stylesheet = std::string(stylesheet);
    }
    void parseStylesheet()
    {
        std::stringstream ss(m_stylesheet);
        std::string tag;
        while(std::getline(ss, tag, ';'))
        {
            printf("tag: %s\n", tag.c_str());
            std::string property = tag.substr(0, tag.find(':'));
            std::string value = tag.substr(tag.find('#') + 1, tag.size());
            uint32_t color = strtol(value.c_str(), NULL, 16);
            Item type = Item::BACKGROUND_COLOR;
            if (property == "background-color")
            {
                type = Item::BACKGROUND_COLOR;
            }
            else if (property == "color")
            {
                type = Item::COLOR;
            }
            m_colors_map[type] = color;
        }
    }
    std::string m_stylesheet;
    std::map<Item, uint32_t> m_colors_map;
};

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
   uint32_t getButtonID(const std::string& name);
   void subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*);
   void unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*);
   void setButtonBackgroundColor(uint32_t button_id, uint32_t color);
   void setButtonFontColor(uint32_t button_id, uint32_t color);
   void setButtonCheckable(uint32_t button_id, bool checkable);
   void setButtonChecked(uint32_t button_id, bool checked);
   void setButtonEnabled(uint32_t button_id, bool enabled);
   void setButtonText(uint32_t button_id, const std::string& text);
   void clearTerminalView();
   void clearTraceView();
   void addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color);
   void addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color);
   uint32_t countTerminalItems();
   uint32_t countTraceItems();
   void setTerminalScrollingEnabled(bool enabled);
   void setTraceScrollingEnabled(bool enabled);
   void subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback);
   void registerPortOpened(const std::string& port_name);
   void registerPortClosed(const std::string& port_name);
   void setCommandsHistory(const std::vector<std::string>& history);
   void addCommandToHistory(const std::string& history);
   std::string getCurrentCommand();
   std::string getCurrentLineEnding();
   void addLineEnding(const std::string& ending);
   void setCurrentLineEnding(const std::string& ending);
   uint32_t getTraceFilterID(const std::string& name);
   void setTraceFilter(uint8_t id, const std::string& filter);
   std::string getTraceFilter(uint8_t id);
   void setTraceFilterEnabled(uint8_t id, bool enabled);
   void setTraceFilterBackgroundColor(uint32_t id, uint32_t color);
   void setTraceFilterFontColor(uint32_t id, uint32_t color);
   void setPortLabelText(uint8_t id, const std::string& description);
   void setPortLabelStylesheet(uint8_t id, const std::string& stylesheet);
   void reloadTheme(Theme theme);
   Theme currentTheme();
   std::string themeToName(Theme theme);
   Theme nameToTheme(const std::string& name);
   uint32_t getBackgroundColor();
   uint32_t getTerminalBackgroundColor();
   uint32_t getTextColor();
   QPalette getApplicationPalette();
   void subscribeForThemeReloadEvent(ThemeListener*);
   void unsubscribeFromThemeReloadEvent(ThemeListener*);
   QWidget* getParent();
   void setStatusBarNotification(const std::string& notification, uint32_t timeout);
   void setInfoLabelText(const std::string& text);
   void setApplicationTitle(const std::string& title);

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
