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
#include "GUIControllerMock.h"
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

constexpr uint32_t RED_COLOR = 0xFF0000;
constexpr uint32_t GREEN_COLOR = 0x00FF00;
constexpr uint32_t BLUE_COLOR = 0x0000FF;
constexpr uint32_t BLACK_COLOR = 0x000000;
constexpr uint32_t BUTTON_DEFAULT_BACKGROUND_COLOR = 0x111111;
constexpr uint32_t BUTTON_DEFAULT_FONT_COLOR = 0x222222;

constexpr int TEST_TIMER_ID = 1;
const std::string TEST_BUTTON_NAME = "BUTTON_NAME";
constexpr uint32_t TEST_BUTTON_ID = 1;

struct TestParam
{
   Dialogs::PortSettingDialog::PortType type;
};

struct PortHandlerFixture : public testing::Test
{
   PortHandlerFixture():
   gui_controller(nullptr)
   {

   }
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      PortSettingDialogMock_init();
      GUIControllerMock_init();
      g_socket_mock = new Drivers::SocketClient::ISocketClientMock;
      g_serial_mock = new Drivers::Serial::ISerialDriverMock;

      EXPECT_CALL(timer_mock, createTimer(_,_)).WillOnce(Return(TEST_TIMER_ID));
      EXPECT_CALL(listener_mock, onPortHandlerEvent(_));
      EXPECT_CALL(*g_socket_mock, addListener(_));
      EXPECT_CALL(*g_serial_mock, addListener(_));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));
      EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillOnce(Return(BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillOnce(Return(BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, HasSubstr("PORT")));
      EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
      m_test_subject.reset(new PortHandler(gui_controller, TEST_BUTTON_NAME, timer_mock, &listener_mock, fake_persistence));
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
      EXPECT_CALL(*g_socket_mock, removeListener(_));
      EXPECT_CALL(*g_serial_mock, removeListener(_));
      EXPECT_CALL(*g_socket_mock, disconnect());
      EXPECT_CALL(*g_serial_mock, close());
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      m_test_subject.reset(nullptr);

      PortSettingDialogMock_deinit();
      GUIControllerMock_deinit();
      QtCoreMock_deinit();
      QtWidgetsMock_deinit();
   }

   PortHandlerListenerMock listener_mock;
   Utilities::ITimersMock timer_mock;
   GUIController gui_controller;
   std::unique_ptr<PortHandler> m_test_subject;
   QMainWindow test_parent;
   QPushButton test_button;
   QLabel test_label;
   QShortcut test_shortcut;
   Persistence::PersistenceHandler fake_persistence;
   ButtonEventListener* m_button_listener;
};

struct PortHandlerParamFixture : public PortHandlerFixture, public ::testing::WithParamInterface<TestParam>
{};

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
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   EXPECT_TRUE(m_test_subject->isOpened());
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("PORT"));
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
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* port opening */
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
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
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* data received from port, three events */
   const std::vector<uint8_t> event_data = {'a', 'b', 'c', '\r', '\n', 'd', 'e', 'f', 'n'};
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event))
                                                    .WillOnce(SaveArg<0>(&receivied_event))
                                                    .WillOnce(SaveArg<0>(&receivied_event));

   if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
   {
      Drivers::Serial::DriverEvent event = Drivers::Serial::DriverEvent::DATA_RECV;
      /* force three events with new data */
      ((Drivers::Serial::SerialListener*)m_test_subject.get())->onSerialEvent(event, event_data, event_data.size());
      ((Drivers::Serial::SerialListener*)m_test_subject.get())->onSerialEvent(event, event_data, event_data.size());
      ((Drivers::Serial::SerialListener*)m_test_subject.get())->onSerialEvent(event, event_data, event_data.size());
   }
   else
   {
      Drivers::SocketClient::ClientEvent event = Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV;
      /* force three events with new data */
      ((Drivers::SocketClient::ClientListener*)m_test_subject.get())->onClientEvent(event, event_data, event_data.size());
      ((Drivers::SocketClient::ClientListener*)m_test_subject.get())->onClientEvent(event, event_data, event_data.size());
      ((Drivers::SocketClient::ClientListener*)m_test_subject.get())->onClientEvent(event, event_data, event_data.size());
   }

   /* check last notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::NEW_DATA);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_THAT(receivied_event.data, ContainerEq(event_data));

   /* data send to port */
   const std::vector<uint8_t> data_to_write (event_data.rbegin(), event_data.rend());
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
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
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

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
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = GetParam().type;
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* port opening */
   if (user_settings.type == Dialogs::PortSettingDialog::PortType::SERIAL)
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
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* request to open settings dialog */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

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
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillOnce(SaveArg<0>(&receivied_event));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* request to open settings dialog */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);
}

TEST_F(PortHandlerFixture, cannot_open_serial_port)
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

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* expect QMessageBox with error */
   EXPECT_CALL(*QtWidgetsMock_get(), QMessageBox_exec(_));

   /* port opening */
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);
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
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool timer_running = true;

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

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
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(6);
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(7);

   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID)).WillOnce(Invoke([&](int)
         {
            timer_running = false;
         }));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   while(timer_running)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
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
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;
   bool simulate_abort = false;

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

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
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   while(!simulate_abort)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);
   }
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::DISCONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
}

TEST_F(PortHandlerFixture, socket_server_closed_retrying_connection)
{
   /**
    * <b>scenario</b>: Socket server closed connection unexpectedly. <br>
    * <b>expected</b>: Socket connection requested. <br>
    *                  Button color changed to CONNECTING <br>
    *                  Connection started successfully. <br>
    *                  After socket disconnection, button color shall change to CONNECTING. <br>
    *                  Listeners notified about disconnection. <br>
    *                  When server is back again, connection shall be restored.<br>
    * ************************************************
    */

   PortHandlerEvent receivied_event;
   Dialogs::PortSettingDialog::Settings user_settings;
   user_settings.port_name = "TEST_NAME";
   user_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   user_settings.ip_address = "192.168.1.100";
   user_settings.port = 1234;

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* port opening */
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_)).WillRepeatedly(SaveArg<0>(&receivied_event));

   EXPECT_CALL(*g_socket_mock, isConnected()).WillOnce(Return(false));
   EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* server disconnected unexpectedly */
   EXPECT_CALL(timer_mock, setTimeout(TEST_TIMER_ID, _)).Times(AtLeast(1));
   EXPECT_CALL(timer_mock, startTimer(TEST_TIMER_ID)).Times(AtLeast(1));
   EXPECT_CALL(*g_socket_mock, disconnect());
   EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(false));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BLUE_COLOR)).Times(AtLeast(1));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR)).Times(AtLeast(1));
   ((Drivers::SocketClient::ClientListener*)m_test_subject.get())->onClientEvent(Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED, {}, 0);
   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTING);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());

   /* server available again */
   EXPECT_CALL(timer_mock, stopTimer(TEST_TIMER_ID));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*g_socket_mock, connect(_,user_settings.ip_address,user_settings.port)).WillOnce(Return(true));
   ((Utilities::ITimerClient*)m_test_subject.get())->onTimeout(TEST_TIMER_ID);

   /* expect notification via listener */
   EXPECT_EQ(receivied_event.event, GUI::Event::CONNECTED);
   EXPECT_THAT(receivied_event.name, HasSubstr("TEST_NAME"));
   EXPECT_TRUE(receivied_event.data.empty());
}

TEST_F(PortHandlerFixture, refreshing_ui_tests)
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

   /* settings change requested */
   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*PortSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(user_settings), Return(true)));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   m_test_subject->refreshUi();

   /**
    * <b>scenario</b>: User requested to reload the UI elements when port is connected.<br>
    * <b>expected</b>: Settings shall be correctly reloaded without change of state. <br>
    * ************************************************
    */
   /* port opening */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   m_test_subject->refreshUi();
}

TEST_F(PortHandlerFixture, settings_set_and_get_tests)
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
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(false));
   EXPECT_CALL(*g_serial_mock, open(_,_)).WillOnce(Return(true));
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   EXPECT_FALSE(m_test_subject->setSettings(user_settings));
   Dialogs::PortSettingDialog::Settings current_settings = m_test_subject->getSettings();
   EXPECT_THAT(current_settings.port_name, Ne(user_settings.port_name));

   /**
    * <b>scenario</b>: Providing new settings when port is closed.<br>
    * <b>expected</b>: Settings shall be accepted. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*g_serial_mock, isOpened()).WillOnce(Return(true));
   EXPECT_CALL(*g_serial_mock, close());
   EXPECT_CALL(listener_mock, onPortHandlerEvent(_));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, "TEST_NAME"));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelText(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setPortLabelStylesheet(_, _));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_TRUE(m_test_subject->setSettings(user_settings));

}

TestParam params[] = {{Dialogs::PortSettingDialog::PortType::SERIAL},{Dialogs::PortSettingDialog::PortType::ETHERNET}};
INSTANTIATE_TEST_CASE_P(PortHandlerParamFixture, PortHandlerParamFixture, ValuesIn(params));

}

