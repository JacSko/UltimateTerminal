#include "gtest/gtest.h"
#include "TestFramework.h"

const uint32_t TEST_SOCKET_PORT = 2222;
const uint32_t TEST_SECOND_SOCKET_PORT = 3333;
const std::string TEST_IP_ADDRESS = "127.0.0.1";
const uint32_t TEST_BUTTON_CLICK_COUNT = 50;

struct UserButtonsSendingTests : public testing::Test
{
   static void SetUpTestSuite()
   {
      TF::Init();
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
   }
   static void TearDownTestSuite()
   {
      TF::Deinit();
      TF::Disconnect();
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
 * @file UserButtonsSending.cpp
 *
 * @brief
 *    This test suite is designed to verify the user buttons functionality.
 *    Covers basic scenarios like triggering empty button, special button commands, etc..
 *
 * @author Jacek Skowronek
 * @date   10/03/2023
 *
 */
TEST_F(UserButtonsSendingTests, empty_button_pressed)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Simulate BUTTON1 click. <br>
    * <b>expected</b>: <br>
    *       No data shall be sent to socket port. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string USER_BUTTON_NAME = "BUTTON1";
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = 0xAA;
   port_settings.font_color = 0xBB;

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* check user button state before test */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* trigger BUTTON1 click  */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
   TF::wait(500);

   /* check user button state after test */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* expect no message sent */
   EXPECT_EQ(TF::Socket::bufferSize(PORT_ID), 0);
   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(UserButtonsSendingTests, sending_one_command_multiple_times)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set state of BUTTON1 and multiple times simulate the button click. <br>
    * <b>expected</b>: <br>
    *       Socket Server buffer shall contain data from all clicks. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const uint8_t USER_BUTTON_ID = 1;
   const std::string USER_BUTTON_NAME = "BUTTON" + std::to_string(USER_BUTTON_ID);
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = 0xAA;
   port_settings.font_color = 0xBB;

   Dialogs::UserButtonDialog::Settings user_button_settings;
   user_button_settings.id = USER_BUTTON_ID;
   user_button_settings.button_name = "TEST_BUTTON_NAME";
   user_button_settings.raw_commands = "example_command";

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* set user button state */
   EXPECT_TRUE(TF::UserButtons::setSettings(USER_BUTTON_ID, user_button_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(USER_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(USER_BUTTON_NAME), user_button_settings.button_name);

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* set line ending */
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));

   /* trigger BUTTON1 click */
   for (uint8_t i = 0; i < TEST_BUTTON_CLICK_COUNT; i++)
   {
      EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
      EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

      EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
      TF::wait(50);

      EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
      EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));
   }
   TF::wait(100);

   /* check user button state after test */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* expect all messages received */
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SOCKET_PORT), TEST_BUTTON_CLICK_COUNT);
   EXPECT_TRUE(TF::Socket::checkMessageReceived(TEST_SOCKET_PORT, "example_command\n"));

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(UserButtonsSendingTests, sending_multiple_commands_including_wait_command)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set state of BUTTON1 that includes the special "wait" command. <br>
    * <b>expected</b>: <br>
    *       Wait command shall delayed the execution of remaining commands correctly. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const uint8_t USER_BUTTON_ID = 1;
   const std::string USER_BUTTON_NAME = "BUTTON" + std::to_string(USER_BUTTON_ID);
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = 0xAA;
   port_settings.font_color = 0xBB;

   Dialogs::UserButtonDialog::Settings user_button_settings;
   user_button_settings.id = USER_BUTTON_ID;
   user_button_settings.button_name = "TEST_BUTTON_NAME";
   user_button_settings.raw_commands = "example_command\n__wait(5000)\nnew_command\n";

   /* set new port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* set user button state */
   EXPECT_TRUE(TF::UserButtons::setSettings(USER_BUTTON_ID, user_button_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(USER_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(USER_BUTTON_NAME), user_button_settings.button_name);

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* set line ending */
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));

   /* check button state before execution */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* simulate button click and wait for check if first command was executed */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
   TF::wait(200);
   EXPECT_TRUE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_FALSE(TF::Buttons::isEnabled(USER_BUTTON_NAME));
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SOCKET_PORT), 1);

   /* wait for the second command (delayed) */
   TF::wait(5000);
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SOCKET_PORT), 2);

   /* check user button state after test */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(UserButtonsSendingTests, sending_when_two_ports_opened)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set state of BUTTON1. <br>
    *       Open two independent ports. <br>
    *       Trigger user button. <br>
    *       Set the second port as active. <br>
    *       Trigger user button. <br>
    * <b>expected</b>: <br>
    *       First button trigger shall write data to first opened port. <br>
    *       Second button trigger shall write data to port requested by user. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const uint8_t SECOND_PORT_ID = 4;
   const uint8_t USER_BUTTON_ID = 1;
   const std::string USER_BUTTON_NAME = "BUTTON" + std::to_string(USER_BUTTON_ID);
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string SECOND_PORT_BUTTON_NAME = "portButton_" + std::to_string(SECOND_PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string SECOND_PORT_BUTTON_TEXT = "PORT" + std::to_string(SECOND_PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const std::string SECOND_NEW_PORT_NAME = "SEC_NEW_NAME" + std::to_string(SECOND_PORT_ID);

   /* start two socket servers */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));
   EXPECT_TRUE(TF::Socket::startServer(TEST_SECOND_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = 0xAA;
   port_settings.font_color = 0xBB;

   PortSettingDialog::Settings second_port_settings;
   second_port_settings.port_name = SECOND_NEW_PORT_NAME;
   second_port_settings.type = PortSettingDialog::PortType::ETHERNET;
   second_port_settings.ip_address = TEST_IP_ADDRESS;
   second_port_settings.port = TEST_SECOND_SOCKET_PORT;
   second_port_settings.port_id = SECOND_PORT_ID;
   second_port_settings.trace_color = 0xCC;
   second_port_settings.font_color = 0xDD;

   Dialogs::UserButtonDialog::Settings user_button_settings;
   user_button_settings.id = USER_BUTTON_ID;
   user_button_settings.button_name = "TEST_BUTTON_NAME";
   user_button_settings.raw_commands = "example_command";

   /* set new port settings on first port*/
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

   /* set new port settings on second port*/
   EXPECT_TRUE(TF::Ports::setPortSettings(SECOND_PORT_ID, second_port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(SECOND_PORT_BUTTON_NAME), SECOND_NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(SECOND_PORT_ID), second_port_settings.shortSettingsString());

   /* set user button state */
   EXPECT_TRUE(TF::UserButtons::setSettings(USER_BUTTON_ID, user_button_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(USER_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(USER_BUTTON_NAME), user_button_settings.button_name);

   /* open first port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* open second port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(SECOND_NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);

   /* set line ending */
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));

   /* trigger BUTTON1 click */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
   TF::wait(50);
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* expect message received on first port */
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SOCKET_PORT), 1);
   EXPECT_TRUE(TF::Socket::checkMessageReceived(TEST_SOCKET_PORT, "example_command\n"));

   /* set new active port */
   EXPECT_TRUE(TF::Common::setTargetPort(SECOND_NEW_PORT_NAME));

   /* trigger BUTTON1 click */
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
   TF::wait(50);
   EXPECT_FALSE(TF::Buttons::isChecked(USER_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::isEnabled(USER_BUTTON_NAME));

   /* expect message received on second port */
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SECOND_SOCKET_PORT), 1);
   EXPECT_TRUE(TF::Socket::checkMessageReceived(TEST_SECOND_SOCKET_PORT, "example_command\n"));

   /* close the second port */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(SECOND_NEW_PORT_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* check if command is send to first port */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
   TF::wait(50);
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SOCKET_PORT), 2);
   EXPECT_EQ(TF::Socket::bufferSize(TEST_SECOND_SOCKET_PORT), 1);

   /* close remaining socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket servers */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SECOND_SOCKET_PORT));

}