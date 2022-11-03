#include "MainApplication.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QColor>

#include "SettingsHandler.h"
#include "Settings.h"
#include "LoggerEngine.h"
#include "Serialize.hpp"

MainApplication::MainApplication(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_timers(Utilities::ITimersFactory::create()),
m_marker_index(0),
m_filelogging({}),
m_scrolling_active(false)
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

   if (m_filelogging.is_running)
   {
      m_filelogging.file_stream << new_line.toStdString();
      m_filelogging.file_stream.flush();
   }

   // remove last character (it is newline, this will be added by QListWidget)
   new_line.chop(1);
   QListWidgetItem* item = new QListWidgetItem();
   item->setText(new_line);
   item->setBackground(QColor(rgb_color));
   ui->terminalView->addItem(item);
   if (m_scrolling_active)
   {
      ui->terminalView->scrollToBottom();
   }

   if(ui->terminalView->count() >= 10000)
   {
      UT_Log(MAIN, MEDIUM, "Reached maximum lines, cleaning trace view");
      ui->terminalView->clear();
   }
}
void MainApplication::connectSignalsToSlots()
{
   connect(ui->markerButton, SIGNAL(clicked()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonClicked()));

   connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
   connect(ui->textEdit, SIGNAL(returnPressed()), this, SLOT(onSendButtonClicked()));

   ui->loggingButton->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(ui->loggingButton, SIGNAL(clicked()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->loggingButton, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onLoggingButtonContextMenuRequested()));

   connect(ui->scrollButton, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
}
void MainApplication::onMarkerButtonClicked()
{
   m_marker_index++;
   std::string log = "MARKER";
   log += std::to_string(m_marker_index);
   log += '\n';
   addToTerminal("MARKER", log, 0xFF0055);
}
void MainApplication::onLoggingButtonClicked()
{
   if (!m_filelogging.is_running)
   {
      m_filelogging.file_stream.open(m_filelogging.settings.getPath(), std::ios::out);
      if (m_filelogging.file_stream)
      {
         m_filelogging.is_running = true;
         setButtonColor(ui->loggingButton, Qt::green);
      }
   }
   else
   {
      m_filelogging.file_stream.close();
      m_filelogging.is_running = false;
      setButtonColor(ui->loggingButton, Qt::red);
   }
}
void MainApplication::onScrollButtonClicked()
{
   setScrolling(!m_scrolling_active);
}
void MainApplication::onLoggingButtonContextMenuRequested()
{
   LoggingSettingDialog dialog;
   LoggingSettingDialog::Settings new_settings = {};
   auto result = dialog.showDialog(this, m_filelogging.settings, new_settings, !m_filelogging.is_running);
   if (result)
   {
      if (result.value())
      {
         UT_Log(MAIN, LOW, "got new logging settings: auto %u, file:%s/%s", new_settings.use_default_name, new_settings.file_path.c_str(), new_settings.file_name.c_str());
         m_filelogging.settings = new_settings;
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
   if (ui->lineEndingComboBox->currentText() == "\\r\\n")
   {
      data_to_send += "\r\n";
   }
   else if (ui->lineEndingComboBox->currentText() == "\\n")
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
void MainApplication::onPersistenceRead(const std::vector<uint8_t>& data)
{
   uint32_t offset = 0;
   ::deserialize(data, offset, m_filelogging.settings.file_path);
   ::deserialize(data, offset, m_filelogging.settings.file_name);
   ::deserialize(data, offset, m_filelogging.settings.use_default_name);
   ::deserialize(data, offset, m_scrolling_active);

   setScrolling(m_scrolling_active);

}
void MainApplication::onPersistenceWrite(std::vector<uint8_t>& data)
{
   ::serialize(data, m_filelogging.settings.file_path);
   ::serialize(data, m_filelogging.settings.file_name);
   ::serialize(data, m_filelogging.settings.use_default_name);
   ::serialize(data, m_scrolling_active);
}
