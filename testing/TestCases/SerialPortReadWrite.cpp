#include "gtest/gtest.h"
#include "TestFramework.h"
#include "ApplicationExecutor.hpp"

const std::string FIRST_SERIAL_PORT_LINK = std::string(RUNTIME_OUTPUT_DIR) + "/link1";
const std::string SECOND_SERIAL_PORT_LINK = std::string(RUNTIME_OUTPUT_DIR) + "/link2";
static constexpr uint32_t GREEN_COLOR = 0x00FF00;
static constexpr uint32_t BLACK_COLOR = 0x000000;
static constexpr uint32_t TEST_TRACES_COUNT = 5;


/**
 * Reading from serial port - test fixture
 */
struct ReadTestParam
{
   uint8_t port_id;
   uint32_t background_color;
   uint32_t font_color;
   Drivers::Serial::Settings serial_settings;
   friend std::ostream& operator<<(std::ostream& ost, const ReadTestParam& param)
   {
      ost << "Test with settings: " << std::endl;
      ost << "id: " << param.serial_settings.device << std::endl;
      ost << "id: " << (int)param.port_id << std::endl;
      ost << "bg: " << (int)param.background_color << std::endl;
      ost << "font: " << (int)param.font_color << std::endl;
      return ost;
   }
};

/**
 * Writing to serial port - test fixture
 */
struct WriteTestParam
{
   uint8_t first_port_id;
   uint32_t first_background_color;
   uint32_t first_font_color;
   Drivers::Serial::Settings first_serial_settings;

   uint8_t second_port_id;
   uint32_t second_background_color;
   uint32_t second_font_color;
   Drivers::Serial::Settings second_serial_settings;

   friend std::ostream& operator<<(std::ostream& ost, const WriteTestParam& param)
   {
      ost << "Test with settings: " << std::endl;
      ost << "first_port_id: " << (int)param.first_port_id << std::endl;
      ost << "second_port_id: " << (int)param.second_port_id << std::endl;
      return ost;
   }
};

struct SerialPortTests : public testing::Test
{
   static void SetUpTestSuite()
   {
      TF::Init();
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(500);
      ASSERT_TRUE(TF::Connect());
      EXPECT_TRUE(TF::Serial::startForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
      TF::wait(1000);
   }
   static void TearDownTestSuite()
   {
      TF::Deinit();
      EXPECT_TRUE(TF::Serial::stopForwarding(FIRST_SERIAL_PORT_LINK, SECOND_SERIAL_PORT_LINK));
      TF::StopTestSubject();
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

struct SerialPortWritingTests : public SerialPortTests,
                                public testing::WithParamInterface<WriteTestParam>
{};
struct SerialPortReadingTests : public SerialPortTests,
                                public testing::WithParamInterface<ReadTestParam>
{};

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

TEST_P(SerialPortReadingTests, read_serial_port)
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

   /* open serial driver for data generation */
   Drivers::Serial::Settings driver_settings = GetParam().serial_settings;
   driver_settings.device = SECOND_SERIAL_PORT_LINK;
   EXPECT_TRUE(TF::Serial::openSerialPort(driver_settings));

   /* clear terminal view before test */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("clearButton"));

   /* start generating data on serial port */
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Serial::sendMessage(SECOND_SERIAL_PORT_LINK, "TEST MESSAGE " + std::to_string(i) + '\n'));
      TF::wait(20);
   }

   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItems(), TEST_TRACES_COUNT);

   /* close serial port */
   EXPECT_TRUE(TF::Serial::closeSerialPort(driver_settings.device));

   /* close serial port in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));

   /* check button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(PORT_BUTTON_NAME), NEW_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(PORT_ID), port_settings.shortSettingsString());

}

TEST_F(SerialPortTests, open_close_serial_multiple_times_during_high_traffic)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Open and close PORT0 multiple times, generate high traffic on serial port. <br>
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
   const uint8_t RECONNECT_ATTEMPTS = 10;

   PortSettingDialog::Settings port_settings;
   port_settings.port_name = NEW_PORT_NAME;
   port_settings.type = PortSettingDialog::PortType::SERIAL;
   port_settings.serialSettings =  {FIRST_SERIAL_PORT_LINK,
                                    Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                    Drivers::Serial::BaudRate::BR_115200,
                                    Drivers::Serial::ParityType::NONE,
                                    Drivers::Serial::StopBitType::ONE,
                                    Drivers::Serial::DataBitType::EIGHT};
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

   /* start SerialDataGenerator - generate a string data every 2ms */
   TF::ApplicationExecutor serial_data_generator;
   EXPECT_TRUE(serial_data_generator.startApplication(std::string(RUNTIME_OUTPUT_DIR) + "/SerialDataGenerator",
                                                      "--device=" + SECOND_SERIAL_PORT_LINK
                                                    + " --baud=BR_115200 --data=EIGHT --payload_rate=2 --payload=\"THIS_IS_EXAMPLE_PAYLOAD\""));
   TF::wait(2000);

   for (uint8_t i = 0; i < RECONNECT_ATTEMPTS; i++)
   {
      /* clear terminal */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(CLEAR_BUTTON_NAME));
      EXPECT_EQ(TF::TerminalView::countItems(), 0);

      /* open port */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
      EXPECT_EQ(TF::Common::getTargetPort(), NEW_PORT_NAME);
      /* wait for data */
      TF::wait(2000);
      /* close port */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(1000);
      EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_PORT_NAME));
      /* check terminal content - payload generated every 2ms, gathering time 2s, so more around ~1000 traces shall be available */
      EXPECT_GT(TF::TerminalView::countItems(), 500);
   }
   EXPECT_TRUE(serial_data_generator.stopApplication());
   TF::wait(5000);

}

TEST_P(SerialPortWritingTests, write_serial_port)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Setup serial port loopback between two ports, open both and start sending/reading data. <br>
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

   const uint8_t FIRST_PORT_ID = GetParam().first_port_id;
   const uint8_t SECOND_PORT_ID = GetParam().second_port_id;
   const std::string FIRST_PORT_BUTTON_NAME = "portButton_" + std::to_string(FIRST_PORT_ID);
   const std::string SECOND_PORT_BUTTON_NAME = "portButton_" + std::to_string(SECOND_PORT_ID);
   const std::string FIRST_PORT_BUTTON_TEXT = "PORT" + std::to_string(FIRST_PORT_ID);
   const std::string SECOND_PORT_BUTTON_TEXT = "PORT" + std::to_string(SECOND_PORT_ID);
   const std::string NEW_FIRST_PORT_NAME = "NEW_NAME" + std::to_string(FIRST_PORT_ID);
   const std::string NEW_SECOND_PORT_NAME = "NEW_NAME" + std::to_string(SECOND_PORT_ID);
   const uint32_t FIRST_BACKGROUND_COLOR = GetParam().first_background_color;
   const uint32_t SECOND_BACKGROUND_COLOR = GetParam().second_background_color;
   const uint32_t FIRST_FONT_COLOR = GetParam().first_font_color;
   const uint32_t SECOND_FONT_COLOR = GetParam().second_font_color;

   PortSettingDialog::Settings first_port_settings;
   first_port_settings.port_name = NEW_FIRST_PORT_NAME;
   first_port_settings.type = PortSettingDialog::PortType::SERIAL;
   first_port_settings.serialSettings = GetParam().first_serial_settings;
   first_port_settings.port_id = FIRST_PORT_ID;
   first_port_settings.trace_color = FIRST_BACKGROUND_COLOR;
   first_port_settings.font_color = FIRST_FONT_COLOR;

   PortSettingDialog::Settings second_port_settings;
   second_port_settings.port_name = NEW_SECOND_PORT_NAME;
   second_port_settings.type = PortSettingDialog::PortType::SERIAL;
   second_port_settings.serialSettings = GetParam().second_serial_settings;
   second_port_settings.port_id = SECOND_PORT_ID;
   second_port_settings.trace_color = SECOND_BACKGROUND_COLOR;
   second_port_settings.font_color = SECOND_FONT_COLOR;

   /* check first port button and label before test */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(FIRST_PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(FIRST_PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(FIRST_PORT_BUTTON_NAME), FIRST_PORT_BUTTON_TEXT);
   EXPECT_EQ(TF::Ports::getLabelText(FIRST_PORT_ID), FIRST_PORT_BUTTON_TEXT + PortSettingDialog::Settings{}.shortSettingsString());
   /* check throughput label before test */
   EXPECT_EQ(TF::Ports::getThroughput(FIRST_PORT_ID), "");

   /* check second port button and label before test */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(SECOND_PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(SECOND_PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(SECOND_PORT_BUTTON_NAME), SECOND_PORT_BUTTON_TEXT);
   EXPECT_EQ(TF::Ports::getLabelText(SECOND_PORT_ID), SECOND_PORT_BUTTON_TEXT + PortSettingDialog::Settings{}.shortSettingsString());
   /* check throughput label before test */
   EXPECT_EQ(TF::Ports::getThroughput(SECOND_PORT_ID), "");

   /* set new first port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(FIRST_PORT_ID, first_port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(FIRST_PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(FIRST_PORT_BUTTON_NAME), NEW_FIRST_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(FIRST_PORT_ID), first_port_settings.shortSettingsString());

   /* set new second port settings */
   EXPECT_TRUE(TF::Ports::setPortSettings(SECOND_PORT_ID, second_port_settings));
   EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_EQ(TF::Buttons::getText(SECOND_PORT_BUTTON_NAME), NEW_SECOND_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(SECOND_PORT_ID), second_port_settings.shortSettingsString());

   /* open first port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(FIRST_PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_FIRST_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_FIRST_PORT_NAME);

   /* open second port by clicking on button */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_TRUE(TF::Common::isTargetPortVisible(NEW_SECOND_PORT_NAME));
   EXPECT_EQ(TF::Common::getTargetPort(), NEW_FIRST_PORT_NAME);

   /* check first port button state after open */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(FIRST_PORT_BUTTON_NAME), GREEN_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(FIRST_PORT_BUTTON_NAME), BLACK_COLOR);
   EXPECT_EQ(TF::Buttons::getText(FIRST_PORT_BUTTON_NAME), NEW_FIRST_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(FIRST_PORT_ID), first_port_settings.shortSettingsString());

   /* check second port button state after open */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(SECOND_PORT_BUTTON_NAME), GREEN_COLOR);
   EXPECT_EQ(TF::Buttons::getFontColor(SECOND_PORT_BUTTON_NAME), BLACK_COLOR);
   EXPECT_EQ(TF::Buttons::getText(SECOND_PORT_BUTTON_NAME), NEW_SECOND_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(SECOND_PORT_ID), second_port_settings.shortSettingsString());

   /* clear terminal view before test */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("clearButton"));

   /* check throughputs before test */
   TF::wait(500);
   EXPECT_EQ(TF::Ports::getThroughput(FIRST_PORT_ID), "0,00 B/s");
   EXPECT_EQ(TF::Ports::getThroughput(SECOND_PORT_ID), "0,00 B/s");

   /* send data from first to second port */
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));
   EXPECT_TRUE(TF::Common::setTargetPort(NEW_FIRST_PORT_NAME));
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Common::setCommand("this is command " + std::to_string(i)));
      EXPECT_TRUE(TF::Buttons::simulateButtonClick("sendButton"));
      TF::wait(20);
   }

   /* check if data received on second port */
   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItems(), TEST_TRACES_COUNT);

   /* clear terminal view before second test */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("clearButton"));

   /* send data from second to first port */
   EXPECT_TRUE(TF::Common::setTargetPort(NEW_SECOND_PORT_NAME));
   for (uint8_t i = 0; i < TEST_TRACES_COUNT; i++)
   {
      EXPECT_TRUE(TF::Common::setCommand("this is command " + std::to_string(i)));
      EXPECT_TRUE(TF::Buttons::simulateButtonClick("sendButton"));
      TF::wait(20);
   }

   /* check if data received on second port */
   TF::wait(1000);
   EXPECT_EQ(TF::TerminalView::countItems(), TEST_TRACES_COUNT);

   /* close first serial port in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(FIRST_PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_FIRST_PORT_NAME));

   /* close second serial port in application */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick(SECOND_PORT_BUTTON_NAME));
   EXPECT_FALSE(TF::Common::isTargetPortVisible(NEW_SECOND_PORT_NAME));

   /* check first port button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(FIRST_PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(FIRST_PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(FIRST_PORT_BUTTON_NAME), NEW_FIRST_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(FIRST_PORT_ID), first_port_settings.shortSettingsString());

   /* check second port button and label state */
   EXPECT_EQ(TF::Buttons::getBackgroundColor(SECOND_PORT_BUTTON_NAME), GUI_Dark_WindowBackground);
   EXPECT_EQ(TF::Buttons::getFontColor(SECOND_PORT_BUTTON_NAME), GUI_Dark_WindowText);
   EXPECT_EQ(TF::Buttons::getText(SECOND_PORT_BUTTON_NAME), NEW_SECOND_PORT_NAME);
   EXPECT_EQ(TF::Ports::getLabelText(SECOND_PORT_ID), second_port_settings.shortSettingsString());

   TF::wait(500);
   EXPECT_EQ(TF::Ports::getThroughput(FIRST_PORT_ID), "");
   EXPECT_EQ(TF::Ports::getThroughput(SECOND_PORT_ID), "");

}




ReadTestParam read_params [] = {

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

WriteTestParam write_params [] = {

{0, 0x100000, 0x200000, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT},
1, 0x300000, 0x400000, {SECOND_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}},

{2, 0x500000, 0x600000, {FIRST_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT},
3, 0x700000, 0x800000, {SECOND_SERIAL_PORT_LINK, Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
     Drivers::Serial::BaudRate::BR_115200, Drivers::Serial::ParityType::NONE, Drivers::Serial::StopBitType::ONE, Drivers::Serial::DataBitType::EIGHT}}
};


INSTANTIATE_TEST_CASE_P(SerialPortReadingTests, SerialPortReadingTests, testing::ValuesIn(read_params));
INSTANTIATE_TEST_CASE_P(SerialPortWritingTests, SerialPortWritingTests, testing::ValuesIn(write_params));
















