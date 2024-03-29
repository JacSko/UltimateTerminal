#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "MainApplication.h"
#include "PortMock.h"
#include "UserButtonsTabMock.h"
#include "TraceFilterMock.h"
#include "LoggingSettingDialogMock.h"
#include "ITimersMock.h"
#include "PersistenceMock.h"
#include "PortSettingDialogMock.h"
#include "TraceFilterSettingDialogMock.h"
#include "ApplicationSettingsDialogMock.h"
#include "MessageDialogMock.h"
#include "IFileLoggerMock.h"
#include "GUIControllerMock.h"
#include "Settings.h"

Utilities::ITimersMock* g_timers_mock;
std::unique_ptr<Utilities::ITimers> Utilities::ITimersFactory::create()
{
   return std::unique_ptr<ITimers>(g_timers_mock);
}


namespace MainApplication
{

using namespace ::testing;

IFileLoggerMock* g_logger_mock;
std::unique_ptr<IFileLogger> IFileLogger::create()
{
   return std::unique_ptr<IFileLogger>(g_logger_mock);
}

namespace system_call
{
__attribute__((weak)) std::string getExecutablePath()
{
   return "";
}
}

struct MainApplicationFixture : public testing::Test
{
   const uint32_t MARKER_BUTTON_ID = 0x01;
   const uint32_t LOGGING_BUTTON_ID = 0x02;
   const uint32_t SETTING_BUTTON_ID = 0x03;
   const uint32_t SEND_BUTTON_ID = 0x04;
   const uint32_t SCROLL_BOTTOM_BUTTON_ID = 0x05;
   const uint32_t CLEAR_BUTTON_ID = 0x06;
   const uint32_t TRACE_CLEAR_BUTTON_ID = 0x07;
   const uint32_t TRACE_SCROLL_BOTTOM_BUTTON_ID = 0x08;
   const uint32_t DEFAULT_BACKGROUND_COLOR = 0xFFFFFF;
   const uint32_t DEFAULT_FONT_COLOR = 0xFFFFFF;
   const uint32_t GREEN_COLOR = 0x00FF00;
   const uint32_t BLACK_COLOR = 0x000000;
   const uint8_t PORTS_COUNT = 5;
   const uint8_t TRACE_FILTERS_COUNT = 7;
   const uint8_t BUTTONS_TABS = 1;
   const uint8_t BUTTONS_ROWS_PER_TAB = 2;
   const uint8_t BUTTONS_PER_ROW = 5;
   const uint16_t BUTTONS_COUNT = BUTTONS_TABS * BUTTONS_ROWS_PER_TAB * BUTTONS_PER_ROW;

   void SetUp()
   {
      SETTING_SET_U32(GUI_UserButtons_Tabs, BUTTONS_TABS);
      SETTING_SET_U32(GUI_UserButtons_RowsPerTab, BUTTONS_ROWS_PER_TAB);
      SETTING_SET_U32(GUI_UserButtons_ButtonsPerRow, BUTTONS_PER_ROW);
      m_button_listener = nullptr;

      PortMock_init();
      TraceFilterMock_init();
      LoggingSettingDialogMock_init();
      Utilities::Persistence::PersistenceMock_init();
      ApplicationSettingsDialogMock_init();
      GUIControllerMock_init();
      MessageDialogMock_init();

      g_timers_mock = new Utilities::ITimersMock;
      g_logger_mock = new IFileLoggerMock;

      EXPECT_CALL(*g_timers_mock, start());
      EXPECT_CALL(*Utilities::Persistence::PersistenceMock_get(), loadFile(_)).WillOnce(Return(true));
      EXPECT_CALL(*Utilities::Persistence::PersistenceMock_get(), restore());
      EXPECT_CALL(*Utilities::Persistence::PersistenceMock_get(), clearFile());
      EXPECT_CALL(*GUIControllerMock_get(), run());
      EXPECT_CALL(*GUIControllerMock_get(), reloadTheme(_));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForThemeReloadEvent(_)).WillOnce(SaveArg<0>(&m_theme_listener));
      EXPECT_CALL(*GUIControllerMock_get(), setApplicationTitle(_));
      EXPECT_CALL(*GUIControllerMock_get(), setInfoLabelText(_));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("markerButton")).WillOnce(Return(MARKER_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("loggingButton")).WillOnce(Return(LOGGING_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("settingsButton")).WillOnce(Return(SETTING_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("sendButton")).WillOnce(Return(SEND_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("bottomButton")).WillOnce(Return(SCROLL_BOTTOM_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("clearButton")).WillOnce(Return(CLEAR_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("traceClearButton")).WillOnce(Return(TRACE_CLEAR_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID("traceBottomButton")).WillOnce(Return(TRACE_SCROLL_BOTTOM_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(MARKER_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(SETTING_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(SCROLL_BOTTOM_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TRACE_CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TRACE_SCROLL_BOTTOM_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, _));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, _));
      EXPECT_CALL(*GUIControllerMock_get(), addLineEnding("\\r\\n"));
      EXPECT_CALL(*GUIControllerMock_get(), addLineEnding("\\n"));
      EXPECT_CALL(*GUIControllerMock_get(), addLineEnding("EMPTY"));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForActivePortChangedEvent(_)).WillOnce(SaveArg<0>(&m_port_change_listener));
      EXPECT_CALL(*GUIControllerMock_get(), countTabs()).WillOnce(Return(BUTTONS_TABS));
      EXPECT_CALL(*GUIControllerMock_get(), countButtonsPerTab()).WillRepeatedly(Return(0));
      EXPECT_CALL(*GUIControllerMock_get(), countPorts()).WillOnce(Return(PORTS_COUNT));
      EXPECT_CALL(*GUIControllerMock_get(), countTraceFilters()).WillOnce(Return(TRACE_FILTERS_COUNT));

      m_test_subject = std::unique_ptr<MainApplication>(new MainApplication());

      ASSERT_TRUE(m_button_listener);
      ASSERT_TRUE(m_port_change_listener);
      ASSERT_TRUE(m_theme_listener);

      Mock::VerifyAndClearExpectations(g_timers_mock);
      Mock::VerifyAndClearExpectations(GUIControllerMock_get());
      Mock::VerifyAndClearExpectations(PortMock_get());
      Mock::VerifyAndClearExpectations(TraceFilterMock_get());
      Mock::VerifyAndClearExpectations(LoggingSettingDialogMock_get());
      Mock::VerifyAndClearExpectations(Utilities::Persistence::PersistenceMock_get());
   }


   void TearDown()
   {
      EXPECT_CALL(*Utilities::Persistence::PersistenceMock_get(), save(_)).WillOnce(Return(true));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(MARKER_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(SETTING_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(SCROLL_BOTTOM_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TRACE_CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TRACE_SCROLL_BOTTOM_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromThemeReloadEvent(_));
      EXPECT_CALL(*g_timers_mock, stop());

      m_test_subject.reset(nullptr);
      MessageDialogMock_deinit();
      GUIControllerMock_deinit();
      ApplicationSettingsDialogMock_deinit();
      Utilities::Persistence::PersistenceMock_deinit();
      LoggingSettingDialogMock_deinit();
      TraceFilterMock_deinit();
      PortMock_deinit();
   }
   std::unique_ptr<MainApplication> m_test_subject;
   GUIController::ButtonEventListener* m_button_listener;
   GUIController::ThemeListener* m_theme_listener;
   std::function<bool(const std::string&)> m_port_change_listener;
};

TEST_F(MainApplicationFixture, adding_data_from_port_handler_to_main_terminal)
{
   /**
    * <b>scenario</b>: Port  reports opened terminal and start sending data <br>
    * <b>expected</b>: Data shall be added to terminal, but without trailing newline character. <br>
    *                  Check for trace matching shall be made. <br>
    *                  Window shall be scrolled to bottom. <br>
    *                  Check for maximum trace count shall be made. <br>
    * ************************************************
    */
   constexpr uint32_t TEST_BACKGROUND_COLOR = 0x123321;
   constexpr uint32_t TEST_FONT_COLOR = 0xAABBCC;
   const uint32_t TEST_MAX_TRACE_NUMBER = SETTING_GET_U32(MainApplication_maxTerminalTraces);
   PortEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = Event::CONNECTED;
   QListWidgetItem terminal_item;
   PortEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.port_id = 2;
   port_data_event.event = Event::NEW_DATA;
   port_data_event.background_color = TEST_BACKGROUND_COLOR;
   port_data_event.font_color = TEST_FONT_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   PortEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = Event::DISCONNECTED;

   EXPECT_CALL(*GUIControllerMock_get(), registerPortOpened("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_open_event);

   /* adding item with newline at the end */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("some text"), port_data_event.background_color, port_data_event.font_color));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_data_event);

   /* adding item without newline at the end */
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t'};
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("some text"), port_data_event.background_color, port_data_event.font_color));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_data_event);

   /* items count exceeded the maximum count for terminal view, terminal shall be cleaned */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(TEST_MAX_TRACE_NUMBER));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("some text"), port_data_event.background_color, port_data_event.font_color));
   EXPECT_CALL(*GUIControllerMock_get(), clearTerminalView());
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_data_event);


   EXPECT_CALL(*GUIControllerMock_get(), registerPortClosed("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_close_event);

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
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0000; //marker shall be red
   constexpr uint32_t TEST_TEXT_COLOR = 0x000000;   //text color shall be inherited from parent

   /* first marker added */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(TEST_TEXT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("MARKER1"), TEST_MARKER_COLOR, TEST_TEXT_COLOR));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER1")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("MARKER1"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   m_button_listener->onButtonEvent(MARKER_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* second marker added */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(TEST_TEXT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("MARKER2"), TEST_MARKER_COLOR, TEST_TEXT_COLOR));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER2")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("MARKER2"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   m_button_listener->onButtonEvent(MARKER_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
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
   constexpr uint32_t TEST_BACKGROUND_COLOR = 0x123321;
   constexpr uint32_t TEST_FONT_COLOR = 0xAABBCC;
   constexpr uint32_t TEST_MARKER_COLOR = 0xFF0000; //marker shall be red
   PortEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = Event::CONNECTED;
   QListWidgetItem terminal_item;
   PortEvent port_data_event;
   port_data_event.name = "PORT_NAME";
   port_data_event.port_id = 2;
   port_data_event.event = Event::NEW_DATA;
   port_data_event.background_color = TEST_BACKGROUND_COLOR;
   port_data_event.font_color = TEST_FONT_COLOR;
   port_data_event.data = {'s','o','m','e',' ','t','e','x','t','\n'};
   PortEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = Event::DISCONNECTED;
   std::string log_path = "/home/user";

   /* logging button colors change */
   EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(DEFAULT_BACKGROUND_COLOR))
                                                              .WillOnce(Return(DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(DEFAULT_FONT_COLOR)) // setting logging button to green
                                                        .WillOnce(Return(TEST_FONT_COLOR))    // writing marker to terminal
                                                        .WillOnce(Return(DEFAULT_FONT_COLOR));// setting logging button to default

   /* port opening */
   EXPECT_CALL(*GUIControllerMock_get(), registerPortOpened("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_open_event);

   /* user requested to change the logging file */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,true)).WillOnce(Return(log_path));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* requesting file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*g_logger_mock, openFile(HasSubstr(log_path + "/log"))).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), setStatusBarNotification(HasSubstr(log_path + "/log"), SETTING_GET_U32(MainApplication_statusBarTimeout)));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, BLACK_COLOR));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* first marker added */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("MARKER1"), TEST_MARKER_COLOR, TEST_FONT_COLOR));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("MARKER1")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("MARKER1"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   m_button_listener->onButtonEvent(MARKER_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* adding item with newline at the end */
   EXPECT_CALL(*GUIControllerMock_get(), countTerminalItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), countTraceItems()).WillOnce(Return(0));
   EXPECT_CALL(*GUIControllerMock_get(), addToTerminalView(HasSubstr("some text"), TEST_BACKGROUND_COLOR, TEST_FONT_COLOR));
   EXPECT_CALL(*g_logger_mock, putLog(HasSubstr("some text")));
   EXPECT_CALL(*TraceFilterMock_get(), tryMatch(HasSubstr("some text"))).WillRepeatedly(Return(std::optional<Dialogs::TraceFilterSettingDialog::Settings>()));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_data_event);

   /* user requested to change the logging file, it should not be possible when logging is active */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,false)).WillOnce(Return(std::optional<std::string>()));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* closing port */
   EXPECT_CALL(*GUIControllerMock_get(), registerPortClosed("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_close_event);

   /* closing file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*g_logger_mock, closeFile());
   EXPECT_CALL(*GUIControllerMock_get(), setStatusBarNotification(_, SETTING_GET_U32(MainApplication_statusBarTimeout)));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
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
   PortEvent port_open_event;
   port_open_event.name = "PORT_NAME";
   port_open_event.port_id = 2;
   port_open_event.event = Event::CONNECTED;
   QListWidgetItem terminal_item;
   PortEvent port_close_event;
   port_close_event.name = "PORT_NAME";
   port_close_event.port_id = 2;
   port_close_event.event = Event::DISCONNECTED;
   std::string log_path = "/home/user";

   /* logging button colors change */
   EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(DEFAULT_BACKGROUND_COLOR))
                                                              .WillOnce(Return(DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(DEFAULT_FONT_COLOR)) // setting logging button to green
                                                        .WillOnce(Return(DEFAULT_FONT_COLOR));// setting logging button to default


   /* user requested to change the logging file */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,true)).WillOnce(Return(log_path));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* requesting file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*g_logger_mock, openFile(HasSubstr(log_path + "/log"))).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), setStatusBarNotification(HasSubstr(log_path + "/log"), SETTING_GET_U32(MainApplication_statusBarTimeout)));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, BLACK_COLOR));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* port opening */
   EXPECT_CALL(*GUIControllerMock_get(), registerPortOpened("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_open_event);

   /* closing port */
   EXPECT_CALL(*GUIControllerMock_get(), registerPortClosed("PORT_NAME"));
   ((PortListener*)m_test_subject.get())->onPortEvent(port_close_event);

   /* closing file logging */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*g_logger_mock, closeFile());
   EXPECT_CALL(*GUIControllerMock_get(), setStatusBarNotification(_, SETTING_GET_U32(MainApplication_statusBarTimeout)));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(MainApplicationFixture, terminal_and_trace_view_clearing)
{
   /**
    * <b>scenario</b>: User requested to clear trace view and terminal view. <br>
    * <b>expected</b>: clear() method called on terminal view. <br>
    *                  clear() method called on trace view. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), clearTerminalView());
   m_button_listener->onButtonEvent(CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   EXPECT_CALL(*GUIControllerMock_get(), clearTraceView());
   m_button_listener->onButtonEvent(TRACE_CLEAR_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

}

TEST_F(MainApplicationFixture, sending_data_to_port_no_port_opened)
{
   /**
    * <b>scenario</b>: No port opened, data send requested by user. <br>
    * <b>expected</b>: Port shall be asked about write <br>
    * ************************************************
    */
   const std::string PORT_NAME = "NAME1";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "\n";

   EXPECT_CALL(*GUIControllerMock_get(), getCurrentCommand()).WillOnce(Return(DATA_TO_SEND));
   EXPECT_CALL(*GUIControllerMock_get(), getCurrentLineEnding()).WillOnce(Return(LINE_ENDING));
   EXPECT_CALL(*GUIControllerMock_get(), clearCommand());

   /* different name to not match empty name from combobox */
   EXPECT_CALL(*PortMock_get(), getName(_)).WillRepeatedly(ReturnRef(PORT_NAME));
   EXPECT_CALL(*PortMock_get(), write(_,_)).WillOnce(Return(false));
   m_button_listener->onButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(MainApplicationFixture, sending_data_to_port)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  Ports shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Data shall be added to terminal view. <br>
    * ************************************************
    */
   const std::string PORT_NAME = "NAME1";
   const std::string INCORRECT_PORT_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "\\n";

   EXPECT_CALL(*GUIControllerMock_get(), getCurrentCommand()).WillOnce(Return(DATA_TO_SEND));
   EXPECT_CALL(*GUIControllerMock_get(), getCurrentLineEnding()).WillOnce(Return(LINE_ENDING));

   /* simulate user action that changing the current port */
   EXPECT_CALL(*PortMock_get(), getName(_)).WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*GUIControllerMock_get(), setCommandsHistory(_)).Times(2); //second time when writing to terminal
   m_port_change_listener(PORT_NAME);
   Mock::VerifyAndClearExpectations(PortMock_get());

   /* size to write should be 1 byte more because \n is added */
   std::string data_payload = DATA_TO_SEND + '\n';
   EXPECT_CALL(*PortMock_get(), getName(_)).WillOnce(ReturnRef(PORT_NAME))
                                           .WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*PortMock_get(), write(std::vector<uint8_t>(data_payload.begin(), data_payload.end()), data_payload.size())).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), clearCommand());

   m_button_listener->onButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(MainApplicationFixture, sending_data_to_port_empty_line_ending)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user with empty line ending. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  Ports shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Data shall be added to terminal view. <br>
    * ************************************************
    */
   const std::string PORT_NAME = "NAME1";
   const std::string INCORRECT_PORT_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "EMPTY";

   EXPECT_CALL(*GUIControllerMock_get(), getCurrentCommand()).WillOnce(Return(DATA_TO_SEND));
   EXPECT_CALL(*GUIControllerMock_get(), getCurrentLineEnding()).WillOnce(Return(LINE_ENDING));

   /* simulate user action that changing the current port */
   EXPECT_CALL(*PortMock_get(), getName(_)).WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*GUIControllerMock_get(), setCommandsHistory(_)).Times(2); //second time when writing to terminal
   m_port_change_listener(PORT_NAME);
   EXPECT_CALL(*PortMock_get(), getName(_)).WillOnce(ReturnRef(PORT_NAME))
                                           .WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*PortMock_get(), write(std::vector<uint8_t>(DATA_TO_SEND.begin(), DATA_TO_SEND.end()), DATA_TO_SEND.size())).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), clearCommand());

   m_button_listener->onButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(MainApplicationFixture, sending_data_to_port_failed)
{
   /**
    * <b>scenario</b>: Port opened, data send requested by user with empty line ending, but write to port fails. <br>
    * <b>expected</b>: Current port name shall be read from combobox. <br>
    *                  Ports shall be asked about names. <br>
    *                  Data shall be sent to correct handler. <br>
    *                  Error information shall be added to terminal. <br>
    * ************************************************
    */
   const std::string PORT_NAME = "NAME1";
   const std::string INCORRECT_PORT_NAME = "NAMEX";
   const std::string DATA_TO_SEND = "some command to send";
   const std::string LINE_ENDING = "EMPTY";

   EXPECT_CALL(*GUIControllerMock_get(), getCurrentCommand()).WillOnce(Return(DATA_TO_SEND));
   EXPECT_CALL(*GUIControllerMock_get(), getCurrentLineEnding()).WillOnce(Return(LINE_ENDING));

   /* simulate user action that changing the current port */
   EXPECT_CALL(*PortMock_get(), getName(_)).WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*GUIControllerMock_get(), setCommandsHistory(_));
   m_port_change_listener(PORT_NAME);
   EXPECT_CALL(*PortMock_get(), getName(_)).WillOnce(ReturnRef(PORT_NAME))
                                                      .WillRepeatedly(ReturnRef(INCORRECT_PORT_NAME));
   EXPECT_CALL(*PortMock_get(), write(std::vector<uint8_t>(DATA_TO_SEND.begin(), DATA_TO_SEND.end()), DATA_TO_SEND.size())).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), clearCommand());

   m_button_listener->onButtonEvent(SEND_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(MainApplicationFixture, persistence_write_and_read)
{
   /**
    * <b>scenario</b>: Persistence write and read sequence to verify data serialization <br>
    * <b>expected</b>: Newly created object shall correctly process the persistence data.<br>
    * ************************************************
    */
   Utilities::Persistence::PersistenceListener::PersistenceItems data_buffer;
   std::string log_path = "/home/user";
   Theme theme = Theme::DARK;
   const std::string LINE_ENDING = "\\n";

   EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillRepeatedly(Return(DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillRepeatedly(Return(DEFAULT_FONT_COLOR));

   /* replace all persistent data, to get known state */
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,true)).WillOnce(Return(log_path));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);
   EXPECT_CALL(*GUIControllerMock_get(), getCurrentLineEnding()).WillOnce(Return(LINE_ENDING));
   EXPECT_CALL(*GUIControllerMock_get(), currentTheme()).WillOnce(Return(theme));
   ASSERT_EQ(data_buffer.size(), 0);
   ((Utilities::Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));

   // change object state to verify data restoration
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*LoggingSettingDialogMock_get(), showDialog(_,_,true)).WillOnce(Return(""));
   m_button_listener->onButtonEvent(LOGGING_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);

   //restore
   EXPECT_CALL(*GUIControllerMock_get(), reloadTheme(theme));
   EXPECT_CALL(*GUIControllerMock_get(), setCurrentLineEnding(LINE_ENDING));
   ((Utilities::Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceRead(data_buffer);

}

TEST_F(MainApplicationFixture, theme_reloading)
{
   /**
    * <b>scenario</b>: Theme reload event received, file logging not activated. <br>
    * <b>expected</b>: Logging button shall be refreshed. <br>
    *                  All ports shall be refreshed. <br>
    *                  All trace filters shall be refreshed. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(DEFAULT_FONT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), themeToName(_)).WillOnce(Return(""));
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(LOGGING_BUTTON_ID, DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(LOGGING_BUTTON_ID, DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortMock_get(), refreshUi()).Times(PORTS_COUNT);
   EXPECT_CALL(*TraceFilterMock_get(), refreshUi()).Times(TRACE_FILTERS_COUNT);
   m_theme_listener->onThemeChange(Theme::DEFAULT);


   /**
    * <b>scenario</b>: Theme reload event received, file logging activated. <br>
    * <b>expected</b>: All port shall be refreshed. <br>
    *                  All trace filters shall be refreshed. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), themeToName(_)).WillOnce(Return(""));
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(true));
   EXPECT_CALL(*PortMock_get(), refreshUi()).Times(PORTS_COUNT);
   EXPECT_CALL(*TraceFilterMock_get(), refreshUi()).Times(TRACE_FILTERS_COUNT);
   m_theme_listener->onThemeChange(Theme::DEFAULT);

}

}
