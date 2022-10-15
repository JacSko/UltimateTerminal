#include "MainApplication.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QColor>

#include "SettingsHandler.h"
#include "Settings.h"
#include "LoggerEngine.h"


MainApplication::MainApplication(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_timers(Utilities::ITimersFactory::create())
{
    ui->setupUi(this);

    Settings::SettingsHandler::create();
    Settings::SettingsHandler::get()->start(CONFIG_FILE_PATH, m_timers.get());
    LoggerEngine::get()->startFrontends();
    Settings::SettingsHandler::get()->printSettings();


    m_timers->start();

    connectSignalsToSlots();
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_1, ui->portLabel_1, *m_timers, std::bind(&MainApplication::onPortHandlerEvent, this, std::placeholders::_1), this)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_2, ui->portLabel_2, *m_timers, std::bind(&MainApplication::onPortHandlerEvent, this, std::placeholders::_1), this)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_3, ui->portLabel_3, *m_timers, std::bind(&MainApplication::onPortHandlerEvent, this, std::placeholders::_1), this)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_4, ui->portLabel_4, *m_timers, std::bind(&MainApplication::onPortHandlerEvent, this, std::placeholders::_1), this)));
    m_port_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
          new GUI::PortHandler(ui->portButton_5, ui->portLabel_5, *m_timers, std::bind(&MainApplication::onPortHandlerEvent, this, std::placeholders::_1), this)));

    ui->lineEndingComboBox->addItem("\\r\\n");
    ui->lineEndingComboBox->addItem("\\n");
    ui->lineEndingComboBox->addItem("EMPTY");


    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4");

    addToTerminal("test", "test data1");
    addToTerminal("test", "test data2");
    addToTerminal("test", "test data3");
    addToTerminal("test", "test data4", 0xFF0000);

}
MainApplication::~MainApplication()
{
    delete ui;
}
void MainApplication::onPortHandlerEvent(const GUI::PortHandler::PortHandlerEvent& event)
{
   if (event.event == GUI::PortHandler::Event::NEW_DATA)
   {
      addToTerminal(event.name, std::string(event.data.begin(), event.data.end()), event.trace_color);
   }
   else if (event.event == GUI::PortHandler::Event::CONNECTED)
   {
      ui->portComboBox->addItem(QString(event.name.c_str()));
   }
   else if (event.event == GUI::PortHandler::Event::DISCONNECTED)
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
   // remove last character (it is newline, this will be added by QListWidget)
   new_line.chop(1);
   QListWidgetItem* item = new QListWidgetItem();
   item->setText(new_line);
   item->setBackground(QColor(rgb_color));
   ui->terminalView->addItem(item);
   ui->terminalView->scrollToBottom();

}
void MainApplication::connectSignalsToSlots()
{
   connect(ui->markerButton, SIGNAL(clicked()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->loggingButton, SIGNAL(clicked()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonClicked()));

   connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));

   connect(ui->userButton_1, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_2, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_3, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_4, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_5, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_6, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_7, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_8, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_9, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(ui->userButton_10, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));

   ui->userButton_1->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_2->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_3->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_4->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_5->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_6->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_7->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_8->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_9->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->userButton_10->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

   connect(ui->userButton_1, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_2, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_3, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_4, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_5, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_6, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_7, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_8, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_9, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(ui->userButton_10, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));

}
void MainApplication::onMarkerButtonClicked()
{
}
void MainApplication::onLoggingButtonClicked()
{
}
void MainApplication::onClearButtonClicked()
{
}
void MainApplication::onSendButtonClicked()
{
}
void MainApplication::onUserButtonClicked()
{
}
void MainApplication::onUserButtonContextMenuRequested()
{
}
