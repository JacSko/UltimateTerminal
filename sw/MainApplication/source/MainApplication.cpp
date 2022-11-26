#include <QtWidgets/QMessageBox>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "MainApplication.h"
#include "SettingsHandler.h"
#include "Settings.h"
#include "LoggerEngine.h"
#include "Serialize.hpp"
#include "ApplicationSettingsDialog.h"

constexpr uint32_t TRACE_MARKER_COLOR = 0xFF0000;
constexpr uint8_t PORT_HANDLERS_COUNT = 5;


namespace system_call
{
__attribute__((weak)) std::string getExecutablePath()
{
   constexpr uint32_t MAX_PATH_LENGTH = 4096;
   char result[MAX_PATH_LENGTH];
   ssize_t count = readlink("/proc/self/exe", result, MAX_PATH_LENGTH);
   const char *path;
   if (count != -1) {
       path = dirname(result);
   }
   return std::string(path);
}
}

#undef APPLICATION_THEME
#define APPLICATION_THEME(name) #name,
std::array<std::string, (uint32_t)IThemeController::Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME


MainApplication::MainApplication(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_timers(Utilities::ITimersFactory::create()),
m_file_logger(IFileLogger::create()),
m_marker_index(0),
m_scrolling_active(false),
m_trace_scrolling_active(false)
{
   Persistence::PersistenceListener::setName("MAIN_APPLICATION");
   m_persistence.addListener(*this);
   Settings::SettingsHandler::create();
   Settings::SettingsHandler::get()->start(system_call::getExecutablePath() + '/' + CONFIG_FILE);
   LoggerEngine::get()->startFrontends();
   ui->setupUi(this);

   if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Dark_Theme_ID))
   {
      ui->loadTheme(IThemeController::Theme::DARK);
      m_current_theme = IThemeController::Theme::DARK;
   }
   else if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Default_Theme_ID))
   {
      ui->loadTheme(IThemeController::Theme::DEFAULT);
      m_current_theme = IThemeController::Theme::DEFAULT;
   }
   this->setWindowTitle("UltimateTerminal");

   UT_Log(MAIN, ALWAYS, "UltimateTerminal version %s", std::string(APPLICATION_VERSION).c_str());
   ui->infoLabel->setText(QString().asprintf("UltimateTerminal v%s", std::string(APPLICATION_VERSION).c_str()));

   Settings::SettingsHandler::get()->printSettings();
   m_timers->start();

   setScrolling(true);
   setTraceScrolling(true);
   setButtonState(ui->loggingButton, false);

   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(ui->portButton_1, ui->portLabel_1, ui->port1ButtonShortcut, *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(ui->portButton_2, ui->portLabel_2, ui->port2ButtonShortcut, *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(ui->portButton_3, ui->portLabel_3, ui->port3ButtonShortcut, *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(ui->portButton_4, ui->portLabel_4, ui->port4ButtonShortcut, *m_timers, this, this, m_persistence)));
   m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
       new GUI::PortHandler(ui->portButton_5, ui->portLabel_5, ui->port5ButtonShortcut, *m_timers, this, this, m_persistence)));

   /* create handlers for user buttons */
   auto buttons = ui->getUserButtons();
   for (QPushButton* button : buttons)
   {
      m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(button, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
   }

   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_1, ui->traceFilterButton_1, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_2, ui->traceFilterButton_2, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_3, ui->traceFilterButton_3, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_4, ui->traceFilterButton_4, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_5, ui->traceFilterButton_5, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_6, ui->traceFilterButton_6, m_persistence)));
   m_trace_filter_handlers.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(this, ui->traceFilter_7, ui->traceFilterButton_7, m_persistence)));

   ui->lineEndingComboBox->addItem("\\r\\n");
   ui->lineEndingComboBox->addItem("\\n");
   ui->lineEndingComboBox->addItem("EMPTY");

   m_file_logging_path = system_call::getExecutablePath();

   for (auto& handler : m_user_button_handlers)
   {
      handler->startThread();
   }
   if (!m_persistence.restore(PERSISTENCE_PATH))
   {
      UT_Log(MAIN, ERROR, "Cannot restore persistence!");
   }
   connectSignalsToSlots();
}
MainApplication::~MainApplication()
{
   UT_Log(MAIN, INFO, "Closing application!");
   if (!m_persistence.save(PERSISTENCE_PATH))
   {
      UT_Log(MAIN, ERROR, "Cannot write persistence!");
   }
   m_timers->stop();
   m_persistence.removeListener(*this);
   Settings::SettingsHandler::get()->stop();
   Settings::SettingsHandler::destroy();
   delete ui;
}
void MainApplication::onPortHandlerEvent(const GUI::PortHandlerEvent& event)
{
   if (event.event == GUI::Event::NEW_DATA)
   {
      addToTerminal(event.name, std::string(event.data.begin(), event.data.end()), event.background_color, event.font_color);
   }
   else if (event.event == GUI::Event::CONNECTED)
   {
      m_port_id_name_map[event.port_id] = event.name;
      ui->portComboBox->addItem(QString(event.name.c_str()));
      UT_Log(MAIN, INFO, "Port opened: %u [%s]", event.port_id, event.name.c_str());
   }
   else if (event.event == GUI::Event::DISCONNECTED)
   {
      int index = ui->portComboBox->findText(event.name.c_str());
      if (index != -1)
      {
         ui->portComboBox->removeItem(index);
      }
      m_port_id_name_map[event.port_id] = "";
      UT_Log(MAIN, INFO, "Port closed: %u [%s]", event.port_id, event.name.c_str());
   }
}
void MainApplication::addToTerminal(const std::string& port_name, const std::string& data, uint32_t background_color, uint32_t font_color)
{
   auto currentTime = std::chrono::system_clock::now();
   auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);
   QString new_line = QString().asprintf("[%s]<%u-%.2u-%u %u:%.2u:%.2u:%.3lu>: %s", port_name.c_str(),
                                   ts->tm_mday, ts->tm_mon, ts->tm_year + 1900,
                                   ts->tm_hour, ts->tm_min, ts->tm_sec, millis,
                                   data.c_str());

   m_file_logger->putLog(new_line.toStdString());

   // remove trailing newline
   if (new_line.back() == '\n')
   {
      new_line.chop(1);
   }

   if((uint32_t)ui->terminalView->count() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in main terminal, cleaning trace view");
      ui->terminalView->clear();
   }

   if((uint32_t)ui->traceView->count() >= SETTING_GET_U32(MainApplication_maxTerminalTraces))
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in trace view, cleaning trace view");
      ui->traceView->clear();
   }

   QListWidgetItem* item = new QListWidgetItem();
   item->setText(new_line);
   item->setBackground(QColor(background_color));
   item->setForeground(QColor(font_color));
   ui->terminalView->addItem(item);

   for (auto& filter : m_trace_filter_handlers)
   {
      std::optional<Dialogs::TraceFilterSettingDialog::Settings> color_set = filter->tryMatch(data);
      if (color_set.has_value())
      {
         QListWidgetItem* item = new QListWidgetItem();
         item->setText(new_line);
         item->setBackground(QColor(color_set.value().background));
         item->setForeground(QColor(color_set.value().font));
         ui->traceView->addItem(item);
      }
   }

   if (m_scrolling_active)
   {
      ui->terminalView->scrollToBottom();
   }

   if (m_trace_scrolling_active)
   {
      ui->traceView->scrollToBottom();
   }

}
void MainApplication::connectSignalsToSlots()
{
   connect(ui->markerButton, SIGNAL(clicked()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->markerButtonShortcut, SIGNAL(activated()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonClicked()));
   connect(ui->clearButtonShortcut, SIGNAL(activated()), this, SLOT(onClearButtonClicked()));
   connect(ui->traceClearButton, SIGNAL(clicked()), this, SLOT(onTraceClearButtonClicked()));
   connect(ui->traceClearButtonShortcut, SIGNAL(activated()), this, SLOT(onTraceClearButtonClicked()));
   connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
   connect(ui->textEdit->lineEdit(), SIGNAL(returnPressed()), this, SLOT(onSendButtonClicked()));
   ui->loggingButton->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(ui->loggingButton, SIGNAL(clicked()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->loggingButton, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onLoggingButtonContextMenuRequested()));
   connect(ui->loggingButtonShortcut, SIGNAL(activated()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->scrollButton, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
   connect(ui->traceScrollButton, SIGNAL(clicked()), this, SLOT(onTraceScrollButtonClicked()));
   connect(ui->portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentPortSelectionChanged(int)));
   connect(ui->switchSendPortShortcut, SIGNAL(activated()), this, SLOT(onPortSwitchRequest()));
   connect(ui->settingsButton, SIGNAL(clicked()), this, SLOT(onSettingsButtonClicked()));
}
void MainApplication::onMarkerButtonClicked()
{
   m_marker_index++;
   std::string log = "MARKER";
   log += std::to_string(m_marker_index);
   log += '\n';
   addToTerminal("MARKER", log, TRACE_MARKER_COLOR, this->palette().color(QPalette::Text).rgb());
}
void MainApplication::onLoggingButtonClicked()
{
   if (!m_file_logger->isActive())
   {
      UT_Log(MAIN, LOW, "enabling file logging");
      m_log_file_name = m_file_logging_path + '/' + createLogFileName();
      if (m_file_logger->openFile(m_log_file_name))
      {
         UT_Log(MAIN, INFO, "Logging started, new logfile created: %s", m_log_file_name.c_str());
         ui->statusbar->showMessage(QString("").asprintf("New log file: %s", m_log_file_name.c_str()), SETTING_GET_U32(MainApplication_statusBarTimeout));
         setButtonState(ui->loggingButton, true);
      }
      else
      {
         QString error_message = QString().asprintf("Cannot create logfile @ %s\n%s (%u)", m_file_logging_path.c_str(), strerror(errno), errno);
         QMessageBox messageBox;
         messageBox.setText(error_message);
         messageBox.setWindowTitle("Error");
         messageBox.setIcon(QMessageBox::Critical);
         messageBox.setPalette(this->palette());
         messageBox.exec();
      }
   }
   else
   {
      UT_Log(MAIN, LOW, "stopping file logging");
      ui->statusbar->showMessage(QString("").asprintf("Log file ready: %s", m_log_file_name.c_str()), SETTING_GET_U32(MainApplication_statusBarTimeout));
      m_file_logger->closeFile();
      setButtonState(ui->loggingButton, false);
   }
}
void MainApplication::onScrollButtonClicked()
{
   setScrolling(!m_scrolling_active);
}
void MainApplication::onTraceScrollButtonClicked()
{
   setTraceScrolling(!m_trace_scrolling_active);
}
void MainApplication::onTraceClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing trace window requested");
   ui->traceView->clear();
}
void MainApplication::onLoggingButtonContextMenuRequested()
{
   Dialogs::LoggingSettingDialog dialog;
   UT_Log(MAIN, MEDIUM, "Opening log setting dialog with path %s", m_file_logging_path.c_str());
   auto result = dialog.showDialog(this, m_file_logging_path, !m_file_logger->isActive());
   if (result)
   {
      UT_Log(MAIN, LOW, "got new logging path %s", result.value().c_str());
      m_file_logging_path = result.value();
   }
}
void MainApplication::onClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing requested");
   ui->terminalView->clear();
}
void MainApplication::onCurrentPortSelectionChanged(int index)
{
   std::string port_name = ui->portComboBox->itemText(index).toStdString();
   UT_Log(MAIN, LOW, "Selected port changed, idx %u, name %s", (uint8_t) index, port_name.c_str());

   uint8_t port_id = portNameToId(port_name);
   std::vector<std::string>& commands_history = m_commands_history[port_id];
   ui->textEdit->clear();

   for (const std::string& command : commands_history)
   {
      ui->textEdit->insertItem(0, QString(command.c_str()));
   }
   ui->textEdit->lineEdit()->clear();
}
void MainApplication::onPortSwitchRequest()
{
   int element_count = ui->portComboBox->count();
   if (element_count > 1)
   {
      int idx = ui->portComboBox->currentIndex();
      if (idx == (element_count - 1))
      {
         ui->portComboBox->setCurrentIndex(0);
      }
      else
      {
         ui->portComboBox->setCurrentIndex(idx+1);
      }
   }
}
void MainApplication::onSettingsButtonClicked()
{
   Dialogs::ApplicationSettingsDialog dialog (m_port_handlers, m_trace_filter_handlers, m_file_logger, m_file_logging_path, *this);
   dialog.showDialog(this);
}
bool MainApplication::sendToPort(const std::string& string)
{
   bool result = false;
   std::string port_name = ui->portComboBox->currentText().toStdString();
   uint8_t port_id = portNameToId(port_name);
   std::string data_to_send = string;
   UT_Log(MAIN, HIGH, "Trying to send data to port %u[%s] - %s", port_id, port_name.c_str(), string.c_str());
   std::string current_ending = ui->lineEndingComboBox->currentText().toStdString();
   if (current_ending == "\\r\\n")
   {
      data_to_send += "\r\n";
   }
   else if (current_ending == "\\n")
   {
      data_to_send += '\n';
   }

   for (const auto& handler : m_port_handlers)
   {
      if (handler->getName() == port_name)
      {
         if (handler->write({data_to_send.begin(), data_to_send.end()}, data_to_send.size()))
         {
            result = true;
            addToTerminal(port_name, data_to_send, ui->terminalView->palette().color(QPalette::Base).rgb(), this->palette().color(QPalette::Text).rgb());
            addToCommandHistory(port_id, string);
         }
         else
         {
            std::string error = "Cannot send data to port ";
            error += port_name;
            UT_Log(MAIN, ERROR, "%s", error.c_str());
            error += '\n';
            addToTerminal(port_name, error, ui->terminalView->palette().color(QPalette::Base).rgb(), this->palette().color(QPalette::Text).rgb());
         }
      }
   }
   return result;
}
void MainApplication::onSendButtonClicked()
{
   UT_Log(MAIN, LOW, "Send button clicked");
   (void)sendToPort(ui->textEdit->currentText().toStdString());
}
void MainApplication::setButtonState(QPushButton* button, bool state)
{
   if (state)
   {
      QPalette palette = this->palette();
      palette.setColor(QPalette::Button, Qt::green);
      palette.setColor(QPalette::ButtonText, Qt::black);
      button->setPalette(palette);
      button->update();
   }
   else
   {
      button->setPalette(this->palette());
      button->update();
   }
}
void MainApplication::setScrolling(bool active)
{
   UT_Log(MAIN, MEDIUM, "%s %u", __func__, active);
   m_scrolling_active = active;
   setButtonState(ui->scrollButton, m_scrolling_active);
}
void MainApplication::setTraceScrolling(bool active)
{
   UT_Log(MAIN, MEDIUM, "%s %u", __func__, active);
   m_trace_scrolling_active = active;
   setButtonState(ui->traceScrollButton, m_trace_scrolling_active);
}
void MainApplication::onPersistenceRead(const std::vector<uint8_t>& data)
{
   std::string line_ending;
   std::string application_version;
   uint32_t offset = 0;
   uint8_t ports_count = 0;

   ::deserialize(data, offset, application_version);
   ::deserialize(data, offset, m_file_logging_path);
   ::deserialize(data, offset, m_scrolling_active);
   ::deserialize(data, offset, m_trace_scrolling_active);
   ::deserialize(data, offset, line_ending);

   ::deserialize(data, offset, ports_count);
   for (uint8_t i = 0; i < ports_count; i++)
   {
      uint8_t port_id;
      uint32_t commands_size;
      std::vector<std::string> port_commands;
      ::deserialize(data, offset, port_id);
      ::deserialize(data, offset, commands_size);
      for (uint32_t i = 0; i < commands_size; i++)
      {
         std::string command;
         ::deserialize(data, offset, command);
         port_commands.push_back(command);
      }

      if (port_id <= PORT_HANDLERS_COUNT)
      {
         m_commands_history[port_id] = port_commands;
      }
      UT_Log(MAIN, HIGH, "Restored %u commands for port %u", commands_size, port_id);
   }
   setScrolling(m_scrolling_active);
   setTraceScrolling(m_trace_scrolling_active);
   ui->lineEndingComboBox->setCurrentText(QString(line_ending.c_str()));
   UT_Log_If(application_version != std::string(APPLICATION_VERSION), MAIN, INFO, "Application update detected %s -> %s", application_version.c_str(), std::string(APPLICATION_VERSION).c_str());
}
void MainApplication::onPersistenceWrite(std::vector<uint8_t>& data)
{
   ::serialize(data, std::string(APPLICATION_VERSION));
   ::serialize(data, m_file_logging_path);
   ::serialize(data, m_scrolling_active);
   ::serialize(data, m_trace_scrolling_active);
   ::serialize(data, ui->lineEndingComboBox->currentText().toStdString());
   ::serialize(data, (uint8_t)m_commands_history.size());
   for (const auto& item : m_commands_history)
   {
      ::serialize(data, item.first);
      ::serialize(data, (uint32_t)item.second.size());
      for (const std::string& history_item : item.second)
      {
         ::serialize(data, history_item);
      }
      UT_Log(MAIN, HIGH, "Serialized %u commands for port %u", item.second.size(), item.first);
   }
}

void MainApplication::addToCommandHistory(uint8_t port_id, const std::string& text)
{
   if (!text.empty())
   {
      /* insert item at the top of combobox */
      ui->textEdit->insertItem(0, QString(text.c_str()));
      /* put in history */
      std::vector<std::string>& history = m_commands_history[port_id];
      history.push_back(text);
      while(history.size() >= MAX_COMMANDS_HISTORY_ITEMS)
      {
         ui->textEdit->removeItem(history.size());
         history.erase(history.begin());
      }
   }
}
void MainApplication::reloadTheme(IThemeController::Theme theme)
{
   if (m_current_theme != theme)
   {
      UT_Log(MAIN_GUI, INFO, "reloading theme %u -> %u", (uint8_t)m_current_theme, (uint8_t)theme);
      m_current_theme = theme;
      ui->loadTheme(theme);
      for (const auto& port : m_port_handlers)
      {
         port->refreshUi();
      }
      for (const auto& handler : m_trace_filter_handlers)
      {
         handler->refreshUi();
      }
      setButtonState(ui->loggingButton, m_file_logger->isActive());
      setButtonState(ui->scrollButton, m_scrolling_active);
      setButtonState(ui->traceScrollButton, m_trace_scrolling_active);
      SETTING_SET_U32(GUI_Theme_ID, theme == IThemeController::Theme::DARK? SETTING_GET_U32(GUI_Dark_Theme_ID) : SETTING_GET_U32(GUI_Default_Theme_ID));
   }
}
IThemeController::Theme MainApplication::currentTheme()
{
   return m_current_theme;
}
std::string MainApplication::themeToName(IThemeController::Theme theme)
{
   UT_Assert(theme < IThemeController::Theme::APPLICATION_THEMES_MAX);
   return m_themes_names[(uint32_t)theme];
}
IThemeController::Theme MainApplication::nameToTheme(const std::string& name)
{
   IThemeController::Theme result = IThemeController::Theme::APPLICATION_THEMES_MAX;
   auto it = std::find(m_themes_names.begin(), m_themes_names.end(), name);
   if (it != m_themes_names.end())
   {
      result = (IThemeController::Theme)(std::distance(m_themes_names.begin(), it));
   }
   return result;
}

uint8_t MainApplication::portNameToId(const std::string& name)
{
   auto result = std::find_if(m_port_id_name_map.begin(), m_port_id_name_map.end(), [&](const auto& obj){return obj.second == name;});
   if (result != m_port_id_name_map.end())
   {
      return result->first;
   }
   else
   {
      return 0;
   }
}
std::string MainApplication::createLogFileName()
{
   std::string result;
   /* create new file */
   auto currentTime = std::chrono::system_clock::now();
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);
   result = QString().asprintf("log_%u%.2u%u_%.2u%.2u%.2u",
                                   ts->tm_mday, ts->tm_mon, ts->tm_year + 1900,
                                   ts->tm_hour, ts->tm_min, ts->tm_sec).toStdString();
   return result;
}
