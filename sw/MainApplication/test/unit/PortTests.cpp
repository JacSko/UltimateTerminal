#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include <thread>
#include "Port.h"
#include "PortSettingDialogMock.h"
#include "MessageDialogMock.h"
#include "ITimersMock.h"
#include "SerialDriverMock.h"
#include "ISocketClientMock.h"
#include "Logger.h"
#include "GUIControllerMock.h"
#include "QtWidgets/QtWidgetsMock.h"

Drivers::Serial::ISerialDriverMock* g_serial_mock;
Drivers::SocketClient::ISocketClientMock* g_socket_mock;

std::unique_ptr<Drivers::Serial::ISerialDriver> Drivers::Serial::ISerialDriver::create()
{
   return std::unique_ptr<Drivers::Serial::ISerialDriver>(g_serial_mock);
}

std::unique_ptr<Drivers::SocketClient::ISocketClient> Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode)
{
   return std::unique_ptr<Drivers::SocketClient::ISocketClient>(g_socket_mock);
}


namespace MainApplication
{

using namespace ::testing;

struct PortListenerMock : public PortListener
{
   MOCK_METHOD1(onPortEvent, void(const PortEvent&));
};

constexpr uint32_t GREEN_COLOR = 0x00FF00;
constexpr uint32_t BLUE_COLOR = 0x0000FF;
constexpr uint32_t BLACK_COLOR = 0x000000;
constexpr uint32_t BUTTON_DEFAULT_BACKGROUND_COLOR = 0x111111;
constexpr uint32_t BUTTON_DEFAULT_FONT_COLOR = 0x222222;

constexpr int TEST_TIMER_ID = 1;
constexpr int THROUGHPUT_TIMER_ID = 2;
const std::string TEST_BUTTON_NAME = "BUTTON_NAME";
constexpr uint32_t TEST_BUTTON_ID = 1;
constexpr uint8_t TEST_PORT_ID = 0;
constexpr uint32_t DEFAULT_CONNECT_RETRY_PERIOD = 5000;
constexpr uint32_t THROUGHTPUT_CALC_WINDOW_MS = 200;

struct TestParam
{
   Dialogs::PortSettingDialog::PortType type;
};

struct PortFixture : public testing::Test
{
   PortFixture():
   gui_controller(nullptr)
   {

   }
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      PortSettingDialogMock_init();
      GUIControllerMock_init();
      MessageDialogMock_init();
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;

      EXPECT_CALL(timer_mock, createTimer(_,DEFAULT_CONNECT_RETRY_PERIOD)).WillOnce(Return(TEST_TIMER_ID));
      EXPECT_CALL(timer_mock, createTimer(_,THROUGHTPUT_CALC_WINDOW_MS)).WillOnce(Return(THROUGHPUT_TIMER_ID));
      EXPECT_CALL(listener_mock, onPortEvent(_));
      EXPECT_CALL(*g_serial_mock, addListener(_));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED,_));
      EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, HasSubstr("PORT")));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      m_test_subject.reset(new Port(TEST_PORT_ID, gui_controller, TEST_BUTTON_NAME, timer_mock, &listener_mock, fake_persistence));
      Mock::VerifyAndClearExpectations(g_socket_mock);
      Mock::VerifyAndClearExpectations(g_serial_mock);
      Mock::VerifyAndClearExpectations(&timer_mock);
      Mock::VerifyAndClearExpectations(&listener_mock);
      Mock::VerifyAndClearExpectations(GUIControllerMock_get());

      EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillRepeatedly(Return(BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillRepeatedly(Return(BUTTON_DEFAULT_FONT_COLOR));

   }
   void TearDown()
   {
      EXPECT_CALL(timer_mock, removeTimer(TEST_TIMER_ID));
      EXPECT_CALL(timer_mock, removeTimer(THROUGHPUT_TIMER_ID));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, _));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      m_test_subject.reset(nullptr);

      MessageDialogMock_deinit();
      PortSettingDialogMock_deinit();
      GUIControllerMock_deinit();
      QtCoreMock_deinit();
      QtWidgetsMock_deinit();
   }

   PortListenerMock listener_mock;
   Utilities::ITimersMock timer_mock;
   GUIController::GUIController gui_controller;
   std::unique_ptr<Port> m_test_subject;
   QMainWindow test_parent;
   QPushButton test_button;
   QLabel test_label;
   QShortcut test_shortcut;
   Utilities::Persistence::Persistence fake_persistence;
   GUIController::ButtonEventListener* m_button_listener;


   void setNewSettings(const Dialogs::PortSettingDialog::Settings& user_settings)
   {
      if (user_settings.type == Dialogs::PortSettingDialog::PortType::ETHERNET)
      {
         g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
      }
      else
      {
         g_serial_mock = new Drivers::Serial::ISerialDriverMock;
      }
      EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, user_settings.port_name));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);
   }

   void requestPortOpen(const Dialogs::PortSettingDialog::Settings& user_settings)
   {
      PortEvent receivied_event;
      /* port opening */
      if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
      {
         EXPECT_CALL(*g_serial_mock, close());
         EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
         EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
      }
      else
      {
         EXPECT_CALL(*g_socket_mock, disconnect());
         EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
         EXPECT_CALL(*g_socket_mock, connect(user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
      }
      EXPECT_CALL(timer_mock, startTimer(THROUGHPUT_TIMER_ID));
      EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
      EXPECT_CALL(listener_mock, onPortEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, user_settings.port_name));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

      /* expect notification via listener */
      EXPECT_EQ(receivied_event.event, Event::CONNECTED);
      EXPECT_THAT(receivied_event.name, HasSubstr(user_settings.port_name));
      EXPECT_TRUE(receivied_event.data.empty());
   }

   void requestPortClose(const Dialogs::PortSettingDialog::Settings& user_settings)
   {
      PortEvent receivied_event;

      /* closing serial port */
      if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
      {
         EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
         EXPECT_CALL(*g_serial_mock, close());
      }
      else
      {
         EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(true));
         EXPECT_CALL(*g_socket_mock, disconnect());
      }
      EXPECT_CALL(listener_mock, onPortEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, user_settings.port_name));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

      /* expect notification via listener */
      EXPECT_EQ(receivied_event.event, Event::DISCONNECTED);
      EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
      EXPECT_TRUE(receivied_event.data.empty());
   }

   void simulateNewDataReceived(const std::vector<uint8_t>& event_data)
   {
      PortEvent receivied_event;

      EXPECT_CALL(listener_mock, onPortEvent(_)).WillOnce(SaveArg<0>(&receivied_event))
                                                       .WillOnce(SaveArg<0>(&receivied_event))
                                                       .WillOnce(SaveArg<0>(&receivied_event));

      ((DriversProxy::Listener*)m_test_subject.get())->onEvent(DriversProxy::Event::DATA_RECEIVED,
                                                               event_data,
                                                               event_data.size());
      ((DriversProxy::Listener*)m_test_subject.get())->onEvent(DriversProxy::Event::DATA_RECEIVED,
                                                               event_data,
                                                               event_data.size());
      ((DriversProxy::Listener*)m_test_subject.get())->onEvent(DriversProxy::Event::DATA_RECEIVED,
                                                               event_data,
                                                               event_data.size());
      EXPECT_EQ(receivied_event.event, Event::NEW_DATA);
      EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
      EXPECT_THAT(receivied_event.data, ContainerEq(event_data));
   }
   void simulateDataWrite(Dialogs::PortSettingDialog::PortType type, const std::vector<uint8_t>& data_to_write)
   {
      if (type == Dialogs::PortSettingDialog::PortType::SERIAL)
      {
         EXPECT_CALL(*g_serial_mock, write(_,_)).WillOnce(Return(true));
      }
      else
      {
         EXPECT_CALL(*g_socket_mock, write(_,_)).WillOnce(Return(true));
      }
      m_test_subject->write(data_to_write, data_to_write.size());
   }
};

struct PortParamFixture : public PortFixture, public ::testing::WithParamInterface<TestParam>
{};

TEST_F(PortFixture, connecting_with_default_settings)
{
   /**
    * <b>scenario</b>: User requested to connect, but the port settings are default <br>
    * <b>expected</b>: Connection to serial port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    * ************************************************
    */

   requestPortOpen(m_test_subject->getSettings());
}

TEST_P(PortParamFixture, settings_change_and_port_connection)
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
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   setNewSettings(user_settings);
   requestPortOpen(user_settings);
   simulateNewDataReceived(std::vector<uint8_t>{'a', 'b', 'c', '\r', '\n', 'd', 'e', 'f', '\n'});

   simulateDataWrite(user_settings.type.value, std::vector<uint8_t>{'a', 'b', 'c', '\r', '\n', 'd', 'e', 'f', '\n'});
   requestPortClose(user_settings);
}

TEST_P(PortParamFixture, settings_change_when_port_is_opened)
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

   PortEvent receivied_event;
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   setNewSettings(user_settings);
   requestPortOpen(user_settings);

   /* request to open settings dialog */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);

   requestPortClose(user_settings);

   /* request to open settings dialog */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED);
}

TEST_F(PortFixture, cannot_open_serial_port)
{
   /**
    * <b>scenario</b>: Serial port open requested, but it is not possible.<br>
    * <b>expected</b>: Button color shall not change. <br>
    * ************************************************
    */
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;

   setNewSettings(user_settings);

   /* expect MessageBox with error */
   EXPECT_CALL(*MessageDialogMock_get(), show(Dialogs::MessageDialog::Icon::Critical,_,_,_));

   /* port opening */
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);
}

TEST_F(PortFixture, cannot_connect_to_socket_server)
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

   PortEvent receivied_event;
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool timer_running = true;

   setNewSettings(user_settings);

   /* port opening */
   EXPECT_CALL(listener_mock, onPortEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
   EXPECT_CALL(*g_socket_mock, disconnect()).Times(7);
   EXPECT_CALL(*g_socket_mock, connect(user_settings.ip_address,user_settings.port)).WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(false))
                                                                                      .WillOnce(Return(true));
   //finally connected to server
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(6);
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(7);

   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID)).WillOnce(Invoke([&](int)
         {
            timer_running = false;
         }));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   EXPECT_CALL(timer_mock, startTimer(THROUGHPUT_TIMER_ID));
   while(timer_running)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   requestPortClose(user_settings);
}

TEST_F(PortFixture, aborting_connection_trials)
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

   PortEvent receivied_event;
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool simulate_abort = false;

   setNewSettings(user_settings);

   /* port opening */
   EXPECT_CALL(listener_mock, onPortEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false))
                                             .WillOnce(Return(false));
   EXPECT_CALL(*g_socket_mock, disconnect()).Times(7);
   EXPECT_CALL(*g_socket_mock, connect(user_settings.ip_address,user_settings.port))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Return(false))
                                                                 .WillOnce(Invoke([&](std::string, uint16_t)
                                                                 {
                                                                     simulate_abort = true;
                                                                     return false;
                                                                 }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   while(!simulate_abort)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
}

TEST_P(PortParamFixture, device_closed_retrying_connection)
{
   /**
    * <b>scenario</b>: Port opened successfully, but after some time the port was disconnected (socket server closed or serial device disconnected). <br>
    * <b>expected</b>: Port shall switch to auto-reconnecting mode. <br>
    *                  Communication shall be restored, when device available again. <br>
    * ************************************************
    */

   PortEvent receivied_event;
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;

   setNewSettings(user_settings);
   requestPortOpen(user_settings);

   EXPECT_CALL(listener_mock, onPortEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));

   /* server disconnected unexpectedly */
   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, user_settings.port_name)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(AtLeast(1));

   ((DriversProxy::Listener*)m_test_subject.get())->onEvent(DriversProxy::Event::ENDPOINT_DISCONNECTED, {}, 0);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* server available again */
   EXPECT_CALL(timer_mock, startTimer(THROUGHPUT_TIMER_ID));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   if (GetParam().type == Dialogs::PortSettingDialog::PortType::ETHERNET)
   {
      EXPECT_CALL(*g_socket_mock, disconnect());
      EXPECT_CALL(*g_socket_mock, connect(user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
   }
   else
   {
      EXPECT_CALL(*g_serial_mock, close());
      EXPECT_CALL(*g_serial_mock, open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER,_)).WillOnce(Return(true));
   }
   ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
}

TEST_F(PortFixture, refreshing_ui_tests)
{
   /**
    * <b>scenario</b>: User requested to reload the UI elements when port is not connected.<br>
    * <b>expected</b>: Settings shall be correctly reloaded without change of state. <br>
    * ************************************************
    */

   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   setNewSettings(user_settings);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_test_subject->refreshUi();

   /**
    * <b>scenario</b>: User requested to reload the UI elements when port is connected.<br>
    * <b>expected</b>: Settings shall be correctly reloaded without change of state. <br>
    * ************************************************
    */
   /* port opening */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(listener_mock, onPortEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   m_test_subject->refreshUi();
}

TEST_F(PortFixture, settings_set_and_get_tests)
{
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   /**
    * <b>scenario</b>: Providing new settings when port is opened.<br>
    * <b>expected</b>: Settings shall be rejected. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "PORT0"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(listener_mock, onPortEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   EXPECT_FALSE(m_test_subject->setSettings(user_settings));
   Dialogs::PortSettingDialog::Settings current_settings = m_test_subject->getSettings();
   EXPECT_THAT(current_settings.port_name, Ne(user_settings.port_name));

   /**
    * <b>scenario</b>: Providing new settings when port is closed.<br>
    * <b>expected</b>: Settings shall be accepted. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "PORT0"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
   EXPECT_CALL(*g_serial_mock, close());
   EXPECT_CALL(listener_mock, onPortEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, GUIController::ButtonEvent::CLICKED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::ETHERNET)
   {
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
   }
   else
   {
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;
   }
   EXPECT_TRUE(m_test_subject->setSettings(user_settings));

}

TEST_F(PortFixture, persistence_writing)
{
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.port_id = 0;
   user_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.serialSettings.mode = Drivers::Serial::DataMode::NEW_LINE_DELIMITER;
   user_settings.serialSettings.device = "";
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   user_settings.trace_color = 1;
   user_settings.font_color = 2;
   ASSERT_TRUE(user_settings != m_test_subject->getSettings());

   /**
    * <b>scenario</b>: Persistence written.<br>
    * <b>expected</b>: Persistence was correctly serialized. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(TEST_PORT_ID, HasSubstr("9600/8/e/2")));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::ETHERNET)
   {
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
   }
   else
   {
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;
   }
   EXPECT_TRUE(m_test_subject->setSettings(user_settings));

   Utilities::Persistence::PersistenceListener::PersistenceItems persistence_written;
   dynamic_cast<Utilities::Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceWrite(persistence_written);

   Dialogs::PortSettingDialog::Settings serialized_settings;
   std::string baudrate_name;
   std::string databits_name;
   std::string paritybits_name;
   std::string stopbits_name;
   std::string porttype_name;

   Utilities::Persistence::readItem(persistence_written, "ipAddress", serialized_settings.ip_address);
   Utilities::Persistence::readItem(persistence_written, "ipPort", serialized_settings.port);
   Utilities::Persistence::readItem(persistence_written, "portId", serialized_settings.port_id);
   Utilities::Persistence::readItem(persistence_written, "portName", serialized_settings.port_name);
   Utilities::Persistence::readItem(persistence_written, "baudRate", baudrate_name);
   Utilities::Persistence::readItem(persistence_written, "dataBits", databits_name);
   Utilities::Persistence::readItem(persistence_written, "parityBits", paritybits_name);
   Utilities::Persistence::readItem(persistence_written, "stopBits", stopbits_name);
   Utilities::Persistence::readItem(persistence_written, "device", serialized_settings.serialSettings.device);
   Utilities::Persistence::readItem(persistence_written, "mode", (uint8_t&)serialized_settings.serialSettings.mode);
   Utilities::Persistence::readItem(persistence_written, "traceColor", serialized_settings.trace_color);
   Utilities::Persistence::readItem(persistence_written, "fontColor", serialized_settings.font_color);
   Utilities::Persistence::readItem(persistence_written, "type", porttype_name);
   serialized_settings.serialSettings.baudRate.fromName(baudrate_name);
   serialized_settings.serialSettings.dataBits.fromName(databits_name);
   serialized_settings.serialSettings.parityBits.fromName(paritybits_name);
   serialized_settings.serialSettings.stopBits.fromName(stopbits_name);
   serialized_settings.type.fromName(porttype_name);

   EXPECT_TRUE(user_settings == serialized_settings);
}

TEST_F(PortFixture, persistence_restoring)
{
   Utilities::Persistence::PersistenceListener::PersistenceItems persistence_written;
   Dialogs::PortSettingDialog::Settings user_settings;
   /**
    * <b>scenario</b>: Persistence written.<br>
    * <b>expected</b>: Persistence was correctly serialized. <br>
    * ************************************************
    */
   user_settings.port_name = "TEST_NAME";
   user_settings.port_id = 0;
   user_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.serialSettings.mode = Drivers::Serial::DataMode::NEW_LINE_DELIMITER;
   user_settings.serialSettings.device = "";
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   user_settings.trace_color = 1;
   user_settings.font_color = 2;
   ASSERT_TRUE(user_settings != m_test_subject->getSettings());

   Utilities::Persistence::writeItem(persistence_written, "ipAddress", user_settings.ip_address);
   Utilities::Persistence::writeItem(persistence_written, "ipPort", user_settings.port);
   Utilities::Persistence::writeItem(persistence_written, "portId", user_settings.port_id);
   Utilities::Persistence::writeItem(persistence_written, "portName", user_settings.port_name);
   Utilities::Persistence::writeItem(persistence_written, "baudRate", user_settings.serialSettings.baudRate.toName());
   Utilities::Persistence::writeItem(persistence_written, "dataBits", user_settings.serialSettings.dataBits.toName());
   Utilities::Persistence::writeItem(persistence_written, "parityBits", user_settings.serialSettings.parityBits.toName());
   Utilities::Persistence::writeItem(persistence_written, "stopBits", user_settings.serialSettings.stopBits.toName());
   Utilities::Persistence::writeItem(persistence_written, "device", user_settings.serialSettings.device);
   Utilities::Persistence::writeItem(persistence_written, "mode", (uint8_t)user_settings.serialSettings.mode);
   Utilities::Persistence::writeItem(persistence_written, "traceColor", user_settings.trace_color);
   Utilities::Persistence::writeItem(persistence_written, "fontColor", user_settings.font_color);
   Utilities::Persistence::writeItem(persistence_written, "type", user_settings.type.toName());

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, user_settings.port_name));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, HasSubstr("9600/8/e/2")));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::ETHERNET)
   {
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
   }
   else
   {
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;
   }
   dynamic_cast<Utilities::Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceRead(persistence_written);

   EXPECT_TRUE(user_settings == m_test_subject->getSettings());

}

TEST_P(PortParamFixture, throughputForwardingWhenPortOpened)
{
   /**
    * <b>scenario</b>: Port is opened, throughput timer expires. <br>
    * <b>expected</b>: Update is sent to device. <br>
    * ************************************************
    */

   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   setNewSettings(user_settings);
   requestPortOpen(user_settings);

   EXPECT_CALL(timer_mock, startTimer(THROUGHPUT_TIMER_ID));
   EXPECT_CALL(*GUIControllerMock_get(), setThroughputText(TEST_PORT_ID, "0.00 B/s"));
   dynamic_cast<Utilities::ITimerClient*>(m_test_subject.get())->onTimeout(THROUGHPUT_TIMER_ID);

   requestPortClose(user_settings);

   /**
    * <b>scenario</b>: Port gets closed, throughput timer expires. <br>
    * <b>expected</b>: Throughtput text shall be cleared, timer should not be started again. <br>
    * ************************************************
    */
   EXPECT_CALL(timer_mock, startTimer(THROUGHPUT_TIMER_ID)).Times(0);
   EXPECT_CALL(*GUIControllerMock_get(), setThroughputText(TEST_PORT_ID, ""));
   dynamic_cast<Utilities::ITimerClient*>(m_test_subject.get())->onTimeout(THROUGHPUT_TIMER_ID);

}


TestParam params[] = {{Dialogs::PortSettingDialog::PortType::SERIAL},{Dialogs::PortSettingDialog::PortType::ETHERNET}};
INSTANTIATE_TEST_CASE_P(PortParamFixture, PortParamFixture, ValuesIn(params));

}

