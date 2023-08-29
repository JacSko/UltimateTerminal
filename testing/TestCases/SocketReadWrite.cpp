#include "gtest/gtest.h"
#include "TestFramework.h"
#include "ApplicationExecutor.hpp"

const uint32_t TEST_SOCKET_PORT = 2222;
const std::string TEST_IP_ADDRESS = "127.0.0.1";
static constexpr uint32_t GREEN_COLOR = 0x00FF00;
static constexpr uint32_t BLACK_COLOR = 0x000000;
static constexpr uint32_t BLUE_COLOR = 0x0000FF;
static constexpr uint32_t TEST_TRACES_COUNT = 5;
static constexpr uint32_t TEST_BACKGROUND_COLOR = 0x000001;
static constexpr uint32_t TEST_FONT_COLOR = 0x000002;

struct SocketRead : public testing::Test
{
   static void SetUpTestSuite()
   {
      TF::Init();
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(500);
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
   }
   static void TearDownTestSuite()
   {
      TF::StopTestSubject();
      TF::Disconnect();
      TF::Deinit();
   }
   virtual void SetUp()
   {
      TF::BeginTest();
   }
   virtual void TearDown()
   {
      TF::FinishTest();
   }
};

using namespace Dialogs;

/**
 * @file SocketReadWrite.cpp
 *
 * @brief
 *    This test suite is designed to verify the socket communication functionality.
 *    Covers basic scenarios like open/close sockets with various settings, writing and reading to multiple opened ports.
 *
 * @author Jacek Skowronek
 * @date   18-11-2022
 *
 */

TEST_F(SocketRead, read_data_from_socket)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open one PORT in socket mode, start receiving data. <br>
    * <b>expected</b>: <br>
    *       Default button colors shall be default background with default font. <br>
    *       New settings shall be accepted after dialog shown. <br>
    *       Port label shall be updated and present correct settings. <br>
    *       Port shall be opened correctly.<br>
    *       Correct name shall be presented in port combobox. <br>
    *       Green button color shall be set with black font. <br>
    *       Data shall be added to terminal with correct background and font color. <br>
    *       Port shall be closed correctly. <br>
    *       Port name shall be removed from port combobox. <br>
    *       Red button color shall be set with black font. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const uint32_t BACKGROUND_COLOR = TEST_BACKGROUND_COLOR;
   const uint32_t FONT_COLOR = TEST_FONT_COLOR;

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   /* check port button and label before test */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), PORT_BUTTON_TEXT);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), PORT_BUTTON_TEXT + PortSettingDialog::Settings{}.shortSettingsString());

   /* check throughput label before test */
   EXPECT_EQ(TF::Ports::getThroughput(PORT_ID), "");

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* check button state after open */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GREEN_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), BLACK_COLOR);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* check throughput label after port open */
   TF::wait(500);
   EXPECT_EQ(TF::Ports::getThroughput(PORT_ID), "0,00 B/s");

   /* start generating data on socket server */
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Socket::sendMessage(TEST_SOCKET_PORT, "TEST MESSAGE " + std::to_string(i) + '\n'));
      TF::wait(20);
   }

   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItems(), TEST_TRACES_COUNT);
   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* check button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* check throughput label after test */
   TF::wait(500);
   EXPECT_EQ(TF::Ports::getThroughput(PORT_ID), "");

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(SocketRead, write_socket)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open one PORT in socket mode, start receiving data. <br>
    * <b>expected</b>: <br>
    *       Default button colors shall be default background with default font. <br>
    *       New settings shall be accepted after dialog shown. <br>
    *       Port label shall be updated and present correct settings. <br>
    *       Both ports shall be opened correctly.<br>
    *       Last-opened port name shall be presented in ports combobox. <br>
    *       Green button color shall be set with black font for all ports. <br>
    *       Data shall be added to terminal with correct background and font color. <br>
    *       Ports shall be closed correctly. <br>
    *       Port names shall be removed from port combobox. <br>
    *       Red button color shall be set with black font. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 1;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const uint32_t BACKGROUND_COLOR = TEST_BACKGROUND_COLOR;
   const uint32_t FONT_COLOR = TEST_FONT_COLOR;

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));
   TF::Socket::clearMessageBuffer(TEST_SOCKET_PORT);

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   /* check port button and label before test */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), PORT_BUTTON_TEXT);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), PORT_BUTTON_TEXT + PortSettingDialog::Settings{}.shortSettingsString());

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* check button state after open */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GREEN_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), BLACK_COLOR);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* send data from application to server */
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));
   EXPECT_TRUE(TF::Common::setTargetPort(NEW_PORT_NAME));
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Common::setCommand("this is command " + std::to_string(i)));
      EXPECT_TRUE(TF::Buttons::simulateButtonClick("sendButton"));
      TF::wait(20);
   }

   /* check if data received on socket server */
   TF::wait(1000);
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      std::string expected_message = "this is command " + std::to_string(i) + '\n';
      EXPECT_TRUE(TF::Socket::checkMessageReceived(TEST_SOCKET_PORT, expected_message));
   }

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* check button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(SocketRead, server_reconnection)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open one PORT in socket mode, than close the server. Renable the server. <br>
    * <b>expected</b>: <br>
    *       Default button colors shall be default background with default font. <br>
    *       New settings shall be accepted after dialog shown. <br>
    *       Port label shall be updated and present correct settings. <br>
    *       Both ports shall be opened correctly.<br>
    *       Last-opened port name shall be presented in ports combobox. <br>
    *       Green button color shall be set with black font for all ports. <br>
    *       On server disconnection, the port button shall be BLUE and the reconnection attempts shall be started. <br>
    *       When server is back again, the connection shall be made automatically. <br>
    *       Ports shall be closed correctly. <br>
    *       Port names shall be removed from port combobox. <br>
    *       Red button color shall be set with black font. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 1;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const uint32_t BACKGROUND_COLOR = TEST_BACKGROUND_COLOR;
   const uint32_t FONT_COLOR = TEST_FONT_COLOR;

   /* clear terminal window before test */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("clearButton"));

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));
   TF::Socket::clearMessageBuffer(TEST_SOCKET_PORT);

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* close the server */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));
   TF::wait(1000);

   /* check button color on server disconnection */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), BLUE_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), BLACK_COLOR);
   /* check throughput label after disconnection */
   EXPECT_EQ(TF::Ports::getThroughput(PORT_ID), "0,00 B/s");

   /* start server once again */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   /* wait 2 seconds for reconnection */
   TF::wait(2000);

   /* check button color after server reconnection */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GREEN_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), BLACK_COLOR);

   /* send some data from server to client */
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Socket::sendMessage(TEST_SOCKET_PORT, "TEST MESSAGE " + std::to_string(i) + '\n'));
      TF::wait(20);
   }

   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItems(), TEST_TRACES_COUNT);

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

   TF::wait(500);
   /* check throughput label after disconnection */
   EXPECT_EQ(TF::Ports::getThroughput(PORT_ID), "");

}

TEST_F(SocketRead, open_close_socket_multiple_times_during_high_traffic)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open and close PORT0 multiple times, generate high traffic on socket port. <br>
    * <b>expected</b>: <br>
    *       Application behaves stable during muliple reconnecting events. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string CLEAR_BUTTON_NAME = "clearButton";
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const uint32_t BACKGROUND_COLOR = 0x000011;
   const uint32_t FONT_COLOR = 0x000022;
   const uint8_t RECONNECT_ATTEMPTS = 3;

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* start SocketDataGenerator - generate a string data every 2ms */
   TF::ApplicationExecutor serial_data_generator;
   EXPECT_TRUE(serial_data_generator.startApplication(std::string(RUNTIME_OUTPUT_DIR) + "/SocketDataGenerator",
                                                      "--mode=server --port=" + std::to_string(TEST_SOCKET_PORT) +
                                                      " --payload_rate=2 --payload=\"THIS_IS_EXAMPLE_PAYLOAD\" --exit_on_last_client=0"));
   TF::wait(2000);

   for (uint8_t i = 0; i < RECONNECT_ATTEMPTS; i++)
   {
      /* clear terminal */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(CLEAR_BUTTON_NAME));
      EXPECT_EQ(TF::TerminalView::countItems(), 0);

      /* open port */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);
      EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
      EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
      /* wait for data */
      TF::wait(2000);
      /* close port */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(1000);
      EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
      EXPECT_GT(TF::TerminalView::countItems(), 400);
   }
   EXPECT_TRUE(serial_data_generator.stopApplication());
   TF::wait(5000);
}
