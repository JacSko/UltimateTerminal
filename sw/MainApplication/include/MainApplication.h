#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>

#include "ITimers.h"
#include "PortHandler.h"
#include "LoggingSettingDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainApplication : public QMainWindow
{
    Q_OBJECT

public:
    MainApplication(QWidget *parent = nullptr);
    ~MainApplication();
private:
    struct FileLogging
    {
       bool is_running;
       LoggingSettingDialog::Settings settings;
       std::ofstream file_stream;
    };

    Ui::MainWindow *ui;
    std::unique_ptr<Utilities::ITimers> m_timers;
    std::vector<std::unique_ptr<GUI::PortHandler>> m_port_handlers;
    uint32_t m_marker_index;
    FileLogging m_filelogging;

    void onPortHandlerEvent(const GUI::PortHandler::PortHandlerEvent&);
    void connectSignalsToSlots();
    void addToTerminal(const std::string& port_name, const std::string& data, uint32_t rgb_color = 0xFFFFFF);
    void setButtonColor(QPushButton* button, QColor color);
public slots:
   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onClearButtonClicked();

   void onSendButtonClicked();
   void onUserButtonClicked();
   void onUserButtonContextMenuRequested();
   void onLoggingButtonContextMenuRequested();

};
#endif
