#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "QtWidgets/QtWidgetsMock.h"

#include "MainApplication.h"
#include "PortHandlerMock.h"
#include "UserButtonHandlerMock.h"
#include "TraceFilterHandlerMock.h"
#include "LoggingSettingDialogMock.h"
#include "ITimersMock.h"
#include "PersistenceHandlerMock.h"
#include "IFileLoggerMock.h"

using namespace ::testing;

Utilities::ITimersMock* g_timers_mock;
IFileLoggerMock* g_logger_mock;

std::unique_ptr<Utilities::ITimers> Utilities::ITimersFactory::create()
{
   return std::unique_ptr<ITimers>(g_timers_mock);
}
std::unique_ptr<IFileLogger> IFileLogger::create()
{
   return std::unique_ptr<IFileLogger>(g_logger_mock);
}


struct MainApplicationFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      GUI::PortHandlerMock_init();
      TraceFilterHandlerMock_init();
      LoggingSettingDialogMock_init();
      Persistence::PersistenceHandlerMock_init();
      g_timers_mock = new Utilities::ITimersMock;
      g_logger_mock = new IFileLoggerMock;

      EXPECT_CALL(*QtCoreMock_get(), QObject_objectName(_)).WillOnce(Return(QString("")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_setObjectName(_, _)).Times(AtLeast(1));

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_user_button_1))
                                                         .WillOnce(Return(&test_user_button_2))
                                                         .WillOnce(Return(&test_user_button_5))
                                                         .WillOnce(Return(&test_user_button_3))
                                                         .WillOnce(Return(&test_user_button_4))
                                                         .WillOnce(Return(&test_user_button_6))
                                                         .WillOnce(Return(&test_user_button_7))
                                                         .WillOnce(Return(&test_user_button_8))
                                                         .WillOnce(Return(&test_user_button_9))
                                                         .WillOnce(Return(&test_user_button_10))
                                                         .WillOnce(Return(&test_marker_button))
                                                         .WillOnce(Return(&test_logging_button))
                                                         .WillOnce(Return(&test_send_button))
                                                         .WillOnce(Return(&test_scroll_button))
                                                         .WillOnce(Return(&test_clear_button))
                                                         .WillOnce(Return(&test_trace_clear_button))
                                                         .WillOnce(Return(&test_trace_scroll_button))
                                                         .WillOnce(Return(&test_trace_filter_7))
                                                         .WillOnce(Return(&test_trace_filter_1))
                                                         .WillOnce(Return(&test_trace_filter_5))
                                                         .WillOnce(Return(&test_trace_filter_4))
                                                         .WillOnce(Return(&test_trace_filter_3))
                                                         .WillOnce(Return(&test_trace_filter_6))
                                                         .WillOnce(Return(&test_trace_filter_2))
                                                         .WillOnce(Return(&test_port_button_2))
                                                         .WillOnce(Return(&test_port_button_5))
                                                         .WillOnce(Return(&test_port_button_1))
                                                         .WillOnce(Return(&test_port_button_3))
                                                         .WillOnce(Return(&test_port_button_4));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_new()).WillOnce(Return(&test_info_label))
                                                     .WillOnce(Return(&test_port_label_1))
                                                     .WillOnce(Return(&test_port_label_2))
                                                     .WillOnce(Return(&test_port_label_3))
                                                     .WillOnce(Return(&test_port_label_4))
                                                     .WillOnce(Return(&test_port_label_5));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_port_box))
                                                        .WillOnce(Return(&test_line_ending_box))
                                                        .WillOnce(Return(&test_text_edit));
      EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_new()).WillOnce(Return(&test_terminal_view))
                                                          .WillOnce(Return(&test_trace_view));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_trace_filter_2))
                                                        .WillOnce(Return(&test_trace_filter_1))
                                                        .WillOnce(Return(&test_trace_filter_5))
                                                        .WillOnce(Return(&test_trace_filter_3))
                                                        .WillOnce(Return(&test_trace_filter_6))
                                                        .WillOnce(Return(&test_trace_filter_4))
                                                        .WillOnce(Return(&test_trace_filter_7));
      EXPECT_CALL(*QtWidgetsMock_get(), QMenuBar_new()).WillOnce(Return(&test_menu_bar));
      EXPECT_CALL(*QtWidgetsMock_get(), QStatusBar_new()).WillOnce(Return(&test_status_bar));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(_,_)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(_,_)).Times(AtLeast(1));

      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::red))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));


      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_marker_button, HasSubstr("clicked"), _, HasSubstr("onMarkerButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_clear_button, HasSubstr("clicked"), _, HasSubstr("onClearButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_trace_clear_button, HasSubstr("clicked"), _, HasSubstr("onTraceClearButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_send_button, HasSubstr("clicked"), _, HasSubstr("onSendButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(_, HasSubstr("returnPressed"), _, HasSubstr("onSendButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_logging_button, HasSubstr("clicked"), _, HasSubstr("onLoggingButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_logging_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onLoggingButtonContextMenuRequested")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_scroll_button, HasSubstr("clicked"), _, HasSubstr("onScrollButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_trace_scroll_button, HasSubstr("clicked"), _, HasSubstr("onTraceScrollButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_port_box, HasSubstr("currentIndexChanged"), _, HasSubstr("onCurrentPortSelectionChanged")));

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&test_logging_button, Qt::ContextMenuPolicy::CustomContextMenu));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_line_ending_box, _)).Times(3);

      EXPECT_CALL(*g_timers_mock, start());
      EXPECT_CALL(*Persistence::PersistenceHandlerMock_get(), restore(_)).WillOnce(Return(true));
      m_test_subject = std::unique_ptr<MainApplication>(new MainApplication());

      Mock::VerifyAndClearExpectations(g_timers_mock);
      Mock::VerifyAndClearExpectations(QtCoreMock_get());
      Mock::VerifyAndClearExpectations(QtWidgetsMock_get());
      Mock::VerifyAndClearExpectations(GUI::PortHandlerMock_get());
      Mock::VerifyAndClearExpectations(TraceFilterHandlerMock_get());
      Mock::VerifyAndClearExpectations(LoggingSettingDialogMock_get());
      Mock::VerifyAndClearExpectations(Persistence::PersistenceHandlerMock_get());
   }


   void TearDown()
   {
      EXPECT_CALL(*Persistence::PersistenceHandlerMock_get(), save(_)).WillOnce(Return(true));

      m_test_subject.reset(nullptr);
      Persistence::PersistenceHandlerMock_deinit();
      LoggingSettingDialogMock_deinit();
      TraceFilterHandlerMock_deinit();
      GUI::PortHandlerMock_deinit();
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }


   QMainWindow test_main_window;

   QPushButton test_user_button_1;
   QPushButton test_user_button_2;
   QPushButton test_user_button_3;
   QPushButton test_user_button_4;
   QPushButton test_user_button_5;
   QPushButton test_user_button_6;
   QPushButton test_user_button_7;
   QPushButton test_user_button_8;
   QPushButton test_user_button_9;
   QPushButton test_user_button_10;
   QPushButton test_marker_button;
   QPushButton test_logging_button;
   QPushButton test_send_button;
   QPushButton test_scroll_button;
   QPushButton test_clear_button;
   QPushButton test_trace_clear_button;
   QPushButton test_trace_scroll_button;
   QPushButton test_trace_filter_button_1;
   QPushButton test_trace_filter_button_2;
   QPushButton test_trace_filter_button_3;
   QPushButton test_trace_filter_button_4;
   QPushButton test_trace_filter_button_5;
   QPushButton test_trace_filter_button_6;
   QPushButton test_trace_filter_button_7;
   QPushButton test_port_button_1;
   QPushButton test_port_button_2;
   QPushButton test_port_button_3;
   QPushButton test_port_button_4;
   QPushButton test_port_button_5;
   QLabel test_info_label;
   QLabel test_port_label_1;
   QLabel test_port_label_2;
   QLabel test_port_label_3;
   QLabel test_port_label_4;
   QLabel test_port_label_5;
   QComboBox test_port_box;
   QComboBox test_line_ending_box;
   QComboBox test_text_edit;
   QListWidget test_terminal_view;
   QListWidget test_trace_view;
   QLineEdit test_trace_filter_1;
   QLineEdit test_trace_filter_2;
   QLineEdit test_trace_filter_3;
   QLineEdit test_trace_filter_4;
   QLineEdit test_trace_filter_5;
   QLineEdit test_trace_filter_6;
   QLineEdit test_trace_filter_7;
   QMenuBar test_menu_bar;
   QStatusBar test_status_bar;

   std::unique_ptr<MainApplication> m_test_subject;
};

TEST_F(MainApplicationFixture, adding_data_from_port_handler_to_main_terminal)
{
   /**
    * <b>scenario</b>: Port handler reports opened terminal and start sending data <br>
    * <b>expected</b>: Data shall be added to terminal, but without trailing newline character. <br>
    *                  Check for trace matching shall be made. <br>
    *                  Window shall be scrolled to bottom. <br>
    *                  Check for maximum trace count shall be made. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_TRACE_COLOR = 0x123321;
   constexpr uint8_t TEST_PORT_INDEX = 3;
   constexpr uint32_t TEST_MAX_TRACE_NUMBER = 10000;
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.port_id = 2;
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = GUI::Event::DISCONNECTED;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* adding item with newline at the end */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* adding item without newline at the end */
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t'};
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* items count exceeded the maximum count for terminal view, terminal shall be cleaned */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(TEST_MAX_TRACE_NUMBER));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_clear(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT_NAME"))).WillOnce(Return(TEST_PORT_INDEX));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, TEST_PORT_INDEX));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_close_event);

}

TEST_F(MainApplicationFixture, putting_marker_into_traces)
{
   /**
    * <b>scenario</b>: Marker button clicked <br>
    * <b>expected</b>: Marker shall be added to terminal view with correct index number <br>
    *                  Check for trace matching shall be made. <br>
    *                  Window shall be scrolled to bottom. <br>
    *                  Check for maximum trace count shall be made. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0055; //marker shall be red
   QListWidgetItem terminal_item;

   /* first marker added */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("MARKER1")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_MARKER_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER1")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("MARKER1"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   m_test_subject->onMarkerButtonClicked();

   /* second marker added */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("MARKER2")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_MARKER_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER2")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("MARKER2"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   m_test_subject->onMarkerButtonClicked();

}

TEST_F(MainApplicationFixture, logging_to_file_started_and_stopped)
{
   /**
    * <b>scenario</b>: Logging to file requested by user with predefined filename then stopped, when logging is active user requested to change the name <br>
    * <b>expected</b>: File for logging shall be opened <br>
    *                  Button shall be in green color. <br>
    *                  New trace shall be added to terminal and to the file as well as markers. <br>
    *                  LoggingSettingDialog shall be shown, but with disabled editing. <br>
    *                  File shall be closed. <br>
    *                  Button shall be in red color. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_TRACE_COLOR = 0x123321;
   constexpr uint8_t TEST_PORT_INDEX = 3;
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0055; //marker shall be red
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.port_id = 2;
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = GUI::Event::DISCONNECTED;
   LoggingSettingDialog::Settings logger_settings;
   logger_settings.file_name = "some_log.txt";
   logger_settings.file_path = "/home/user";
   logger_settings.use_default_name = false;

   /* port opening */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* user requested to change the logging file */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(logger_settings), Return(std::optional<bool>(true))));
   m_test_subject->onLoggingButtonContextMenuRequested();

   /* requesting file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*g_logger_mock, openFile("/home/user/some_log.txt")).WillOnce(Return(true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onLoggingButtonClicked();

   /* first marker added */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("MARKER1")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_MARKER_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER1")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("MARKER1"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   m_test_subject->onMarkerButtonClicked();

   /* adding item with newline at the end */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* user requested to change the logging file, it should not be possible when logging is active */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,_,false)).WillOnce(Return(std::optional<bool>(false)));
   m_test_subject->onLoggingButtonContextMenuRequested();

   /* closing port */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT_NAME"))).WillOnce(Return(TEST_PORT_INDEX));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, TEST_PORT_INDEX));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_close_event);

   /* closing file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*g_logger_mock, closeFile());
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::red))));
   m_test_subject->onLoggingButtonClicked();

}

TEST_F(MainApplicationFixture, filelogging_enabling_when_no_port_active)
{
   /**
    * <b>scenario</b>: Logging to file requested by user while no port has been opened, then port was opened, closed, and filelogging was disabled <br>
    * <b>expected</b>: File for logging shall be opened <br>
    *                  Button shall be in green color. <br>
    *                  File shall be closed. <br>
    *                  Button shall be in red color. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_TRACE_COLOR = 0x123321;
   constexpr uint8_t TEST_PORT_INDEX = 3;
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0055; //marker shall be red
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = GUI::Event::DISCONNECTED;
   LoggingSettingDialog::Settings logger_settings;
   logger_settings.file_name = "some_log.txt";
   logger_settings.file_path = "/home/user";
   logger_settings.use_default_name = false;


   /* user requested to change the logging file */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(logger_settings), Return(std::optional<bool>(true))));
   m_test_subject->onLoggingButtonContextMenuRequested();

   /* requesting file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*g_logger_mock, openFile("/home/user/some_log.txt")).WillOnce(Return(true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onLoggingButtonClicked();

   /* port opening */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* closing port */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT_NAME"))).WillOnce(Return(TEST_PORT_INDEX));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, TEST_PORT_INDEX));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_close_event);

   /* closing file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*g_logger_mock, closeFile());
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::red))));
   m_test_subject->onLoggingButtonClicked();

}

TEST_F(MainApplicationFixture, terminal_view_scrolling_deactivation_and_activation)
{
   /**
    * <b>scenario</b>: Scrolling is enabled by default, then was turned OFF by user, then was turned ON again. <br>
    * <b>expected</b>: Button is in default color when scrolling disabled. <br>
    *                  scrollToBottom() shall not be called when adding new trace. <br>
    *                  Button should be green when scrolling enabled. <br>
    *                  scrollToBottom() shall be called when adding new trace. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_TRACE_COLOR = 0x123321;
   constexpr uint8_t TEST_PORT_INDEX = 3;
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0055; //marker shall be red
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.port_id = 2;
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = GUI::Event::DISCONNECTED;


   /* user requested to disable scrolling */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));
   m_test_subject->onScrollButtonClicked();

   /* port opening */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* writing data */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* user requested to enable scrolling */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onScrollButtonClicked();

   /* writing data */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* closing port */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT_NAME"))).WillOnce(Return(TEST_PORT_INDEX));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, TEST_PORT_INDEX));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_close_event);

}

TEST_F(MainApplicationFixture, trace_view_scrolling_deactivation_and_activation)
{
   /**
    * <b>scenario</b>: Trace scrolling is enabled by default, then was turned OFF by user, then was turned ON again. <br>
    * <b>expected</b>: Button is in default color when scrolling disabled. <br>
    *                  scrollToBottom() shall not be called when adding new trace. <br>
    *                  Button should be green when scrolling enabled. <br>
    *                  scrollToBottom() shall be called when adding new trace. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_TRACE_COLOR = 0x123321;
   constexpr uint32_t TEST_FILER_COLOR = 0x999999;
   constexpr uint8_t TEST_PORT_INDEX = 3;
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0055; //marker shall be red
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   QListWidgetItem trace_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.port_id = 2;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = GUI::Event::DISCONNECTED;


   /* user requested to disable scrolling */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));
   m_test_subject->onTraceScrollButtonClicked();

   /* port opening */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* writing data */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item)).WillOnce(Return(&trace_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&trace_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&trace_item, QColor(TEST_FILER_COLOR)));
   /* trace view shall not be scrolled, by terminal view MUST be scrolled */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, &terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_trace_view, &trace_item));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillOnce(Return(std::optional<uint32_t>(TEST_FILER_COLOR)))
                                                                               .WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* user requested to enable scrolling */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onTraceScrollButtonClicked();

   /* writing data */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item)).WillOnce(Return(&trace_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&trace_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&trace_item, QColor(TEST_FILER_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, &terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_trace_view, &trace_item));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillOnce(Return(std::optional<uint32_t>(TEST_FILER_COLOR)))
                                                                               .WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* closing port */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT_NAME"))).WillOnce(Return(TEST_PORT_INDEX));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, TEST_PORT_INDEX));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_close_event);

}

TEST_F(MainApplicationFixture, terminal_and_trace_view_clearing)
{
   /**
    * <b>scenario</b>: User requested to clear trace view and terminal view. <br>
    * <b>expected</b>: clear() method called on terminal view. <br>
    *                  clear() method called on trace view. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_clear(&test_terminal_view));
   m_test_subject->onClearButtonClicked();
   Mock::VerifyAndClearExpectations(QtWidgetsMock_get());

   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_clear(&test_trace_view));
   m_test_subject->onTraceClearButtonClicked();
   Mock::VerifyAndClearExpectations(QtWidgetsMock_get());

}

TEST_F(MainApplicationFixture, sending_data_to_port_no_port_opened)
{
   /**
    * <b>scenario</b>: No port opened, data send requested by user. <br>
    * <b>expected</b>: PortHandler shall be asked about write <br>
    * ************************************************
    */
   const std::string PORT_HANDLER_NAME = "NAME1";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "\n";
   QListWidgetItem terminal_item;

   /* no port opened, so combobox shall be empty*/
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString("")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(DATA_TO_SEND.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString(LINE_ENDING.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillRepeatedly(ReturnRef(PORT_HANDLER_NAME));
   m_test_subject->onSendButtonClicked();
}

TEST_F(MainApplicationFixture, sending_data_to_port)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  PortHandlers shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Data shall be added to terminal view. <br>
    * ************************************************
    */
   const std::string PORT_HANDLER_NAME = "NAME1";
   const std::string INCORRECT_PORT_HANDLER_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "\\n";
   QListWidgetItem terminal_item;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString(PORT_HANDLER_NAME.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(DATA_TO_SEND.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString(LINE_ENDING.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillOnce(ReturnRef(PORT_HANDLER_NAME))
                                                      .WillRepeatedly(ReturnRef(INCORRECT_PORT_HANDLER_NAME));
   /* size to write should be 1 byte more because \n is added */
   std::string data_payload = DATA_TO_SEND + '\n';
   EXPECT_CALL(*GUI::PortHandlerMock_get(), write(std::vector<uint8_t>(data_payload.begin(), data_payload.end()), data_payload.size())).WillOnce(Return(true));

   /* writing data to terminal */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr(DATA_TO_SEND)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr(DATA_TO_SEND)));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr(DATA_TO_SEND))).WillRepeatedly(Return(std::optional<uint32_t>()));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(DATA_TO_SEND.c_str())));

   m_test_subject->onSendButtonClicked();
}

TEST_F(MainApplicationFixture, sending_data_to_port_empty_line_ending)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user with empty line ending. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  PortHandlers shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Data shall be added to terminal view. <br>
    * ************************************************
    */
   const std::string PORT_HANDLER_NAME = "NAME1";
   const std::string INCORRECT_PORT_HANDLER_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "EMPTY";
   QListWidgetItem terminal_item;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString(PORT_HANDLER_NAME.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(DATA_TO_SEND.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString(LINE_ENDING.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillOnce(ReturnRef(PORT_HANDLER_NAME))
                                                      .WillRepeatedly(ReturnRef(INCORRECT_PORT_HANDLER_NAME));
   /* size to write should be 1 byte more because \n is added */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), write(std::vector<uint8_t>(DATA_TO_SEND.begin(), DATA_TO_SEND.end()),DATA_TO_SEND.size())).WillOnce(Return(true));

   /* writing data to terminal */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr(DATA_TO_SEND)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr(DATA_TO_SEND)));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr(DATA_TO_SEND))).WillRepeatedly(Return(std::optional<uint32_t>()));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(DATA_TO_SEND.c_str())));

   m_test_subject->onSendButtonClicked();
}

TEST_F(MainApplicationFixture, sending_data_to_port_failed)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user with empty line ending, but write to port fails. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  PortHandlers shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Error information shall be added to terminal. <br>
    * ************************************************
    */
   const std::string PORT_HANDLER_NAME = "NAME1";
   const std::string INCORRECT_PORT_HANDLER_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "EMPTY";
   QListWidgetItem terminal_item;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString(PORT_HANDLER_NAME.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(DATA_TO_SEND.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString(LINE_ENDING.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillOnce(ReturnRef(PORT_HANDLER_NAME))
                                                      .WillRepeatedly(ReturnRef(INCORRECT_PORT_HANDLER_NAME));
   /* size to write should be 1 byte more because \n is added */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), write(std::vector<uint8_t>(DATA_TO_SEND.begin(), DATA_TO_SEND.end()), DATA_TO_SEND.size())).WillOnce(Return(false));

   /* writing data to terminal */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("Cannot send data to port")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("Cannot send data to port")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("Cannot send data to port"))).WillRepeatedly(Return(std::optional<uint32_t>()));

   m_test_subject->onSendButtonClicked();
}

TEST_F(MainApplicationFixture, persistence_write_and_read)
{
   /**
    * <b>scenario</b>: Persistence write and read sequence to verify data serialization <br>
    * <b>expected</b>: Newly created object shall correctly process the persistence data.<br>
    * ************************************************
    */
   std::vector<uint8_t> data_buffer;
   std::string current_ending = "\\n";
   LoggingSettingDialog::Settings old_logger_settings;
   old_logger_settings.file_name = "old file";
   old_logger_settings.file_path = "old path";
   old_logger_settings.use_default_name = true;
   LoggingSettingDialog::Settings new_logger_settings;
   new_logger_settings.file_name = "new file";
   new_logger_settings.file_path = "new path";
   new_logger_settings.use_default_name = true;

   /* replace all persistent data, to get known state */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(old_logger_settings), Return(std::optional<bool>(true))));
   m_test_subject->onLoggingButtonContextMenuRequested();
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));
   m_test_subject->onScrollButtonClicked();
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));
   m_test_subject->onTraceScrollButtonClicked();
   /* expectations before write */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString(current_ending.c_str())));
   ASSERT_EQ(data_buffer.size(), 0);
   ((Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));


   /* replace all persistent data, to make sure that has been restored from persistence correctly */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(new_logger_settings), Return(std::optional<bool>(true))));
   m_test_subject->onLoggingButtonContextMenuRequested();
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onScrollButtonClicked();
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
   m_test_subject->onTraceScrollButtonClicked();


   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_line_ending_box, QString(current_ending.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_trace_scroll_button, QPalette(QPalette::ColorRole::Button, QColor())));

   ((Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceRead(data_buffer);

}

TEST_F(MainApplicationFixture, command_history_item_removing)
{
   /**
    * <b>scenario</b>: Port opened, command history is full, removing the oldest items started. <br>
    * <b>expected</b>: Last items shall be removed from combobox. <br>
    * ************************************************
    */

   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "\\n";
   GUI::PortHandlerEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString(port_open_event.name.c_str())));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* start writing data to this port */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillRepeatedly(Return(QString(port_open_event.name.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillRepeatedly(Return(QString(DATA_TO_SEND.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillRepeatedly(Return(QString(LINE_ENDING.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillRepeatedly(ReturnRef(port_open_event.name));
   /* size to write should be 1 byte more because \n is added */
   std::string data_payload = DATA_TO_SEND + '\n';
   EXPECT_CALL(*GUI::PortHandlerMock_get(), write(std::vector<uint8_t>(data_payload.begin(), data_payload.end()), data_payload.size())).WillRepeatedly(Return(true));

   /* writing data to terminal */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillRepeatedly(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr(DATA_TO_SEND))).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillRepeatedly(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillRepeatedly(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _)).Times(AtLeast(1));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr(DATA_TO_SEND))).Times(AtLeast(1));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr(DATA_TO_SEND))).WillRepeatedly(Return(std::optional<uint32_t>()));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(DATA_TO_SEND.c_str()))).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_text_edit, _)).Times(AtLeast(1));
   for (uint8_t i = 0; i < MainApplication::MAX_COMMANDS_HISTORY_ITEMS + 10; i++)
   {
      m_test_subject->onSendButtonClicked();
   }
}

TEST_F(MainApplicationFixture, command_history_reloading)
{
   /**
    * <b>scenario</b>: Two ports opened, data sent to both of them, should be added correctly to history. Then user changed the current port, correct commands should be restored. <br>
    * <b>expected</b>: Newly opened ports added to combobox. <br>
    *                  Initial commands sent successfully to both of them, added to history.<br>
    *                  On current port switch, commands for new port shall be restored to combobox. <br>
    * ************************************************
    */

   const std::string LINE_ENDING = "\\n";
   const std::string EMPTY_PORT_NAME = "";
   GUI::PortHandlerEvent open_port_1;
   open_port_1.name = "PORT_NAME1";
   open_port_1.port_id = 1;
   open_port_1.event = GUI::Event::CONNECTED;
   std::string port1_command1 = "port1_command1";
   std::string port1_command2 = "port1_command2";
   std::string port1_command3 = "port1_command3";
   GUI::PortHandlerEvent open_port_2;
   open_port_2.name = "PORT_NAME2";
   open_port_2.port_id = 2;
   open_port_2.event = GUI::Event::CONNECTED;
   std::string port2_command1 = "port2_command1";
   std::string port2_command2 = "port2_command2";
   std::string port2_command3 = "port2_command3";
   QListWidgetItem terminal_item;

   /* common expectations */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillRepeatedly(Return(QString(LINE_ENDING.c_str())));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(_)).WillRepeatedly(Return(std::optional<uint32_t>()));
   EXPECT_CALL(*GUI::PortHandlerMock_get(), write(_,_)).WillRepeatedly(Return(true));
   EXPECT_CALL(*g_logger_mock, putLog(_)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillRepeatedly(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillRepeatedly(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillRepeatedly(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _)).Times(AtLeast(1));


   /* open two ports */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString(open_port_1.name.c_str())));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(open_port_1);
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString(open_port_2.name.c_str())));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(open_port_2);

   /* start writing data to PORT_NAME1 */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString(open_port_1.name.c_str())))
                                                                           .WillOnce(Return(QString(open_port_1.name.c_str())))
                                                                           .WillOnce(Return(QString(open_port_1.name.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(port1_command1.c_str())))
                                                                            .WillOnce(Return(QString(port1_command2.c_str())))
                                                                            .WillOnce(Return(QString(port1_command3.c_str())));
   /* different name to not match empty name from combobox */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getName(_)).WillRepeatedly(Invoke([&](uint8_t id)->const std::string&
         {
            if (id == 1) return open_port_1.name;
            if (id == 2) return open_port_2.name;
            return EMPTY_PORT_NAME;
         }));


   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command1.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command2.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command3.c_str())));

   /* send commands for PORT_NAME 1*/
   m_test_subject->onSendButtonClicked();
   m_test_subject->onSendButtonClicked();
   m_test_subject->onSendButtonClicked();

   /* user selected the PORT_NAME2 */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_itemText(&test_port_box, 0)).WillOnce(Return(QString(open_port_2.name.c_str())));
   m_test_subject->onCurrentPortSelectionChanged(0);

   /* start writing data to PORT_NAME2 */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_port_box)).WillOnce(Return(QString(open_port_2.name.c_str())))
                                                                           .WillOnce(Return(QString(open_port_2.name.c_str())))
                                                                           .WillOnce(Return(QString(open_port_2.name.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_text_edit)).WillOnce(Return(QString(port2_command1.c_str())))
                                                                            .WillOnce(Return(QString(port2_command2.c_str())))
                                                                            .WillOnce(Return(QString(port2_command3.c_str())));

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port2_command1.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port2_command2.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port2_command3.c_str())));

   /* send commands for PORT_NAME 2*/
   m_test_subject->onSendButtonClicked();
   m_test_subject->onSendButtonClicked();
   m_test_subject->onSendButtonClicked();

   /* user switched back to PORT_NAME1, expect history restoration */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command1.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command2.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString(port1_command3.c_str())));

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_itemText(&test_port_box, 0)).WillOnce(Return(QString(open_port_1.name.c_str())));
   m_test_subject->onCurrentPortSelectionChanged(0);
}


