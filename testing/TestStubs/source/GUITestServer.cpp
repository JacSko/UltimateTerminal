#include <sstream>

#include "GUITestServer.h"
#include <QtCore/QVector>
#include "TestFrameworkAPI.h"
#include "TestFrameworkCommon.h"
#include "RPCServer.h"
#include "RPCCommon.h"
#include "GUIControllerHelpers.hpp"

const uint32_t COLOR_MASK = 0xFFFFFF;

/* global objects declaration */
std::unique_ptr<RPC::RPCServer> rpc_server;
std::vector<Dialogs::PortSettingDialog::Settings> g_port_settings;
std::vector<Dialogs::TraceFilterSettingDialog::Settings> g_trace_filter_settings;
std::vector<Dialogs::UserButtonDialog::Settings> g_user_button_settings;
GUITestServer::MessageBoxDetails g_messagebox_details;
std::string g_logging_path;

GUITestServer::GUITestServer(Ui::MainWindow* ui):
ui(ui)
{
   UT_Log(TEST_SERVER, INFO, "Starting %s", __func__);

   rpc_server = std::unique_ptr<RPC::RPCServer>(new RPC::RPCServer());
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetButtonState, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetButtonStateRequest(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ButtonClick, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onButtonClickRequest(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ButtonContextMenuClick, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onButtonContextMenuRequested(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetCommand, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetCommandRequest(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetCommand, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetCommandRequest(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetPortLabel, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetPortLabel(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetLineEnding, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetLineEnding(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetLineEnding, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetLineEnding(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetAllLineEndings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetAllLineEndings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTargetPort, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTargetPort(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTargetPort, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetTargetPort(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetAllTargetPorts, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetAllTargetPorts(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceFilterState, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTraceFilterState(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTraceFilter, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetTraceFilter(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetCommandHistory, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetCommandHistory(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTerminalViewContent, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTerminalViewContent(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTerminalViewCount, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTerminalViewCount(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceViewContent, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTraceViewContent(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceViewCount, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTraceViewCount(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetPortSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetPortSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetPortSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetPortSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetTraceFilterSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetTraceFilterSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetTraceFilterSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetTraceFilterSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetUserButtonSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetUserButtonSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetUserButtonSettings, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetUserButtonSettings(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetMessageBox, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetMessageBox(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::ResetMessageBox, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onResetMessageBox(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::SetLoggingPath, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onSetLoggingPath(bytes);});});
   rpc_server->addCommandExecutor((uint8_t)RPC::Command::GetLoggingPath, [&](const std::vector<uint8_t>& bytes)->bool
         {return executeInGUIThread([bytes, this]()->bool{return onGetLoggingPath(bytes);});});

   rpc_server->start(SERVER_PORT);

   connect(this, SIGNAL(executeGUIRequestSignal()), this, SLOT(onExecuteGUIRequestSignal()));

   auto& filters = ui->getTraceFilters();
   for (uint32_t i = 0; i < filters.size(); i++)
   {
      g_trace_filter_settings.push_back({});
   }
   auto& ports = ui->getPorts();
   for (uint8_t i = 0; i < ports.size(); i++)
   {
      g_port_settings.push_back({});
   }
   g_user_button_settings.resize(ui->countUserButtons());
}
GUITestServer::~GUITestServer()
{
   rpc_server->stop();
}

bool GUITestServer::executeInGUIThread(std::function<bool()> function)
{
   constexpr std::chrono::milliseconds EXECUTION_TIMEOUT (100);
   std::unique_lock<std::mutex> lock(m_mutex);

   m_command = std::move(function);
   emit executeGUIRequestSignal();
   return m_cond_var.wait_for(lock, EXECUTION_TIMEOUT, [&](){return !m_command;});
}
void GUITestServer::onExecuteGUIRequestSignal()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   bool result = m_command && m_command();
   UT_Log(TEST_SERVER, INFO, "Command execution %s!", result? "OK" : "NOK");
   m_command = nullptr;
   m_cond_var.notify_all();
}
bool GUITestServer::onGetButtonStateRequest(const std::vector<uint8_t>& data)
{
   RPC::GetButtonStateRequest request = RPC::convert<RPC::GetButtonStateRequest>(data);

   QPushButton* button = getButtonByName(request.button_name);
   UT_Assert(button);

   RPC::GetButtonStateReply reply = {};
   reply.button_name = button->objectName().toStdString();

   QPalette button_palette = button->palette();

   reply.background_color = button_palette.color(QPalette::Button).rgb() & COLOR_MASK;
   reply.font_color = button_palette.color(QPalette::ButtonText).rgb() & COLOR_MASK;
   reply.checkable = button->isCheckable();
   reply.checked = button->isChecked();
   reply.enabled = button->isEnabled();
   reply.text = button->text().toStdString();

   UT_Log(TEST_SERVER, LOW, "%s name %s bg %.8x font %.8x text [%s]", __func__, reply.button_name.c_str(), reply.background_color, reply.font_color, reply.text.c_str());
   return rpc_server->respond<RPC::GetButtonStateReply>(reply);
}
bool GUITestServer::onButtonClickRequest(const std::vector<uint8_t>& data)
{
   RPC::ButtonClickRequest request = RPC::convert<RPC::ButtonClickRequest>(data);
   RPC::ButtonClickReply reply = {};
   reply.clicked = false;

   QPushButton* button = getButtonByName(request.button_name);
   if (button)
   {
      button->click();
      reply.clicked = true;
   }

   UT_Log(TEST_SERVER, LOW, "%s clicked %u", __func__, reply.clicked);
   return rpc_server->respond<RPC::ButtonClickReply>(reply);
}
bool GUITestServer::onButtonContextMenuRequested(const std::vector<uint8_t>& data)
{
   RPC::ButtonClickRequest request = RPC::convert<RPC::ButtonClickRequest>(data);
   RPC::ButtonClickReply reply = {};
   reply.clicked = false;

   QPushButton* button = getButtonByName(request.button_name);
   if (button)
   {
      reinterpret_cast<QWidget*>(button)->customContextMenuRequested({});
      reply.clicked = true;
   }

   UT_Log(TEST_SERVER, LOW, "%s clicked %u", __func__, reply.clicked);
   return rpc_server->respond<RPC::ButtonClickReply>(reply);
}
bool GUITestServer::onSetCommandRequest(const std::vector<uint8_t>& data)
{
   RPC::SetCommandRequest request = RPC::convert<RPC::SetCommandRequest>(data);
   RPC::SetCommandReply reply = {};
   reply.reply = true;
   UT_Log(TEST_SERVER, LOW, "%s %s", __func__, request.command.c_str());

   ui->textEdit->setCurrentText(QString(request.command.c_str()));

   return rpc_server->respond<RPC::SetCommandReply>(reply);
}
bool GUITestServer::onGetCommandRequest(const std::vector<uint8_t>&)
{
   RPC::GetCommandReply reply = {};
   reply.command = ui->textEdit->currentText().toStdString();

   UT_Log(TEST_SERVER, LOW, "%s %s", __func__, reply.command.c_str());
   return rpc_server->respond<RPC::GetCommandReply>(reply);
}
bool GUITestServer::onGetPortLabel(const std::vector<uint8_t>& data)
{
   RPC::GetPortLabelRequest request = RPC::convert<RPC::GetPortLabelRequest>(data);
   RPC::GetPortLabelReply reply = {};

   auto ports = ui->getPorts();

   UT_Assert(request.id < ports.size());
   reply.id = request.id;
   reply.stylesheet = ports[request.id].label->styleSheet().toStdString();

   reply.text = ports[request.id].label->text().toStdString();
   UT_Log(TEST_SERVER, LOW, "%s id %u %s %s", __func__, reply.id, reply.text.c_str(), reply.stylesheet.c_str());
   return rpc_server->respond<RPC::GetPortLabelReply>(reply);
}
bool GUITestServer::onGetLineEnding(const std::vector<uint8_t>&)
{
   RPC::GetLineEndingReply reply = {};
   reply.lineending = ui->lineEndingComboBox->currentText().toStdString();
   UT_Log(TEST_SERVER, LOW, "%s %s", __func__, reply.lineending.c_str());
   return rpc_server->respond<RPC::GetLineEndingReply>(reply);
}
bool GUITestServer::onGetAllLineEndings(const std::vector<uint8_t>&)
{
   RPC::GetAllLineEndingsReply reply = {};

   for (int i = 0; i < ui->lineEndingComboBox->count(); i++)
   {
      reply.lineendings.push_back(ui->lineEndingComboBox->itemText(i).toStdString());
   }

   UT_Log(TEST_SERVER, LOW, "%s size %u", __func__, (uint32_t)reply.lineendings.size());
   return rpc_server->respond<RPC::GetAllLineEndingsReply>(reply);
}
bool GUITestServer::onSetLineEnding(const std::vector<uint8_t>& data)
{
   RPC::SetLineEndingRequest request = RPC::convert<RPC::SetLineEndingRequest>(data);
   RPC::SetLineEndingReply reply = {};

   int index = ui->lineEndingComboBox->findText(QString(request.lineending.c_str()));
   if ( index != -1 )
   {
      ui->lineEndingComboBox->setCurrentIndex(index);
      reply.result = true;
   }

   UT_Log(TEST_SERVER, LOW, "%s [%s] found %u ", __func__, request.lineending.c_str(), reply.result);
   return rpc_server->respond<RPC::SetLineEndingReply>(reply);
}
bool GUITestServer::onGetTargetPort(const std::vector<uint8_t>&)
{
   RPC::GetTargetPortReply reply = {};

   reply.port_name = ui->portComboBox->currentText().toStdString();
   UT_Log(TEST_SERVER, LOW, "%s %s", __func__, reply.port_name.c_str());

   return rpc_server->respond<RPC::GetTargetPortReply>(reply);
}
bool GUITestServer::onGetAllTargetPorts(const std::vector<uint8_t>&)
{
   RPC::GetAllTargetPortsReply reply = {};

   for (int i = 0; i < ui->portComboBox->count(); i++)
   {
      reply.port_names.push_back(ui->portComboBox->itemText(i).toStdString());
   }

   return rpc_server->respond<RPC::GetAllTargetPortsReply>(reply);
}
bool GUITestServer::onSetTargetPort(const std::vector<uint8_t>& data)
{
   RPC::SetTargetPortRequest request = RPC::convert<RPC::SetTargetPortRequest>(data);
   RPC::SetTargetPortReply reply = {};

   int index = ui->portComboBox->findText(QString(request.port_name.c_str()));
   if ( index != -1 )
   {
      ui->portComboBox->setCurrentIndex(index);
      reply.result = true;
   }

   UT_Log(TEST_SERVER, LOW, "%s [%s] found %u ", __func__, request.port_name.c_str(), reply.result);
   return rpc_server->respond<RPC::SetTargetPortReply>(reply);
}
bool GUITestServer::onGetTraceFilterState(const std::vector<uint8_t>& data)
{
   RPC::GetTraceFilterStateRequest request = RPC::convert<RPC::GetTraceFilterStateRequest>(data);
   auto filter = getTraceFilterByName(request.filter_name);
   UT_Assert(filter.button && filter.line_edit);

   RPC::GetTraceFilterStateReply reply = {};
   reply.filter_name = request.filter_name;

   reply.enabled = filter.button->isEnabled() && filter.line_edit->isEnabled();
   Stylesheet ss (filter.line_edit->styleSheet().toStdString());
   reply.background_color = ss.getColor(Stylesheet::Item::BACKGROUND_COLOR);
   reply.font_color = ss.getColor(Stylesheet::Item::COLOR);
   reply.text = filter.line_edit->text().toStdString();

   UT_Log(TEST_SERVER, LOW, "%s name %s bg %u fb %u text %s enabled %u", __func__, request.filter_name.c_str(), reply.background_color, reply.font_color, reply.text.c_str(), reply.enabled);
   return rpc_server->respond<RPC::GetTraceFilterStateReply>(reply);
}
bool GUITestServer::onSetTraceFilter(const std::vector<uint8_t>& data)
{
   RPC::SetTraceFilterRequest request = RPC::convert<RPC::SetTraceFilterRequest>(data);
   auto filter = getTraceFilterByName(request.filter_name);
   UT_Assert(filter.button && filter.line_edit);

   RPC::SetTraceFilterReply reply = {};

   reply.result = filter.button->isEnabled() && filter.line_edit->isEnabled();
   if (reply.result)
   {
      filter.line_edit->setText(request.text.c_str());
   }

   UT_Log(TEST_SERVER, LOW, "%s name %s text %s result %u", __func__, request.filter_name.c_str(), request.text.c_str(), reply.result);
   return rpc_server->respond<RPC::SetTraceFilterReply>(reply);
}
bool GUITestServer::onGetCommandHistory(const std::vector<uint8_t>&)
{
   RPC::GetCommandHistoryReply reply = {};

   for (int i = 0; i < ui->textEdit->count(); i++)
   {
      reply.history.push_back(ui->textEdit->itemText(i).toStdString());
   }

   return rpc_server->respond<RPC::GetCommandHistoryReply>(reply);
}
bool GUITestServer::onGetTerminalViewContent(const std::vector<uint8_t>&)
{
   RPC::GetTerminalViewContentReply reply = {};

   std::stringstream stream (ui->terminalView->toPlainText().toStdString());
   std::string line;

   while(std::getline(stream, line, '\n'))
   {
      UT_Log(TEST_SERVER, LOW, "%s line %s", __func__, line.c_str());
      reply.content.push_back({line, 0, 0});
   }

   UT_Log(TEST_SERVER, LOW, "%s size %u", __func__, reply.content.size());
   return rpc_server->respond<RPC::GetTerminalViewContentReply>(reply);
}
bool GUITestServer::onGetTerminalViewCount(const std::vector<uint8_t>&)
{
   RPC::GetTerminalViewCountReply reply = {};

   std::stringstream stream (ui->terminalView->toPlainText().toStdString());
   std::string line;

   while(std::getline(stream, line, '\n'))
   {
      UT_Log(TEST_SERVER, LOW, "%s line %s", __func__, line.c_str());
      reply.count++;
   }

   UT_Log(TEST_SERVER, LOW, "%s count %u", __func__, reply.count);
   return rpc_server->respond<RPC::GetTerminalViewCountReply>(reply);
}
bool GUITestServer::onGetTraceViewContent(const std::vector<uint8_t>&)
{
   RPC::GetTraceViewContentReply reply = {};

   for (int i = 0; i < ui->traceView->count(); i++)
   {
      QListWidgetItem* item = ui->traceView->item(i);
      reply.content.push_back({item->text().toStdString(), item->backgroundColor().rgb() & COLOR_MASK, item->textColor().rgb() & COLOR_MASK});
   }


   UT_Log(TEST_SERVER, LOW, "%s size %u", __func__, reply.content.size());
   return rpc_server->respond<RPC::GetTraceViewContentReply>(reply);
}
bool GUITestServer::onGetTraceViewCount(const std::vector<uint8_t>&)
{
   RPC::GetTraceViewCountReply reply = {};

   for (int i = 0; i < ui->traceView->count(); i++)
   {
      reply.count++;
   }

   UT_Log(TEST_SERVER, LOW, "%s count %u", __func__, reply.count);
   return rpc_server->respond<RPC::GetTraceViewCountReply>(reply);
}
bool GUITestServer::onSetPortSettings(const std::vector<uint8_t>& data)
{
   RPC::SetPortSettingsRequest request = RPC::convert<RPC::SetPortSettingsRequest>(data);
   uint32_t id = request.port_id;
   UT_Assert(id < g_port_settings.size());

   RPC::SetPortSettingsReply reply = {};
   reply.result = true;
   g_port_settings[id] = request.settings;

   UT_Log(TEST_SERVER, LOW, "%s result %u id %u settings %s", __func__, reply.result, id, request.settings.shortSettingsString().c_str());
   return rpc_server->respond<RPC::SetPortSettingsReply>(reply);
}
bool GUITestServer::onGetPortSettings(const std::vector<uint8_t>& data)
{
   RPC::GetPortSettingsRequest request = RPC::convert<RPC::GetPortSettingsRequest>(data);
   uint32_t id = request.port_id;
   UT_Assert(id < g_port_settings.size());

   RPC::GetPortSettingsReply reply = {};
   reply.port_id = request.port_id;
   reply.settings = g_port_settings[id];

   UT_Log(TEST_SERVER, LOW, "%s id %u settings %s", __func__, id, reply.settings.shortSettingsString().c_str());
   return rpc_server->respond<RPC::GetPortSettingsReply>(reply);
}
bool GUITestServer::onSetTraceFilterSettings(const std::vector<uint8_t>& data)
{
   RPC::SetTraceFilterSettingsRequest request = RPC::convert<RPC::SetTraceFilterSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_trace_filter_settings.size());

   RPC::SetTraceFilterSettingsReply reply = {};
   reply.result = true;
   g_trace_filter_settings[id] = request.settings;

   UT_Log(TEST_SERVER, LOW, "%s result %u id %u regex %s bg %.6x fg %.6x", __func__, reply.result, id, request.settings.regex.c_str(), request.settings.background, request.settings.font);
   return rpc_server->respond<RPC::SetTraceFilterSettingsReply>(reply);
}
bool GUITestServer::onGetTraceFilterSettings(const std::vector<uint8_t>& data)
{
   RPC::GetTraceFilterSettingsRequest request = RPC::convert<RPC::GetTraceFilterSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_trace_filter_settings.size());

   RPC::GetTraceFilterSettingsReply reply = {};
   reply.id = request.id;
   reply.settings = g_trace_filter_settings[id];

   UT_Log(TEST_SERVER, LOW, "%s id %u regex %s bg %.6x fg %.6x", __func__, reply.id, reply.settings.regex.c_str(), reply.settings.background, reply.settings.font);
   return rpc_server->respond<RPC::GetTraceFilterSettingsReply>(reply);
}
bool GUITestServer::onSetUserButtonSettings(const std::vector<uint8_t>& data)
{
   RPC::SetUserButtonSettingsRequest request = RPC::convert<RPC::SetUserButtonSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_user_button_settings.size());

   RPC::SetUserButtonSettingsReply reply = {};
   reply.result = true;
   g_user_button_settings[id] = request.settings;

   UT_Log(TEST_SERVER, LOW, "%s result %u id %u name %s", __func__, reply.result, id, request.settings.button_name.c_str());
   return rpc_server->respond<RPC::SetUserButtonSettingsReply>(reply);
}
bool GUITestServer::onGetUserButtonSettings(const std::vector<uint8_t>& data)
{
   RPC::GetUserButtonSettingsRequest request = RPC::convert<RPC::GetUserButtonSettingsRequest>(data);
   uint32_t id = request.id;
   UT_Assert(id < g_user_button_settings.size());

   RPC::GetUserButtonSettingsReply reply = {};
   reply.id = request.id;
   reply.settings = g_user_button_settings[id];

   UT_Log(TEST_SERVER, LOW, "%s id %u name %s", __func__, id, reply.settings.button_name.c_str());
   return rpc_server->respond<RPC::GetUserButtonSettingsReply>(reply);
}
bool GUITestServer::onGetMessageBox(const std::vector<uint8_t>&)
{
   RPC::GetMessageBoxReply reply = {};
   reply.text = g_messagebox_details.text;
   reply.icon = g_messagebox_details.icon;
   reply.title = g_messagebox_details.title;

   UT_Log(TEST_SERVER, LOW, "%s", __func__);
   return rpc_server->respond<RPC::GetMessageBoxReply>(reply);
}
bool GUITestServer::onResetMessageBox(const std::vector<uint8_t>&)
{
   RPC::ResetMessageBoxReply reply = {};
   reply.result = g_messagebox_details.reported;
   g_messagebox_details = {};

   UT_Log(TEST_SERVER, LOW, "%s", __func__);
   return rpc_server->respond<RPC::ResetMessageBoxReply>(reply);
}
bool GUITestServer::onSetLoggingPath(const std::vector<uint8_t>& data)
{
   RPC::SetLoggingPathRequest request = RPC::convert<RPC::SetLoggingPathRequest>(data);
   g_logging_path = request.path;
   RPC::SetLoggingPathReply reply = {};
   reply.result = true;

   UT_Log(TEST_SERVER, LOW, "%s path %s", __func__, g_logging_path.c_str());
   return rpc_server->respond<RPC::SetLoggingPathReply>(reply);
}
bool GUITestServer::onGetLoggingPath(const std::vector<uint8_t>&)
{
   RPC::GetLoggingPathReply reply = {};
   reply.path = g_logging_path;
   UT_Log(TEST_SERVER, LOW, "%s path %s", __func__, g_logging_path.c_str());
   return rpc_server->respond<RPC::GetLoggingPathReply>(reply);
}
void GUITestServer::onCurrentPortSelectionChanged(int)
{
}
QPushButton* GUITestServer::getButtonByName(const std::string& name)
{
   QPushButton* result = nullptr;
   for (auto button : ui->getButtons())
   {
      if (button->objectName().toStdString() == name)
      {
         result = button;
         break;
      }
   }
   return result;
}
Ui_MainWindow::TraceFilterItem GUITestServer::getTraceFilterByName(const std::string& name)
{
   Ui_MainWindow::TraceFilterItem result = {nullptr, nullptr};
   auto filters = ui->getTraceFilters();

   for (auto filter : filters)
   {
      if (filter.button->objectName().toStdString() == name ||
          filter.line_edit->objectName().toStdString() == name)
      {
         result = filter;
         break;
      }
   }
   return result;
}
std::string GUITestServer::onLoggingPathDialogShow(QWidget*, const std::string& current_path, bool)
{
   std::string result = g_logging_path;
   g_logging_path = current_path;
   UT_Log(TEST_SERVER, LOW, "%s current %s new %s", __func__, current_path.c_str(), result.c_str());
   return result;
}
void GUITestServer::onMessageBoxShow(Dialogs::MessageDialog::Icon icon, const std::string& window_title, const std::string& text, QPalette)
{
   g_messagebox_details = {true, window_title, icon, text};
}
std::optional<bool> GUITestServer::onPortSettingsDialogShow(QWidget*, const Dialogs::PortSettingDialog::Settings& current_settings, Dialogs::PortSettingDialog::Settings& out_settings, bool)
{
   UT_Assert(current_settings.port_id < g_port_settings.size());
   out_settings = g_port_settings[current_settings.port_id];
   UT_Log(TEST_SERVER, LOW, "%s %u[%s]", __func__, current_settings.port_id, current_settings.port_name.c_str());
   return true;
}
std::optional<bool> GUITestServer::onTraceFilterSettingDialogShow(QWidget*, const Dialogs::TraceFilterSettingDialog::Settings& current_settings, Dialogs::TraceFilterSettingDialog::Settings& out_settings, bool)
{
   UT_Assert(current_settings.id < g_trace_filter_settings.size());
   out_settings = g_trace_filter_settings[current_settings.id];
   UT_Log(TEST_SERVER, LOW, "%s id %u", __func__, current_settings.id);
   return true;
}
std::optional<bool> GUITestServer::onUserButtonSettingsDialogShow(QWidget*, const Dialogs::UserButtonDialog::Settings& current_settings, Dialogs::UserButtonDialog::Settings& out_settings, bool)
{
   UT_Assert(current_settings.id < g_user_button_settings.size());
   out_settings = g_user_button_settings[current_settings.id];
   UT_Log(TEST_SERVER, LOW, "%s id %u", __func__, current_settings.id);
   return true;
}
