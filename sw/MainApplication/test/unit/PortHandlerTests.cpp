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

      Mock::VerifyAndClearExpectations(g_socket_mock);
      Mock::VerifyAndClearExpectations(g_serial_mock);
      Mock::VerifyAndClearExpectations(QtWidgetsMock_get());
      Mock::VerifyAndClearExpectations(QtCoreMock_get());
      Mock::VerifyAndClearExpectations(&timer_mock);
      Mock::VerifyAndClearExpectations(&listener_mock);
   }
   void TearDown()
   {
      EXPECT_CALL(timer_mock, removeTimer(TEST_TIMER_ID));
      EXPECT_CALL(*g_socket_mock, removeListener(_));
      EXPECT_CALL(*g_serial_mock, removeListener(_));
      EXPECT_CALL(*g_socket_mock, disconnect());
      EXPECT_CALL(*g_serial_mock, close());
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_ON_COLOR)));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_OFF_COLOR))));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
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

TEST_F(PortHandlerFixture, connecting_with_default_settings)
{
   /**
    * <b>scenario</b>: User requested to connect, but the port settings are default <br>
    * <b>expected</b>: Connection to serial port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    * ************************************************
    */

   GUI::PortHandlerEvent receivied_event;

   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_ON_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));

   m_test_subject->onPortButtonClicked();

   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("PORT"));
   EXPECT_EQ(receivied_event.size, 0);
}

TEST_F(PortHandlerFixture, settings_change_and_serial_port_connection)
{
   /**
    * <b>scenario</b>: User changed the connection settings, request the connection and starts data exchange, then closed.<br>
    * <b>expected</b>: Connection to serial port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    * ************************************************
    */

   GUI::PortHandlerEvent receivied_event;
   PortSettingDialog::Settings user_settings;
   user_settings.type = PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   /* settings change requested */

   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_ON_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));

   m_test_subject->onPortButtonClicked();

   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("PORT"));
   EXPECT_EQ(receivied_event.size, 0);
}

}
