#include "gtest/gtest.h"
#include "TestFramework.h"
#include "Settings.h"

const std::string FIRST_SERIAL_PORT_LINK = std::string(RUNTIME_OUTPUT_DIR) + "/link1";
const std::string SECOND_SERIAL_PORT_LINK = std::string(RUNTIME_OUTPUT_DIR) + "/link2";
static constexpr uint32_t GREEN_COLOR = 0x00FF00;
static constexpr uint32_t BLACK_COLOR = 0x000000;
static constexpr uint32_t TEST_TRACES_COUNT = 10;

struct TestParam
{
   uint8_t port_id;
   uint32_t background_color;
   uint32_t font_color;
   Drivers::Serial::Settings serial_settings;
   friend std::ostream& operator<<(std::ostream& ost, const TestParam& param)
   {
      ost << "Test with settings: " << std::endl;
      return ost;
   }

};

struct SerialPortReadWrite : public testing::TestWithParam<TestParam>
{
   static void SetUpTestSuite()
   {
      ASSERT_TRUE(TF::Connect());
      EXPECT_TRUE(TF::Serial::startForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
      TF::wait(1000);
   }
   static void TearDownTestSuite()
   {
      EXPECT_TRUE(TF::Serial::stopForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
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

TEST_P(SerialPortReadWrite, read_write_serial_port)
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

   const uint8_t PORT_ID = GetParam().port_id;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string PORT_BUTTON_TEXT = "PORT" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);
   const uint32_t BACKGROUND_COLOR = GetParam().background_color;
   const uint32_t FONT_COLOR = GetParam().font_color;

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::SERIAL;
   port_settings.serialSettings = GetParam().serial_settings;
   port_settings.port_id = PORT_ID;
   port_settings.trace_color = BACKGROUND_COLOR;
   port_settings.font_color = FONT_COLOR;

   /* check port button and label before test */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowBackground));
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowText));
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

   /* open serial driver for data generation */
   Drivers::Serial::Settings driver_settings = GetParam().serial_settings;
   driver_settings.device = SECOND_SERIAL_PORT_LINK;
   EXPECT_TRUE(TF::Serial::openSerialPort(driver_settings));

   /* start generating data on serial port */
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Serial::sendMessage(SECOND_SERIAL_PORT_LINK, "TEST MESSAGE " + std::to_string(i) + '\n'));
      TF::wait(20);
   }

   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItemsWithBackgroundColor(BACKGROUND_COLOR), TEST_TRACES_COUNT);
   EXPECT_EQ(TF::TerminalView::countItemsWithFontColor(FONT_COLOR), TEST_TRACES_COUNT);

   /* close serial port */
   EXPECT_TRUE(TF::Serial::closeSerialPort(driver_settings.device));

   /* close serial port in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* check button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowBackground));
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), SETTING_GET_U32(GUI_Dark_WindowText));
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

}

TestParam params [] = {

{0, 0x000001, 0x000002, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}},
{1, 0x000003, 0x000004, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}},
{2, 0x000005, 0x000006, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}},
{3, 0x000007, 0x000008, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}},
{4, 0x000009, 0x00000A, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}}

};

INSTANTIATE_TEST_CASE_P(SerialPortReadWrite, SerialPortReadWrite, testing::ValuesIn(params));
















