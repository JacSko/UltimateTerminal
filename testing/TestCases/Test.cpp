#include <thread>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TestFramework.h"

struct TestFixture : public testing::Test
{
   static void SetUpTestSuite()
   {
//      ASSERT_TRUE(TF::Connect());
   }
   static void TearDownTestSuite()
   {
//      TF::Disconnect();
   }
};

TEST_F(TestFixture, test)
{
//   EXPECT_EQ(TF::Buttons::getBackgroundColor("scrollButton"), 0x00FF00);
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("scrollButton"));
//   EXPECT_EQ(TF::Buttons::getBackgroundColor("scrollButton"), 0xffefefef);
//
//   EXPECT_EQ(TF::Common::getCommand(), "");
//   EXPECT_TRUE(TF::Common::setCommand("test command"));
//   EXPECT_EQ(TF::Common::getCommand(), "test command");

//   EXPECT_EQ(TF::Ports::getLabelText(0), "");
//   EXPECT_EQ(TF::Ports::getLabelStylesheet(0), "");
//
//   EXPECT_EQ(TF::Ports::getLabelText(1), "");
//   EXPECT_EQ(TF::Ports::getLabelStylesheet(1), "");

//   EXPECT_EQ(TF::Common::getLineEnding(), "\\r\\n");
//   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));
//   EXPECT_EQ(TF::Common::getLineEnding(), "\\n");
//
//   EXPECT_TRUE(TF::Common::isLineEndingVisible("\\r\\n"));
//   EXPECT_TRUE(TF::Common::isLineEndingVisible("\\n"));
//   EXPECT_TRUE(TF::Common::isLineEndingVisible("EMPTY"));
//   EXPECT_FALSE(TF::Common::isLineEndingVisible("not existing"));
//
//   EXPECT_EQ(TF::Common::getTargetPort(), "");
//   EXPECT_FALSE(TF::Common::setTargetPort("not exist"));
//   EXPECT_EQ(TF::Common::countTargetPorts(), 0);
//

//   EXPECT_TRUE(TF::TraceFilters::isEditable("traceFilterButton_1"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "");
//   EXPECT_TRUE(TF::TraceFilters::setText("traceFilterButton_3", "test_filter"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "test_filter");
//   EXPECT_TRUE(TF::TraceFilters::isEditable("traceFilterButton_3"));
//
//
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_3"));
//   EXPECT_FALSE(TF::TraceFilters::isEditable("traceFilterButton_3"));
//   EXPECT_FALSE(TF::TraceFilters::setText("traceFilterButton_3", "new_filter"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "test_filter");



//   EXPECT_EQ(TF::TerminalView::countItems(), 0);
//   EXPECT_EQ(TF::TraceView::countItems(), 0);
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("markerButton"));
//   EXPECT_EQ(TF::TerminalView::countItems(), 1);
//   EXPECT_EQ(TF::TraceView::countItems(), 0);
//
//   EXPECT_EQ(TF::TerminalView::countItemsWithBackgroundColor(0x00FF0000), 1);
//   EXPECT_EQ(TF::TerminalView::countItemsWithFontColor(0xFF000000), 1);
//
//   EXPECT_TRUE(TF::TraceFilters::setText("traceFilterButton_3", "MARKER"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "MARKER");
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_3"));
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("markerButton"));
//   EXPECT_EQ(TF::TerminalView::countItems(), 2);
//   EXPECT_EQ(TF::TraceView::countItems(), 1);
//

//   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick("portButton_0"));
//   Dialogs::PortSettingDialog::Settings current_settins = TF::Ports::getPortSettings(0);
//   current_settins.port_name = "NEW NAME";
//   EXPECT_TRUE(TF::Ports::setPortSettings(0, current_settins));
//   Dialogs::PortSettingDialog::Settings new_settings = TF::Ports::getPortSettings(0);
//   EXPECT_EQ(new_settings.port_name, "NEW NAME");
//

//   Dialogs::TraceFilterSettingDialog::Settings setting = {};
//   setting.background = 0x112233;
//   setting.font = 0x778899;
//   setting.regex = "MAR";
//   EXPECT_TRUE(TF::TraceFilters::setSettings(0, setting));
//
//   Dialogs::TraceFilterSettingDialog::Settings new_settings = TF::TraceFilters::getSettings(0);
//
//   EXPECT_EQ(new_settings.background, setting.background);
//   EXPECT_EQ(new_settings.font, setting.font);
//   EXPECT_EQ(new_settings.regex, setting.regex);
//
//   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick("traceFilterButton_0"));
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_0"));
//
//   EXPECT_EQ(TF::TerminalView::countItems(), 0);
//   EXPECT_EQ(TF::TraceView::countItems(), 0);
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("markerButton"));
//   EXPECT_EQ(TF::TerminalView::countItems(), 1);
//   EXPECT_EQ(TF::TraceView::countItems(), 1);

//   Dialogs::UserButtonDialog::Settings settings {};
//   settings.button_name = "name";
//   settings.raw_commands = "command1\n__wait(200)\ncommand2\n";
//   EXPECT_TRUE(TF::UserButtons::setSettings(3, settings));
//   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick("BUTTON3"));
//   Dialogs::UserButtonDialog::Settings new_settings = TF::UserButtons::getSettings(3);
//   EXPECT_EQ(new_settings.button_name, settings.button_name);
//   EXPECT_EQ(new_settings.raw_commands, settings.raw_commands);
//
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("BUTTON3"));
//   std::this_thread::sleep_for(std::chrono::milliseconds(500));


   TF::Serial::startForwarding("d1", "d2");
   std::this_thread::sleep_for(std::chrono::seconds(10));

   Drivers::Serial::Settings settings;
   settings.device = "/home/jskowronek/projects/UltimateTerminal/build_sim/d1";

   EXPECT_TRUE(TF::Serial::openSerialPort(settings));
   EXPECT_TRUE(TF::Serial::sendMessage(settings.device, "Testmessage\n"));
   EXPECT_TRUE(TF::Serial::closeSerialPort(settings.device));

   std::this_thread::sleep_for(std::chrono::seconds(5));
   TF::Serial::stopForwarding("d1", "d2");


}
