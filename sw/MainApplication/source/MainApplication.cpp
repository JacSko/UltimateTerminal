#include "MainApplication.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QMessageBox>

#include "SettingsHandler.h"
#include "Settings.h"
#include "LoggerEngine.h"
#include "Serialize.hpp"

constexpr uint32_t TRACE_MARKER_COLOR = 0xFF0055;

MainApplication::MainApplication(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_timers(Utilities::ITimersFactory::create()),
m_file_logger(IFileLogger::create()),
m_marker_index(0),
m_scrolling_active(false),
m_trace_scrolling_active(false)
{
    ui->setupUi(this);

    m_scroll_default_color = ui->scrollButton->palette().color(QPalette::Button).rgb();

    Persistence::PersistenceListener::setName("MAIN_APPLICATION");
    m_persistence.addListener(*this);

    Settings::SettingsHandler::create();
    Settings::SettingsHandler::get()->start(CONFIG_FILE_PATH, m_timers.get());
    LoggerEngine::get()->startFrontends();
    Settings::SettingsHandler::get()->printSettings();

    m_timers->start();

    connectSignalsToSlots();
    setButtonColor(ui->loggingButton, Qt::red);
    setScrolling(true);
    setTraceScrolling(true);

    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_1, ui->portLabel_1, *m_timers, this, this, m_persistence)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_2, ui->portLabel_2, *m_timers, this, this, m_persistence)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_3, ui->portLabel_3, *m_timers, this, this, m_persistence)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_4, ui->portLabel_4, *m_timers, this, this, m_persistence)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_5, ui->portLabel_5, *m_timers, this, this, m_persistence)));

    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_1, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_2, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_3, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_4, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_5, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_6, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_7, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_8, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_9, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));
    m_user_button_handlers.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(
          new GUI::UserButtonHandler(ui->userButton_10, this, m_persistence, std::bind(&MainApplication::sendToPort, this, std::placeholders::_1))));

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

    for (auto& handler : m_user_button_handlers)
    {
       handler->startThread();
    }

    if (!m_persistence.restore(SETTING_GET_STRING(Persistence_filePath)))
    {
       UT_Log(MAIN, ERROR, "Cannot restore persistence!");
    }
}
MainApplication::~MainApplication()
{
   if (!m_persistence.save(SETTING_GET_STRING(Persistence_filePath)))
   {
      UT_Log(MAIN, ERROR, "Cannot write persistence!");
   }

   m_persistence.removeListener(*this);
   delete ui;
}
void MainApplication::onPortHandlerEvent(const GUI::PortHandlerEvent& event)
{
   if (event.event == GUI::Event::NEW_DATA)
   {
      addToTerminal(event.name, std::string(event.data.begin(), event.data.end()), event.trace_color);
   }
   else if (event.event == GUI::Event::CONNECTED)
   {
      ui->portComboBox->addItem(QString(event.name.c_str()));
   }
   else if (event.event == GUI::Event::DISCONNECTED)
   {
      int index = ui->portComboBox->findText(event.name.c_str());
      if (index != -1)
      {
         ui->portComboBox->removeItem(index);
      }
   }
}
void MainApplication::addToTerminal(const std::string& port_name, const std::string& data, uint32_t rgb_color)
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

   if(ui->terminalView->count() >= 10000)
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in main terminal, cleaning trace view");
      ui->terminalView->clear();
   }

   if(ui->traceViw->count() >= 10000)
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines in trace view, cleaning trace view");
      ui->traceViw->clear();
   }

   QListWidgetItem* item = new QListWidgetItem();
   item->setText(new_line);
   item->setBackground(QColor(rgb_color));
   ui->terminalView->addItem(item);

   for (auto& filter : m_trace_filter_handlers)
   {
      std::optional<uint32_t> color = filter->tryMatch(data);
      if (color.has_value())
      {
         QListWidgetItem* item = new QListWidgetItem();
         item->setText(new_line);
         item->setBackground(QColor(color.value()));
         ui->traceViw->addItem(item);
      }
   }

   if (m_scrolling_active)
   {
      ui->terminalView->scrollToBottom();
   }

   if (m_trace_scrolling_active)
   {
      ui->traceViw->scrollToBottom();
   }

}
void MainApplication::connectSignalsToSlots()
{
   connect(ui->markerButton, SIGNAL(clicked()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonClicked()));
   connect(ui->traceClearButton, SIGNAL(clicked()), this, SLOT(onTraceClearButtonClicked()));

   connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
   connect(ui->textEdit, SIGNAL(returnPressed()), this, SLOT(onSendButtonClicked()));

   ui->loggingButton->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(ui->loggingButton, SIGNAL(clicked()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->loggingButton, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onLoggingButtonContextMenuRequested()));

   connect(ui->scrollButton, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
   connect(ui->traceScrollButton, SIGNAL(clicked()), this, SLOT(onTraceScrollButtonClicked()));
}
void MainApplication::onMarkerButtonClicked()
{
   m_marker_index++;
   std::string log = "MARKER";
   log += std::to_string(m_marker_index);
   log += '\n';
   addToTerminal("MARKER", log, TRACE_MARKER_COLOR);
}
void MainApplication::onLoggingButtonClicked()
{
   if (!m_file_logger->isActive())
   {
      if (m_file_logger->openFile(m_file_logger_settings.getPath()))
      {
         setButtonColor(ui->loggingButton, Qt::green);
      }
   }
   else
   {
      m_file_logger->closeFile();
      setButtonColor(ui->loggingButton, Qt::red);
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
   ui->traceViw->clear();
}
void MainApplication::onLoggingButtonContextMenuRequested()
{
   LoggingSettingDialog dialog;
   LoggingSettingDialog::Settings new_settings = {};
   auto result = dialog.showDialog(this, m_file_logger_settings, new_settings, !m_file_logger->isActive());
   if (result)
   {
      if (result.value())
      {
         UT_Log(MAIN, LOW, "got new logging settings: auto %u, file:%s/%s", new_settings.use_default_name, new_settings.file_path.c_str(), new_settings.file_name.c_str());
         m_file_logger_settings = new_settings;
      }
   }
}
void MainApplication::onClearButtonClicked()
{
   UT_Log(MAIN, MEDIUM, "Clearing requested");
   ui->terminalView->clear();
}
bool MainApplication::sendToPort(const std::string& string)
{
   bool result = false;
   std::string current_port = ui->portComboBox->currentText().toStdString();
   std::string data_to_send = string;

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
      if (handler->getName() == current_port)
      {
         if (handler->write({data_to_send.begin(), data_to_send.end()}, data_to_send.size()))
         {
            result = true;
            addToTerminal(current_port, data_to_send);
         }
         else
         {
            std::string error = "Cannot send data to port ";
            error += current_port;
            UT_Log(MAIN, ERROR, "%s", error.c_str());
            error += '\n';
            addToTerminal(current_port, error);
         }
      }
   }
   return result;
}
void MainApplication::onSendButtonClicked()
{
   (void)sendToPort(ui->textEdit->text().toStdString());
}
void MainApplication::setButtonColor(QPushButton* button, QColor color)
{
   QPalette palette = button->palette();
   palette.setColor(QPalette::Button, color);
   button->setPalette(palette);
   button->update();
}
void MainApplication::setScrolling(bool active)
{
   m_scrolling_active = active;
   if(m_scrolling_active)
   {
      setButtonColor(ui->scrollButton, Qt::green);
   }
   else
   {
      setButtonColor(ui->scrollButton, QColor(m_scroll_default_color));
   }
}
void MainApplication::setTraceScrolling(bool active)
{
   m_trace_scrolling_active = active;
   if(m_trace_scrolling_active)
   {
      setButtonColor(ui->traceScrollButton, Qt::green);
   }
   else
   {
      setButtonColor(ui->traceScrollButton, QColor(m_scroll_default_color));
   }
}
void MainApplication::onPersistenceRead(const std::vector<uint8_t>& data)
{
   uint32_t offset = 0;
   ::deserialize(data, offset, m_file_logger_settings.file_path);
   ::deserialize(data, offset, m_file_logger_settings.file_name);
   ::deserialize(data, offset, m_file_logger_settings.use_default_name);
   ::deserialize(data, offset, m_scrolling_active);

   setScrolling(m_scrolling_active);

}
void MainApplication::onPersistenceWrite(std::vector<uint8_t>& data)
{
   ::serialize(data, m_file_logger_settings.file_path);
   ::serialize(data, m_file_logger_settings.file_name);
   ::serialize(data, m_file_logger_settings.use_default_name);
   ::serialize(data, m_scrolling_active);
}
