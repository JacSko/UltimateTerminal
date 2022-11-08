#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include "QtWidgets/QLabel"
#include "QtWidgets/QPushButton"
#include <vector>
#include <map>

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
private:
    Ui::MainWindow *ui;
    std::unique_ptr<Utilities::ITimers> m_timers;
    std::unique_ptr<IFileLogger> m_file_logger;
    std::vector<std::unique_ptr<GUI::PortHandler>> m_port_handlers;
    std::vector<std::unique_ptr<GUI::UserButtonHandler>> m_user_button_handlers;
    std::vector<std::unique_ptr<TraceFilterHandler>> m_trace_filter_handlers;
    uint32_t m_marker_index;
    LoggingSettingDialog::Settings m_file_logger_settings;
    Persistence::PersistenceHandler m_persistence;
    bool m_scrolling_active;
    bool m_trace_scrolling_active;
    uint32_t m_scroll_default_color;
    std::map<uint8_t,std::string> m_port_id_name_map;

    void onPortHandlerEvent(const GUI::PortHandlerEvent&);
    void connectSignalsToSlots();
    bool sendToPort(const std::string&);
    void addToTerminal(const std::string& port_name, const std::string& data, uint32_t rgb_color = 0xFFFFFF);
    void setButtonColor(QPushButton* button, QColor color);
    void setScrolling(bool active);
    void setTraceScrolling(bool active);
    void onPersistenceRead(const std::vector<uint8_t>& data);
    void onPersistenceWrite(std::vector<uint8_t>& data);
    void addtoCommandHistory(QComboBox* item, const QString& text);
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

};
#endif
