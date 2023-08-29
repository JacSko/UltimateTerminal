#pragma once
#include <vector>
#include <utility>
#include <mutex>
#include <condition_variable>

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "MessageDialog.h"
#include "PortSettingDialog.h"
#include "TraceFilterSettingDialog.h"
#include "UserButtonDialog.h"

class GUITestServer : public QObject
{
   Q_OBJECT
public:
   GUITestServer(Ui::MainWindow* ui, QMainWindow& main_window);
   ~GUITestServer();

   struct MessageBoxDetails
   {
      bool reported;
      std::string title;
      Dialogs::MessageDialog::Icon icon;
      std::string text;
   };
   Ui::MainWindow *ui;
   std::function<bool()> m_command;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;

   bool executeInGUIThread(std::function<bool()> function);
   QPushButton* getButtonByName(const std::string& name);

signals:
   void executeGUIRequestSignal();
   void shutdownRequest();

public slots:
   void onExecuteGUIRequestSignal();
   void onShutdownRequest();

   static std::string onLoggingPathDialogShow(QWidget* parent, const std::string& current_path, bool allow_edit);
   static void onMessageBoxShow(Dialogs::MessageDialog::Icon icon, const std::string& window_title, const std::string& text, QPalette palette);
   static std::optional<bool> onPortSettingsDialogShow(QWidget* parent, const Dialogs::PortSettingDialog::Settings& current_settings, Dialogs::PortSettingDialog::Settings& out_settings, bool allow_edit);
   static std::optional<bool> onTraceFilterSettingDialogShow(QWidget* parent, const Dialogs::TraceFilterSettingDialog::Settings& current_settings, Dialogs::TraceFilterSettingDialog::Settings& out_settings, bool allow_edit);
   static std::optional<bool> onUserButtonSettingsDialogShow(QWidget* parent, const Dialogs::UserButtonDialog::Settings& current_settings, Dialogs::UserButtonDialog::Settings& out_settings, bool allow_edit);

private:
   bool onButtonClickRequest(const std::vector<uint8_t>& data);
   bool onButtonContextMenuRequested(const std::vector<uint8_t>& data);
   bool onSetCommandRequest(const std::vector<uint8_t>& data);
   bool onGetCommandRequest(const std::vector<uint8_t>& data);
   bool onGetPortLabel(const std::vector<uint8_t>& data);
   bool onGetLineEnding(const std::vector<uint8_t>& data);
   bool onGetAllLineEndings(const std::vector<uint8_t>& data);
   bool onSetLineEnding(const std::vector<uint8_t>& data);
   bool onGetTargetPort(const std::vector<uint8_t>& data);
   bool onGetAllTargetPorts(const std::vector<uint8_t>& data);
   bool onSetTargetPort(const std::vector<uint8_t>& data);
   bool onGetTraceFilterState(const std::vector<uint8_t>& data);
   bool onSetTraceFilter(const std::vector<uint8_t>& data);
   bool onGetCommandHistory(const std::vector<uint8_t>& data);
   bool onGetTerminalViewContent(const std::vector<uint8_t>& data);
   bool onGetTerminalViewCount(const std::vector<uint8_t>& data);
   bool onGetTraceViewContent(const std::vector<uint8_t>& data);
   bool onGetTraceViewCount(const std::vector<uint8_t>& data);
   bool onSetPortSettings(const std::vector<uint8_t>& data);
   bool onGetPortSettings(const std::vector<uint8_t>& data);
   bool onSetTraceFilterSettings(const std::vector<uint8_t>& data);
   bool onGetTraceFilterSettings(const std::vector<uint8_t>& data);
   bool onSetUserButtonSettings(const std::vector<uint8_t>& data);
   bool onGetUserButtonSettings(const std::vector<uint8_t>& data);
   bool onGetMessageBox(const std::vector<uint8_t>& data);
   bool onResetMessageBox(const std::vector<uint8_t>& data);
   bool onSetLoggingPath(const std::vector<uint8_t>& data);
   bool onGetLoggingPath(const std::vector<uint8_t>& data);
   void onCurrentPortSelectionChanged(int);
   bool onGetButtonStateRequest(const std::vector<uint8_t>&);
   bool onGetTerminalScrollPosition(const std::vector<uint8_t>&);
   bool onGetTraceViewScrollPosition(const std::vector<uint8_t>&);
   bool onSetTerminalScrollPosition(const std::vector<uint8_t>&);
   bool onSetTraceViewScrollPosition(const std::vector<uint8_t>&);
   bool onCloseApplication(const std::vector<uint8_t>&);

   Ui_MainWindow::TraceFilterItem getTraceFilterByName(const std::string& name);
   QMainWindow& m_mainWindow;
};
