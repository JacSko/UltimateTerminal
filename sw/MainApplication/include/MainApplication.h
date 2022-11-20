#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>
#include <map>

#include "ui_MainWindow.h"
#include "ITimers.h"
#include "PortHandler.h"
#include "LoggingSettingDialog.h"
#include "PersistenceHandler.h"
#include "UserButtonHandler.h"
#include "TraceFilterHandler.h"
#include "IFileLogger.h"

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

    void reloadTheme(Ui_MainWindow::Theme);
    std::string themeToName(Ui_MainWindow::Theme);
    Ui_MainWindow::Theme nameToTheme(const std::string& name);

    static const uint32_t MAX_COMMANDS_HISTORY_ITEMS = 100;
private:
    Ui::MainWindow *ui;
    std::unique_ptr<Utilities::ITimers> m_timers;
    std::unique_ptr<IFileLogger> m_file_logger;
    std::vector<std::unique_ptr<GUI::PortHandler>> m_port_handlers;
    std::vector<std::unique_ptr<GUI::UserButtonHandler>> m_user_button_handlers;
    std::vector<std::unique_ptr<TraceFilterHandler>> m_trace_filter_handlers;
    uint32_t m_marker_index;
    std::string m_file_logging_path;
    std::string m_log_file_name;
    Persistence::PersistenceHandler m_persistence;
    bool m_scrolling_active;
    bool m_trace_scrolling_active;
    uint32_t m_scroll_default_color;
    std::map<uint8_t,std::string> m_port_id_name_map;
    std::map<uint8_t, std::vector<std::string>> m_commands_history;
    Ui_MainWindow::Theme m_current_theme;

    void onPortHandlerEvent(const GUI::PortHandlerEvent&);
    void connectSignalsToSlots();
    bool sendToPort(const std::string&);
    void addToTerminal(const std::string& port_name, const std::string& data, uint32_t background_color, uint32_t font_color);
    void setButtonState(QPushButton* button, bool active);
    void setScrolling(bool active);
    void setTraceScrolling(bool active);
    void onPersistenceRead(const std::vector<uint8_t>& data);
    void onPersistenceWrite(std::vector<uint8_t>& data);
    void addToCommandHistory(uint8_t port_id, const std::string& text);
    uint8_t portNameToId(const std::string& name);
    std::string createLogFileName();
public slots:
   void onMarkerButtonClicked();
   void onLoggingButtonClicked();
   void onScrollButtonClicked();
   void onClearButtonClicked();
   void onTraceScrollButtonClicked();
   void onTraceClearButtonClicked();
   void onSendButtonClicked();
   void onLoggingButtonContextMenuRequested();
   void onCurrentPortSelectionChanged(int );
   void onPortSwitchRequest();
   void onSettingsButtonClicked();

};
#endif
