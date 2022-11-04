#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include <thread>
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

struct TestParam
{
   PortSettingDialog::PortType type;
};

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

struct PortHandlerParamFixture : public PortHandlerFixture, public ::testing::WithParamInterface<TestParam>
{

};

/* implementation of signal emited on port handler event */
void PortHandler::portEvent()
{}

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

TEST_P(PortHandlerParamFixture, settings_change_and_port_connection)
{
   /**
    * <b>scenario</b>: User changed the connection settings, request the connection and starts data exchange, then closed.<br>
    * <b>expected</b>: Connection to port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    *                  Data send and read correctly. <br>
    *                  Port closed correctly.<br>
    *                  Button color shall be changed to red. <br>
    *                  Notifications shall be sent in correct order. <br>
    * ************************************************
    */

   GUI::PortHandlerEvent receivied_event;
   PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   /* settings change requested */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, QString(user_settings.shortSettingsString().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setStyleSheet(&test_label,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(user_settings.port_name.c_str())));
   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
      EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   }
   else
   {
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
      EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
      EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
   }
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_ON_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

   /* data received from serial port */
   const std::vector<uint8_t> event_data = {'a', 'b', 'c', '\r', '\n', 'd', 'e', 'f', 'n'};
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));

   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      Drivers::Serial::DriverEvent event = Drivers::Serial::DriverEvent::DATA_RECV;
      ((Drivers::Serial::SerialListener*)m_test_subject.get())->onSerialEvent(event, event_data, event_data.size());
   }
   else
   {
      Drivers::SocketClient::ClientEvent event = Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV;
      ((Drivers::SocketClient::ClientListener*)m_test_subject.get())->onClientEvent(event, event_data, event_data.size());
   }
   m_test_subject->onPortEvent();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::NEW_DATA);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_THAT(receivied_event.data, ContainerEq(event_data));
   EXPECT_EQ(receivied_event.size, event_data.size());

   /* data send to port */
   const std::vector<uint8_t> data_to_write (event_data.rbegin(), event_data.rend());
   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
      EXPECT_CALL(*g_serial_mock, write(_,_)).WillOnce(Return(true));
   }
   else
   {
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(true));
      EXPECT_CALL(*g_socket_mock, write(_,_)).WillOnce(Return(true));
   }
   m_test_subject->write(data_to_write, data_to_write.size());

   /* closing serial port */
   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
      EXPECT_CALL(*g_serial_mock, close());
   }
   else
   {
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(true));
      EXPECT_CALL(*g_socket_mock, disconnect());
   }
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_ON_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_OFF_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

}

TEST_P(PortHandlerParamFixture, settings_change_when_port_is_opened)
{
   /**
    * <b>scenario</b>: Port opened but user requested to change the settings.<br>
    * <b>expected</b>: Connection to port shall be requested. <br>
    *                  Button color shall be changed to green.
    *                  Setting dialog shall be opened but without possibility to edit settings.<br>
    *                  Port correctly closed.<br>
    *                  Setting dialog should be opened with possibility to edit.<br>
    * ************************************************
    */

   GUI::PortHandlerEvent receivied_event;
   PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   /* settings change requested */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, QString(user_settings.shortSettingsString().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setStyleSheet(&test_label,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(user_settings.port_name.c_str())));
   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
      EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   }
   else
   {
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
      EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
      EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
   }
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_ON_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

   /* request to open settings dialog */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,false));
   m_test_subject->onPortButtonContextMenuRequested();

   /* closing serial port */
   if (user_settings.type == PortSettingDialog::PortType::SERIAL)
   {
      EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
      EXPECT_CALL(*g_serial_mock, close());
   }
   else
   {
      EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(true));
      EXPECT_CALL(*g_socket_mock, disconnect());
   }
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, BUTTON_ON_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_OFF_COLOR))));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

   /* request to open settings dialog */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true));
   m_test_subject->onPortButtonContextMenuRequested();
}

TEST_F(PortHandlerFixture, cannot_open_serial_port)
{
   /**
    * <b>scenario</b>: Serial port open requested, but it is not possible.<br>
    * <b>expected</b>: Button color shall not change. <br>
    * ************************************************
    */

   PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;

   /* settings change requested */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, QString(user_settings.shortSettingsString().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setStyleSheet(&test_label,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(user_settings.port_name.c_str())));
   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(false));
   m_test_subject->onPortButtonClicked();

}

TEST_F(PortHandlerFixture, cannot_connect_to_socket_server)
{
   /**
    * <b>scenario</b>: Connect to server requested, but it is not possible.<br>
    * <b>expected</b>: Socket connection requested. <br>
    *                  Button color changed to CONNECTING <br>
    *                  Connection attempts shall be executed until success. <br>
    *                  On success, button color shall be changed to CONNECTED. <br>
    *                  Notifications shall be sent in correct order. <br>
    * ************************************************
    */

   PortHandlerEvent receivied_event;
   PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool timer_running = true;

   /* settings change requested */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, QString(user_settings.shortSettingsString().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setStyleSheet(&test_label,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(user_settings.port_name.c_str())));
   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));

   EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
   EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(true)); //finally connected to server

   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID)).WillOnce(Invoke([&](int)
         {
            timer_running = false;
         }));

   /* expect button color change while trying to connect */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillRepeatedly(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_CONNECTING_COLOR)))).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button)).Times(AtLeast(1));

   /* expect button color change on successfull connection */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_ON_COLOR))));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

   while(timer_running)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

}

TEST_F(PortHandlerFixture, aborting_connection_trials)
{
   /**
    * <b>scenario</b>: Connect to server requested, but it is not possible. Retries started, but aborted by user. <br>
    * <b>expected</b>: Socket connection requested. <br>
    *                  Button color changed to CONNECTING <br>
    *                  Connection attempts shall be executed until aborted. <br>
    *                  After abort, button color shall change to DISCONNECTED. <br>
    *                  Notifications shall be sent in correct order. <br>
    * ************************************************
    */

   PortHandlerEvent receivied_event;
   PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool simulate_abort = false;

   /* settings change requested */
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_label, QString(user_settings.shortSettingsString().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setStyleSheet(&test_label,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(user_settings.port_name.c_str())));
   m_test_subject->onPortButtonContextMenuRequested();

   /* port opening */
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));

   EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false))
                                             .WillOnce(Return(false));
   EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Invoke([&](Drivers::SocketClient::DataMode, std::string, uint16_t)
                                                                                            {
                                                                                                simulate_abort = true;
                                                                                                return false;
                                                                                            }));

   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));

   /* expect button color change while trying to connect */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillRepeatedly(Return(QPalette(QPalette::ColorRole::Button, BUTTON_OFF_COLOR)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_CONNECTING_COLOR)))).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button)).Times(AtLeast(1));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

   while(!simulate_abort)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }

   /* expect button color change on abort */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, QColor(BUTTON_OFF_COLOR))));
   m_test_subject->onPortButtonClicked();

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
   EXPECT_EQ(receivied_event.size, 0);

}

TestParam params[] = {{PortSettingDialog::PortType::SERIAL},{PortSettingDialog::PortType::ETHERNET}};
INSTANTIATE_TEST_CASE_P(PortHandlerParamFixture, PortHandlerParamFixture, ValuesIn(params));

}

