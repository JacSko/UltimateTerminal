#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>

#include "ITimers.h"
#include "PortSettingDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

constexpr uint8_t NUMBER_OF_PORTS = 5;

class MainApplication : public QMainWindow
{
    Q_OBJECT

public:
    MainApplication(QWidget *parent = nullptr);
    ~MainApplication();
private:
    Ui::MainWindow *ui;

    std::unique_ptr<Utilities::ITimers> m_timers;
    std::map<QObject*, PortSettingDialog::Settings> m_port_settings_map;

    void setObjectNames();
    void connectSignalsToSlots();

public slots:
   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onClearButtonClicked();

   void onSendButtonClicked();
   void onPortButtonClicked();
   void onUserButtonClicked();
   void onUserButtonContextMenuRequested();
   void onPortButtonContextMenuRequested();

};
#endif
