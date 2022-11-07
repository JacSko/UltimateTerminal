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
                                                         .WillOnce(Return(&test_user_button_3))
                                                         .WillOnce(Return(&test_user_button_4))
                                                         .WillOnce(Return(&test_user_button_5))
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
                                                        .WillOnce(Return(&test_line_ending_box));
      EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_new()).WillOnce(Return(&test_terminal_view))
                                                          .WillOnce(Return(&test_trace_view));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_text_edit))
                                                        .WillOnce(Return(&test_trace_filter_2))
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


      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_marker_button, HasSubstr("clicked"), _, HasSubstr("onMarkerButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_clear_button, HasSubstr("clicked"), _, HasSubstr("onClearButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_trace_clear_button, HasSubstr("clicked"), _, HasSubstr("onTraceClearButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_send_button, HasSubstr("clicked"), _, HasSubstr("onSendButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_text_edit, HasSubstr("returnPressed"), _, HasSubstr("onSendButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_logging_button, HasSubstr("clicked"), _, HasSubstr("onLoggingButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_logging_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onLoggingButtonContextMenuRequested")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_scroll_button, HasSubstr("clicked"), _, HasSubstr("onScrollButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_trace_scroll_button, HasSubstr("clicked"), _, HasSubstr("onTraceScrollButtonClicked")));

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
   QListWidget test_terminal_view;
   QListWidget test_trace_view;
   QLineEdit test_text_edit;
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
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   port_data_event.size = port_data_event.data.size();
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.event = GUI::Event::DISCONNECTED;

   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT_NAME")));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_open_event);

   /* adding item with newline at the end */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
//   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_terminal_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_count(&test_trace_view)).WillOnce(Return(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_terminal_view, _));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<uint32_t>()));
   ((GUI::PortHandlerListener*)m_test_subject.get())->onPortHandlerEvent(port_data_event);

   /* adding item without newline at the end */
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t'};
   port_data_event.size = port_data_event.data.size();
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&terminal_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&terminal_item, HasSubstr("some text")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&terminal_item, QColor(TEST_TRACE_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_terminal_view));
   //   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
   //   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
//   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
//   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.event = GUI::Event::NEW_DATA;
   port_data_event.trace_color = TEST_TRACE_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   port_data_event.size = port_data_event.data.size();
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
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
//   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
//   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_scrollToBottom(&test_trace_view)); TODO
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
   port_open_event.event = GUI::Event::CONNECTED;
   QListWidgetItem terminal_item;
   GUI::PortHandlerEvent port_close_event;
   port_close_event.name = "PORT_NAME";
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

