#include "gtest/gtest.h"
#include "TestFramework.h"

const uint32_t TEST_SOCKET_PORT = 2222;
const uint32_t TEST_SECOND_SOCKET_PORT = 3333;
const std::string TEST_IP_ADDRESS = "127.0.0.1";
const uint32_t TEST_BUTTON_CLICK_COUNT = 50;

struct CommandsHistoryTests : public testing::Test
{
   static void SetUpTestSuite()
   {
      TF::Init();
   }
   static void TearDownTestSuite()
   {
      TF::Deinit();
   }
   virtual void SetUp()
   {
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(500);
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
      TF::BeginTest();
   }
   virtual void TearDown()
   {
      TF::FinishTest();
      TF::StopTestSubject();
      TF::Disconnect();
   }
};

using namespace Dialogs;

TEST_F(CommandsHistoryTests, noPortOpened_CommandNotSaved)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Try write to closed port. <br>
    * <b>expected</b>: <br>
    *       Commands history shall be empty. <br>
    * ************************************************
    */

   EXPECT_EQ(TF::Common::countCommandHistory(), 0);
   TF::Common::setCommand("TEST COMMAND 1");
   TF::Buttons::simulateButtonClick("sendButton");
   TF::wait(500);
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);
}

TEST_F(CommandsHistoryTests, portOpened_CommandSaved)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Try write to opened port. <br>
    * <b>expected</b>: <br>
    *       Commands history shall contain exactly one command. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.summaryString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   TF::wait(500);

   /* send data to port */
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);
   TF::Common::setCommand("TEST COMMAND 1");
   TF::Buttons::simulateButtonClick("sendButton");
   TF::wait(500);
   EXPECT_EQ(TF::Common::countCommandHistory(), 1);
   EXPECT_TRUE(TF::Common::isCommandInHistory("TEST COMMAND 1", 0));

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(CommandsHistoryTests, commandSent_FieldCleared)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Try write to opened port. <br>
    * <b>expected</b>: <br>
    *       Command textEdit shall be cleared. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.summaryString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   TF::wait(500);

   /* send data to port */
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);
   TF::Common::setCommand("TEST COMMAND 1");
   EXPECT_EQ(TF::Common::getCommand(), "TEST COMMAND 1");
   TF::Buttons::simulateButtonClick("sendButton");
   TF::wait(500);
   EXPECT_EQ(TF::Common::getCommand(), "");

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}


TEST_F(CommandsHistoryTests, multipleWrites_orderIsCorrect)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Try write multiple different commands to opened port. <br>
    * <b>expected</b>: <br>
    *       Commands history shall contain correct amount of commands and correct order. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   const std::vector<std::string> commandsToSend = {"TEST_COMMAND2", "TEST_COMMAND3", "TEST_COMMAND4", "TEST_COMMAND5"};

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.summaryString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   TF::wait(500);

   /* send data to port */
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);

   for (auto command : commandsToSend)
   {
      TF::Common::setCommand(command);
      TF::Buttons::simulateButtonClick("sendButton");
      TF::wait(100);
   }
   TF::wait(500);
   EXPECT_EQ(TF::Common::countCommandHistory(), commandsToSend.size());

   for (uint8_t i = 0; i < commandsToSend.size(); i++)
   {
      EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[i], commandsToSend.size() - i - 1));
   }

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(CommandsHistoryTests, multipleWrites_duplicatesRemoved)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Try write multiple the same commands to opened port. <br>
    * <b>expected</b>: <br>
    *       Duplicates shall be removed, commands shall be saved in history as latest command. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   const std::vector<std::string> commandsToSend = {"TEST_COMMAND2", "TEST_COMMAND3", "TEST_COMMAND4", "TEST_COMMAND3"};

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.summaryString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   TF::wait(500);

   /* send data to port */
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);

   for (auto command : commandsToSend)
   {
      TF::Common::setCommand(command);
      TF::Buttons::simulateButtonClick("sendButton");
      TF::wait(100);
   }
   TF::wait(500);
   EXPECT_EQ(TF::Common::countCommandHistory(), 3);

   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[3], 0));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[2], 1));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[0], 2));

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));

}

TEST_F(CommandsHistoryTests, portSwitched_historyReloaded)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open two ports, start writing and switching the current port. <br>
    * <b>expected</b>: <br>
    *       Commands history shall be always up to date. <br>
    * ************************************************
    */

   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   const uint8_t PORT2_ID = 1;
   const std::string PORT2_BUTTON_NAME = "portButton_" + std::to_string(PORT2_ID);
   const std::string NEW_PORT2_NAME = "NEW_NAME" + std::to_string(PORT2_ID);

   /* start socket server on TEST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));
   EXPECT_TRUE(TF::Socket::startServer(TEST_SECOND_SOCKET_PORT));

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = TEST_IP_ADDRESS;
   port_settings.port = TEST_SOCKET_PORT;
   port_settings.port_id = PORT_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.summaryString());

   PortSettingDialog::Settings second_port_settings;
   second_port_settings.port_name = NEW_PORT2_NAME;
   second_port_settings.type = PortSettingDialog::PortType::ETHERNET;
   second_port_settings.ip_address = TEST_IP_ADDRESS;
   second_port_settings.port = TEST_SECOND_SOCKET_PORT;
   second_port_settings.port_id = PORT2_ID;
   EXPECT_TRUE(TF::Ports::setPortSettings(PORT2_ID, second_port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT2_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT2_BUTTON_NAME), NEW_PORT2_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT2_ID), second_port_settings.summaryString());

   /* open port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT2_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT2_NAME));
   TF::wait(500);

   /* check current port */
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);

   const std::vector<std::string> commandsToSend = {"P1_1", "P1_2", "P1_3", "P1_4"};
   const std::vector<std::string> commandsToSendSecondPort = {"P2_1", "P2_2", "P2_3"};

   for (auto command : commandsToSend)
   {
      TF::Common::setCommand(command);
      TF::Buttons::simulateButtonClick("sendButton");
      TF::wait(100);
   }
   EXPECT_EQ(TF::Common::countCommandHistory(), commandsToSend.size());

   EXPECT_TRUE(TF::Common::setTargetPort(NEW_PORT2_NAME));
   EXPECT_EQ(TF::Common::countCommandHistory(), 0);
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT2_NAME);
   TF::wait(100);

   for (auto command : commandsToSendSecondPort)
   {
      TF::Common::setCommand(command);
      TF::Buttons::simulateButtonClick("sendButton");
      TF::wait(100);
   }
   EXPECT_EQ(TF::Common::countCommandHistory(), commandsToSendSecondPort.size());

   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSendSecondPort[2], 0));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSendSecondPort[1], 1));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSendSecondPort[0], 2));

   EXPECT_TRUE(TF::Common::setTargetPort(NEW_PORT_NAME));
   TF::wait(500);
   EXPECT_EQ(TF::Common::countCommandHistory(), commandsToSend.size());
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[3], 0));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[2], 1));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[1], 2));
   EXPECT_TRUE(TF::Common::isCommandInHistory(commandsToSend[0], 3));

   /* close socket in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT2_BUTTON_NAME));

   /* close socket server on FIRST_SOCKET_PORT */
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));
   EXPECT_TRUE(TF::Socket::stopServer(TEST_SECOND_SOCKET_PORT));


}
