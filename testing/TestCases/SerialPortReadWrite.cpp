#include "gtest/gtest.h"
#include "TestFramework.h"
#include "Settings.h"

static const char* FIRST_SERIAL_PORT_LINK = "link1";
static const char* SECOND_SERIAL_PORT_LINK = "link2";
static constexpr uint32_t RED_COLOR = 0xFF0000;
static constexpr uint32_t GREEN_COLOR = 0x00FF00;
static constexpr uint32_t BLACK_COLOR = 0x000000;
static constexpr uint32_t BACKGROUND_COLOR = 0x111111;
static constexpr uint32_t FONT_COLOR = 0x222222;


struct SerialPortReadWrite : public testing::Test
{
   static void SetUpTestSuite()
   {
      ASSERT_TRUE(TF::Connect());
      EXPECT_TRUE(TF::Serial::startForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
   }
   static void TearDownTestSuite()
   {
      EXPECT_TRUE(TF::Serial::startForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
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
 * @file SerialPortReading.cpp
 *
 * @brief
 *    This test suite is designed to verify the serial communication functionality.
 *    Covers basic scenarios like open/close serial ports with various settings, writing and reading to multiple opened ports.
 *
 * @author Jacek Skowronek
 * @date   18-11-2022
 *
 */

TEST_F(SerialPortReadWrite, read_write_serial_port)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open one PORT at time with different settings, start receiving data. <br>
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

   const std::string PORT_BUTTON_NAME = "portButton_0";
   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_TEXT = "PORT0";
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::SERIAL;
   port_settings.serialSettings.device = FIRST_SERIAL_PORT_LINK;
   port_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_115200;
   port_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;
   port_settings.serialSettings.mode = Drivers::Serial::DataMode::NEW_LINE_DELIMITER;
   port_settings.serialSettings.parityBits = Drivers::Serial::ParityType::NONE;
   port_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::ONE;
   port_settings.port_id = 0;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowBackground));
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowText));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), PORT_BUTTON_TEXT);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), PORT_BUTTON_TEXT + PortSettingDialog::Settings{}.shortSettingsString());
   //TODO check stylesheet colors before and after settings change

   EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), NEW_PORT_NAME + port_settings.shortSettingsString());

}
