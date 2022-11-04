#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "PortHandler.h"
#include "PortSettingDialogMock.h"
#include "ITimersMock.h"
#include "SerialDriverMock.h"
#include "ISocketClientMock.h"
#include "Logger.h"

#include "QtWidgets/QtWidgetsMock.h"

Drivers::Serial::ISerialDriverMock* g_serial_mock;
Drivers::SocketClient::ISocketClientMock* g_socket_mock;

std::unique_ptr<Drivers::Serial::ISerialDriver> Drivers::Serial::ISerialDriver::create()
{
   return std::unique_ptr<Drivers::Serial::ISerialDriver>(g_serial_mock);
}

std::unique_ptr<Drivers::SocketClient::ISocketClient> Drivers::SocketFactory::createClient()
{
   return std::unique_ptr<Drivers::SocketClient::ISocketClient>(g_socket_mock);
}


namespace GUI
{

using namespace ::testing;

struct PortHandlerListenerMock : public GUI::PortHandlerListener
{
   MOCK_METHOD1(onPortHandlerEvent, void(const PortHandlerEvent&));
};

constexpr uint32_t BUTTON_OFF_COLOR = 0xFF0000;
constexpr uint32_t BUTTON_ON_COLOR = 0x00FF00;
constexpr uint32_t BUTTON_CONNECTING_COLOR = 0x0000FF;

constexpr int TEST_TIMER_ID = 1;

struct PortHandlerFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      PortSettingDialogMock_init();
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&test_button, Qt::ContextMenuPolicy::CustomContextMenu));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, HasSubstr("PORT")));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setAutoFillBackground(&test_label, true));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setAlignment(&test_label, Qt::AlignCenter));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("clicked"), _, HasSubstr("onPortButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onPortButtonContextMenuRequested")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(_, HasSubstr("portEvent"), _, HasSubstr("onPortEvent")));
      EXPECT_CALL(timer_mock, createTimer(_,_)).WillOnce(Return(TEST_TIMER_ID));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, 0)));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_OFF_COLOR))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
      EXPECT_CALL(listener_mock, onPortHandlerEvent(_));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, AllOf(HasSubstr("PORT"), HasSubstr("//dev/ttyUSB1/SER/BR_115200"))));

      EXPECT_CALL(*g_socket_mock, addListener(_));
      EXPECT_CALL(*g_serial_mock, addListener(_));
      m_test_subject.reset(new PortHandler(&test_button, &test_label, timer_mock, &listener_mock, nullptr, fake_persistence));

   }
   void TearDown()
   {
      EXPECT_CALL(timer_mock, removeTimer(TEST_TIMER_ID));
      EXPECT_CALL(*g_socket_mock, removeListener(_));
      EXPECT_CALL(*g_serial_mock, removeListener(_));
      EXPECT_CALL(*g_socket_mock, disconnect());
      EXPECT_CALL(*g_serial_mock, close());
      m_test_subject.reset(nullptr);
      PortSettingDialogMock_deinit();
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }

   PortHandlerListenerMock listener_mock;
   Utilities::ITimersMock timer_mock;
   std::unique_ptr<PortHandler> m_test_subject;
   QPushButton test_button;
   QLabel test_label;
   Persistence::PersistenceHandler fake_persistence;
};

/* implementation of signal emited on port handler event */
void PortHandler::portEvent()
{
   onPortEvent();
}

TEST_F(PortHandlerFixture, empty_commands_list_when_execution_requested)
{
   /**
    * <b>scenario</b>: User requested to send commands, but no commands entered by user <br>
    * <b>expected</b>: Button should be deactivated on command execution start.<br>
    *                  No commands sent <br>
    *                  Button should be activated on command execution finish. <br>
    * ************************************************
    */

}

}
