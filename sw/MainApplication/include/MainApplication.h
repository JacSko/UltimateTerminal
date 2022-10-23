#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>

#include "ITimers.h"
#include "PortHandler.h"
#include "LoggingSettingDialog.h"
#include "PersistenceHandler.hpp"
#include "UserButtonHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainApplication : public QMainWindow,
                        public GUI::PortHandlerListener,
                        public Persistence::PersistenceListener
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
    std::vector<std::unique_ptr<GUI::UserButtonHandler>> m_user_button_handlers;
    uint32_t m_marker_index;
    FileLogging m_filelogging;
    Persistence::PersistenceHandler m_persistence;
    bool m_scrolling_active;
    uint32_t m_scroll_default_color;

    void onPortHandlerEvent(const GUI::PortHandlerEvent&);
    void connectSignalsToSlots();
    bool sendToPort(const std::string&);
    void addToTerminal(const std::string& port_name, const std::string& data, uint32_t rgb_color = 0xFFFFFF);
    void setButtonColor(QPushButton* button, QColor color);
    void setScrolling(bool active);
    void onPersistenceRead(const std::vector<uint8_t>& data);
    void onPersistenceWrite(std::vector<uint8_t>& data);
public slots:
   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onScrollButtonClicked();
   void onClearButtonClicked();
   void onSendButtonClicked();
   void onLoggingButtonContextMenuRequested();

};
#endif
