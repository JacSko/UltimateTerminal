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

using namespace ::testing;

Utilities::ITimersMock* g_timers_mock;

std::unique_ptr<Utilities::ITimers> Utilities::ITimersFactory::create()
{
   return std::unique_ptr<ITimers>(g_timers_mock);
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
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(_)).WillRepeatedly(Return(QPalette(QPalette::ColorRole::Button, 0)));

      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_logging_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::red))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_logging_button));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_scroll_button, QPalette(QPalette::ColorRole::Button, QColor(Qt::green))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_scroll_button));


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



TEST_F(MainApplicationFixture, test)
{
   /**
    * <b>scenario</b>: User requested to connect, but the port settings are default <br>
    * <b>expected</b>: Connection to serial port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    * ************************************************
    */
}

