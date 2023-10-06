#include "gtest/gtest.h"
#include "TestFramework.h"

struct PersistenceSaveRestoreTests : public testing::Test
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
      TF::deletePersistenceFile();
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(2000);
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
      TF::BeginTest();
   }
   virtual void TearDown()
   {
      TF::FinishTest();
      TF::StopTestSubject();
      TF::Disconnect();
      TF::deletePersistenceFile();
   }

   void restartApplication()
   {
      TF::Common::closeApplication();
      TF::Disconnect();
      TF::wait(3000);
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(3000);
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
   }
};

using namespace Dialogs;

TEST_F(PersistenceSaveRestoreTests, allPortsShallBeStoredInPersistence)
{
   const uint8_t PORT_COUNT = 5;
   const uint32_t BACKGROUND_COLOR = 0x111111;
   const uint32_t FONT_COLOR = 0x222222;
   const uint32_t IP_PORT_BASE = 1500;

   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set ports to known state, than close and open again. <br>
    * <b>expected</b>: <br>
    *       All ports shall be restored from persistence. <br>
    * ************************************************
    */

   for (uint8_t i = 0; i < PORT_COUNT; i++)
   {
      const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(i);
      const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(i);
      const std::string DEVICE_NAME = "dev" + std::to_string(i);

      PortSettingDialog::Settings port_settings;
      port_settings.port_name = NEW_PORT_NAME;
      port_settings.type = PortSettingDialog::PortType::ETHERNET;
      port_settings.ip_address = "127.0.0.1";
      port_settings.port = IP_PORT_BASE + i;
      port_settings.port_id = i;
      port_settings.trace_color = BACKGROUND_COLOR;
      port_settings.font_color = FONT_COLOR;
      EXPECT_TRUE(TF::Ports::setPortSettings(i, port_settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
      EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
      EXPECT_EQ(TF::Ports::getLabelText(i), port_settings.shortSettingsString());
   }

   restartApplication();

   for (uint8_t i = 0; i < PORT_COUNT; i++)
   {
      const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(i);
      const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(i);
      const std::string DEVICE_NAME = "dev" + std::to_string(i);
      const std::string TEST_MESSAGE = "MESSAGE " + std::to_string(i);
      const uint32_t IP_PORT = IP_PORT_BASE + i;

      EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
      TF::Socket::startServer(IP_PORT);
      TF::wait(100);
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(100);
      TF::Socket::sendMessage(IP_PORT, TEST_MESSAGE + '\n');
      TF::wait(100);
      EXPECT_EQ(TF::TerminalView::countItems(), 1);
      EXPECT_TRUE(TF::Buttons::simulateButtonClick("clearButton"));
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(1000);
      TF::Socket::stopServer(IP_PORT);
   }
}

TEST_F(PersistenceSaveRestoreTests, allTraceFiltersShallBeStoredInPersistence)
{
   const uint8_t FILTERS_COUNT = 7;
   const uint32_t IP_PORT = 1234;
   const std::string PORT_BUTTON_NAME = "portButton_0";

   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set trace filter to known state, than close and open again. <br>
    * <b>expected</b>: <br>
    *       All trace filters shall be restored from persistence. <br>
    * ************************************************
    */

   for (uint8_t i = 0; i < FILTERS_COUNT; i++)
   {
      const std::string TRACE_FILTER_NAME = "traceFilter_" + std::to_string(i);
      const std::string TRACE_BUTTON_NAME = "traceFilterButton_" + std::to_string(i);
      const std::string TRACE_FILTER_TEXT = "TEXT " + std::to_string(i);
      const uint32_t BACKGROUND_COLOR = i;
      const uint8_t FONT_COLOR = UINT8_MAX - i;

      Dialogs::TraceFilterSettingDialog::Settings settings;
      settings.background = BACKGROUND_COLOR;
      settings.font = FONT_COLOR;
      settings.regex = TRACE_FILTER_TEXT;
      settings.id = i;
      EXPECT_TRUE(TF::TraceFilters::setSettings(i,settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(TRACE_BUTTON_NAME));
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(TRACE_BUTTON_NAME));

      EXPECT_EQ(TF::TraceFilters::getBackgroundColor(TRACE_FILTER_NAME), settings.background);
      EXPECT_EQ(TF::TraceFilters::getFontColor(TRACE_FILTER_NAME), settings.font);
      EXPECT_EQ(TF::TraceFilters::getText(TRACE_FILTER_NAME), settings.regex);
      EXPECT_FALSE(TF::TraceFilters::isEditable(TRACE_FILTER_NAME));
   }

   restartApplication();

   TF::Socket::startServer(IP_PORT);
   PortSettingDialog::Settings port_settings;
   port_settings.port_name = "NAME";
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = "127.0.0.1";
   port_settings.port = IP_PORT;
   port_settings.port_id = 0;
   EXPECT_TRUE(TF::Ports::setPortSettings(port_settings.port_id, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   TF::wait(100);


   for (uint8_t i = 0; i < FILTERS_COUNT; i++)
   {
      const std::string TRACE_FILTER_NAME = "traceFilter_" + std::to_string(i);
      const std::string TRACE_FILTER_TEXT = "TEXT " + std::to_string(i);
      const uint32_t BACKGROUND_COLOR = i;
      const uint8_t FONT_COLOR = UINT8_MAX - i;

      EXPECT_EQ(TF::TraceFilters::getBackgroundColor(TRACE_FILTER_NAME), BACKGROUND_COLOR);
      EXPECT_EQ(TF::TraceFilters::getFontColor(TRACE_FILTER_NAME), FONT_COLOR);
      EXPECT_EQ(TF::TraceFilters::getText(TRACE_FILTER_NAME), TRACE_FILTER_TEXT);
      EXPECT_FALSE(TF::TraceFilters::isEditable(TRACE_FILTER_NAME));

      TF::Socket::sendMessage(IP_PORT, TRACE_FILTER_TEXT + '\n');
      TF::wait(100);
      EXPECT_EQ(TF::TraceView::countItems(), 1);
      EXPECT_EQ(TF::TraceView::countItemsWithBackgroundColor(BACKGROUND_COLOR), 1);
      EXPECT_EQ(TF::TraceView::countItemsWithFontColor(FONT_COLOR), 1);
      EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceClearButton"));
   }

   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   TF::wait(1000);
   TF::Socket::stopServer(IP_PORT);
}

TEST_F(PersistenceSaveRestoreTests, allUserButtonsShallBeStoredInPersistence)
{
   const uint8_t USER_BUTTONS_PER_ROW = 5;
   const uint8_t USER_TABS_COUNT = 3;
   const uint8_t USER_BUTTONS_ROWS = 2;
   const uint16_t USER_BUTTONS_COUNT = USER_BUTTONS_PER_ROW * USER_BUTTONS_ROWS * USER_TABS_COUNT;

   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set some of user buttons to known state, than close and open again. <br>
    * <b>expected</b>: <br>
    *       All user buttons shall be restored from persistence. <br>
    * ************************************************
    */

   /* set all ports to known state */
   for (uint8_t i = 0; i < USER_BUTTONS_COUNT; i++)
   {

      const std::string USER_BUTTON_NAME = "BUTTON" + std::to_string(i);
      const std::string USER_BUTTON_TEXT = "TEST BTN " + std::to_string(i);
      const std::string USER_BUTTON_COMMAND = "cmd1\ncmd2\ncmd3\n";
      Dialogs::UserButtonDialog::Settings settings;
      settings.id = i;
      settings.button_name = USER_BUTTON_TEXT;
      settings.raw_commands = USER_BUTTON_COMMAND;
      EXPECT_TRUE(TF::UserButtons::setSettings(i, settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(USER_BUTTON_NAME));
      EXPECT_TRUE(TF::UserButtons::getSettings(i) == settings);

      EXPECT_EQ(TF::Buttons::getText(USER_BUTTON_NAME), USER_BUTTON_TEXT);
   }

   restartApplication();

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = "NAME1";
   port_settings.type = PortSettingDialog::PortType::ETHERNET;
   port_settings.ip_address = "127.0.0.1";
   port_settings.port = 4321;
   port_settings.port_id = 0;
   port_settings.trace_color = 0xAA;
   port_settings.font_color = 0xBB;

   /* set new port settings on first port*/
   EXPECT_TRUE(TF::Ports::setPortSettings(port_settings.port_id, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick("portButton_0"));

   TF::Socket::startServer(port_settings.port);
   TF::wait(100);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("portButton_0"));
   TF::wait(500);

   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));

   for (uint8_t i = 0; i < USER_BUTTONS_COUNT; i++)
   {
      const std::string USER_BUTTON_NAME = "BUTTON" + std::to_string(i);
      const std::string USER_BUTTON_TEXT = "TEST BTN " + std::to_string(i);
      const std::string USER_BUTTON_COMMAND = "cmd1\ncmd2\ncmd3\n";
      EXPECT_EQ(TF::Buttons::getText(USER_BUTTON_NAME), USER_BUTTON_TEXT);

      TF::Socket::clearMessageBuffer(port_settings.port);
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(USER_BUTTON_NAME));
      TF::wait(500);
      EXPECT_TRUE(TF::Socket::checkMessageReceived(port_settings.port, "cmd1\n"));
      EXPECT_TRUE(TF::Socket::checkMessageReceived(port_settings.port, "cmd2\n"));
      EXPECT_TRUE(TF::Socket::checkMessageReceived(port_settings.port, "cmd3\n"));
   }

   TF::Socket::stopServer(port_settings.port);
}

TEST_F(PersistenceSaveRestoreTests, allCommandsHistoryShallBeStoredInPersistence)
{
   const uint8_t PORT_COUNT = 5;
   const uint32_t BACKGROUND_COLOR = 0x111111;
   const uint32_t FONT_COLOR = 0x222222;
   const uint32_t IP_PORT = 1500;
   const uint8_t COMMANDS_COUNT = 5;
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Create some commands history, than close and open again. <br>
    * <b>expected</b>: <br>
    *       All commands history shall be restored from persistence. <br>
    * ************************************************
    */
   TF::Socket::startServer(IP_PORT);
   TF::wait(100);

   for (uint8_t i = 0; i < PORT_COUNT; i++)
   {
      const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(i);
      const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(i);
      const std::string DEVICE_NAME = "dev" + std::to_string(i);

      PortSettingDialog::Settings port_settings;
      port_settings.port_name = NEW_PORT_NAME;
      port_settings.type = PortSettingDialog::PortType::ETHERNET;
      port_settings.ip_address = "127.0.0.1";
      port_settings.port = IP_PORT;
      port_settings.port_id = i;
      port_settings.trace_color = BACKGROUND_COLOR;
      port_settings.font_color = FONT_COLOR;
      EXPECT_TRUE(TF::Ports::setPortSettings(i, port_settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
      EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
      EXPECT_EQ(TF::Ports::getLabelText(i), port_settings.shortSettingsString());

      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);

      for (uint8_t j = 0; j < COMMANDS_COUNT; j++)
      {
         EXPECT_TRUE(TF::Common::setCommand("ID " + std::to_string(i) + " CMD " + std::to_string(j)));
         EXPECT_TRUE(TF::Buttons::simulateButtonClick("sendButton"));
         TF::wait(50);
      }

      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);
   }
   TF::Socket::stopServer(IP_PORT);

   restartApplication();

   TF::Socket::startServer(IP_PORT);
   TF::wait(100);

   for (uint8_t i = 0; i < PORT_COUNT; i++)
   {
      const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(i);
      const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(i);
      const std::string DEVICE_NAME = "dev" + std::to_string(i);

      PortSettingDialog::Settings port_settings;
      port_settings.port_name = NEW_PORT_NAME;
      port_settings.type = PortSettingDialog::PortType::ETHERNET;
      port_settings.ip_address = "127.0.0.1";
      port_settings.port = IP_PORT;
      port_settings.port_id = i;
      port_settings.trace_color = BACKGROUND_COLOR;
      port_settings.font_color = FONT_COLOR;
      EXPECT_TRUE(TF::Ports::setPortSettings(i, port_settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
      EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
      EXPECT_EQ(TF::Ports::getLabelText(i), port_settings.shortSettingsString());

      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);
      EXPECT_EQ(TF::Common::countCommandHistory(), COMMANDS_COUNT);
      for(uint8_t j = 0; j < COMMANDS_COUNT; j++)
      {
         EXPECT_TRUE(TF::Common::isCommandInHistory("ID " + std::to_string(i) + " CMD " + std::to_string(j), COMMANDS_COUNT - j - 1));
      }

      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);
   }
}

TEST_F(PersistenceSaveRestoreTests, lineEndingShallBeStoredInPersistence)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Set line ending, than close and open again. <br>
    * <b>expected</b>: <br>
    *       Line ending shall be restored from persistence. <br>
    * ************************************************
    */
   EXPECT_TRUE(TF::Common::setLineEnding("EMPTY"));
   restartApplication();
   EXPECT_EQ(TF::Common::getLineEnding(), "EMPTY");

}
