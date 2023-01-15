#pragma once

#include <mutex>
#include <condition_variable>

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include "IGUIController.h"
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
   void execute(Ui::MainWindow* ui) override
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
   void execute(Ui::MainWindow* ui) override
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
   void execute(Ui::MainWindow* ui) override
   {
      std::unique_lock<std::mutex> lock(mutex);
      auto filters = ui->getTraceFilters();
      UT_Assert(filters.size() > id);
      trace_filter = filters[id].line_edit->text().toStdString();
      ready = true;
   }
};

class GUIController : public IGUIController,
                      public QMainWindow
{
   Q_OBJECT
public:
   GUIController(QWidget *parent);
   ~GUIController();
private:

   struct ButtonEventItem
   {
      uint32_t id;
      ButtonEvent event;
      IGUIController::ButtonEventListener* listener;
   };

   struct LogViewStatus
   {
      uint32_t item_count;
      bool scrolling_enabled;
   };

   Ui::MainWindow *ui;
   IGUIController::Theme m_current_theme;
   LogViewStatus m_terminal_view_status;
   LogViewStatus m_trace_view_status;
   std::function<bool(const std::string&)> m_active_port_listener;

   std::mutex m_command_mutex;
   CommandExecutor* m_current_command;

   std::mutex m_theme_listeners_mutex;
   std::vector<ThemeListener*> m_theme_listeners;
   std::mutex m_button_listeners_mutex;
   std::vector<ButtonEventItem> m_button_listeners;

   uint32_t getButtonID(const std::string& name) override;
   uint32_t getLabelID(const std::string& name) override;
   void subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*) override;
   void unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener*) override;
   void setButtonBackgroundColor(uint32_t button_id, uint32_t color) override;
   void setButtonFontColor(uint32_t button_id, uint32_t color) override;
   void setButtonCheckable(uint32_t button_id, bool checkable) override;
   void setButtonChecked(uint32_t button_id, bool checked) override;
   void setButtonEnabled(uint32_t button_id, bool enabled) override;
   void setButtonText(uint32_t button_id, const std::string& text) override;
   void clearTerminalView() override;
   void clearTraceView() override;
   void addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color) override;
   void addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color) override;
   uint32_t countTerminalItems() override;
   uint32_t countTraceItems() override;
   void setTerminalScrollingEnabled(bool enabled) override;
   void setTraceScrollingEnabled(bool enabled) override;
   void subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback) override;
   void registerPortOpened(const std::string& port_name) override;
   void registerPortClosed(const std::string& port_name) override;
   void setCommandsHistory(const std::vector<std::string>& history) override;
   void addCommandToHistory(const std::string& history) override;
   std::string getCurrentCommand() override;
   std::string getCurrentLineEnding() override;
   void addLineEnding(const std::string& ending) override;
   void setCurrentLineEnding(const std::string& ending) override;
   uint32_t getTraceFilterID(const std::string& name) override;
   void setTraceFilter(uint8_t id, const std::string& filter) override;
   std::string getTraceFilter(uint8_t id) override;
   void setTraceFilterEnabled(uint8_t id, bool enabled) override;
   void setTraceFilterBackgroundColor(uint32_t id, uint32_t color) override;
   void setTraceFilterFontColor(uint32_t id, uint32_t color) override;
   void setPortLabelText(uint8_t id, const std::string& description) override;
   void setPortLabelStylesheet(uint8_t id, const std::string& stylesheet) override;
   void reloadTheme(Theme theme) override;
   Theme currentTheme() override;
   std::string themeToName(Theme theme) override;
   Theme nameToTheme(const std::string& name) override;
   uint32_t getBackgroundColor() override;
   uint32_t getTerminalBackgroundColor() override;
   uint32_t getTextColor() override;
   QPalette getApplicationPalette() override;
   void subscribeForThemeReloadEvent(ThemeListener*) override;
   void unsubscribeFromThemeReloadEvent(ThemeListener*) override;

   bool executeGUIRequest(CommandExecutor* request);

signals:
   void setButtonBackgroundColorSignal(uint32_t id, uint32_t color);
   void setButtonFontColorSignal(uint32_t id, uint32_t color);
   void setButtonCheckableSignal(uint32_t id, bool checkable);
   void setButtonCheckedSignal(uint32_t id, bool checked);
   void setButtonEnabledSignal(uint32_t id, bool enabled);
   void setButtonTextSignal(uint32_t id, QString text);
   void clearTerminalViewSignal();
   void clearTraceViewSignal();
   void addToTerminalViewSignal(const std::string& text, uint32_t background_color, uint32_t font_color);
   void addToTerminalViewSignal(QString text, uint32_t background_color, uint32_t font_color);
   void addToTraceViewSignal(QString text, uint32_t background_color, uint32_t font_color);
   void setTerminalScrollingEnabledSignal(bool enabled);
   void setTraceScrollingEnabledSignal(bool enabled);
   void registerPortOpenedSignal(QString name);
   void registerPortClosedSignal(QString name);
   void setCommandHistorySignal(QVector<QString> history);
   void addCommandToHistorySignal(QString command);
   void guiRequestSignal();
   void addLineEndingSignal(QString ending);
   void setCurrentLineEndingSignal(QString ending);
   void setTraceFilterSignal(uint8_t id, QString filter);
   void setTraceFilterEnabledSignal(uint8_t id, bool enabled);
   void setTraceFilterBackgroundColorSignal(uint32_t id, uint32_t color);
   void setTraceFilterFontColorSignal(uint32_t id, uint32_t color);
   void setPortLabelTextSignal(uint8_t id, QString description);
   void setPortLabelStylesheetSignal(uint8_t id, QString stylesheet);
   void reloadThemeSignal(uint8_t);

public slots:
   void onButtonClicked();
   void onButtonContextMenuRequested();
   void onCurrentPortSelectionChanged(int);
   void onPortSwitchRequest();
   void onButtonBackgroundColorSignal(uint32_t id, uint32_t color);
   void onButtonFontColorSignal(uint32_t id, uint32_t color);
   void onButtonCheckableSignal(uint32_t id, bool checkable);
   void onButtonCheckedSignal(uint32_t id, bool checked);
   void onButtonEnabledSignal(uint32_t id, bool enabled);
   void onButtonTextSignal(uint32_t id, QString text);
   void onClearTerminalViewSignal();
   void onClearTraceViewSignal();
   void onAddToTerminalViewSignal(QString text, uint32_t background_color, uint32_t font_color);
   void onAddToTraceViewSignal(QString text, uint32_t background_color, uint32_t font_color);
   void onSetTerminalScrollingEnabledSignal(bool enabled);
   void onSetTraceScrollingEnabledSignal(bool enabled);
   void onRegisterPortOpenedSignal(QString name);
   void onRegisterPortClosedSignal(QString name);
   void onSetCommandHistorySignal(QVector<QString> history);
   void onAddCommandToHistorySignal(QString command);
   void onGuiRequestSignal();
   void onAddLineEndingSignal(QString ending);
   void onSetCurrentLineEndingSignal(QString ending);
   void onSetTraceFilterSignal(uint8_t id, QString filter);
   void onSetTraceFilterEnabledSignal(uint8_t id, bool enabled);
   void onSetTraceFilterBackgroundColorSignal(uint32_t id, uint32_t color);
   void onSetTraceFilterFontColorSignal(uint32_t id, uint32_t color);
   void onSetPortLabelTextSignal(uint8_t id, QString description);
   void onSetPortLabelStylesheetSignal(uint8_t id, QString stylesheet);
   void onReloadThemeSignal(uint8_t);

};
