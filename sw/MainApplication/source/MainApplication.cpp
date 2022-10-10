#include "MainApplication.h"
#include "ui_MainWindow.h"

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
}
MainApplication::~MainApplication()
{
    delete ui;
}
void MainApplication::connectSignalsToSlots()
{
   connect(ui->markerButton, SIGNAL(clicked()), this, SLOT(onMarkerButtonClicked()));
   connect(ui->loggingButton, SIGNAL(clicked()), this, SLOT(onLoggingButtonClicked()));
   connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(onClearButtonClicked()));

   connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendButtonClicked()));
   connect(ui->portButton_1, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_2, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_3, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_4, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_5, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));

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

   connect(ui->portButton_1, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_2, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_3, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_4, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));
   connect(ui->portButton_5, SIGNAL(clicked()), this, SLOT(onPortButtonClicked()));

   ui->portButton_1->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->portButton_2->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->portButton_3->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->portButton_4->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   ui->portButton_5->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

   connect(ui->portButton_1, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
   connect(ui->portButton_2, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
   connect(ui->portButton_3, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
   connect(ui->portButton_4, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));
   connect(ui->portButton_5, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onPortButtonContextMenuRequested()));

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
void MainApplication::onPortButtonClicked()
{
}
void MainApplication::onPortButtonContextMenuRequested()
{
}
void MainApplication::onUserButtonClicked()
{
}
void MainApplication::onUserButtonContextMenuRequested()
{
}
