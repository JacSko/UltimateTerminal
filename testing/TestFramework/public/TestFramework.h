#pragma once
#include <string>
#include "PortSettingDialog.h"
#include "TraceFilterSettingDialog.h"
#include "UserButtonDialog.h"
#include "MessageBox.h"
#include "ISerialDriver.h"

namespace TF
{

bool Connect();
void Disconnect();
void BeginTest();
void FinishTest();

namespace Serial
{
bool startForwarding(const std::string& device1, const std::string& device2);
bool stopForwarding(const std::string& device1, const std::string& device2);
bool openSerialPort(const Drivers::Serial::Settings& settings);
bool closeSerialPort(const std::string& device);
bool sendMessage(const std::string& device, const std::string& message);
}

namespace Buttons
{
uint32_t getBackgroundColor(const std::string& name);
uint32_t getFontColor(const std::string& name);
bool isEnabled(const std::string& name);
bool isChecked(const std::string& name);
bool isCheckable(const std::string& name);
std::string getText(const std::string& name);

bool simulateButtonClick(const std::string& name);
bool simulateContextMenuClick(const std::string& name);
}

namespace Common
{
std::string getCommand();
bool setCommand(const std::string& command);

bool isLineEndingVisible(const std::string ending);
bool setLineEnding(const std::string ending);
std::string getLineEnding();

bool isTargetPortVisible(const std::string ending);
bool setTargetPort(const std::string ending);
std::string getTargetPort();
uint32_t countTargetPorts();

uint32_t countCommandHistory();
bool isCommandInHistory(const std::string& command, uint32_t index = UINT32_MAX);

std::string getMessageBoxText();
std::string getMessageBoxTitle();
Dialogs::MessageBox::Icon getMessageBoxIcon();
bool resetMessageBox();
bool setLoggingPath(const std::string& path);
std::string getLoggingPath();

}

namespace Ports
{
std::string getLabelText(uint8_t id);
std::string getLabelStylesheet(uint8_t id);
Dialogs::PortSettingDialog::Settings getPortSettings(uint8_t port_id);
bool setPortSettings(uint8_t port_id, const Dialogs::PortSettingDialog::Settings&);
}

namespace TraceFilters
{
std::string getText(const std::string& filter_name);
uint32_t getBackgroundColor(const std::string& filter_name);
uint32_t getFontColor(const std::string& filter_name);
bool isEditable(const std::string& filter_name);
bool setText(const std::string& filter_name, const std::string& filter);
bool setSettings(uint8_t id, const Dialogs::TraceFilterSettingDialog::Settings& settings);
Dialogs::TraceFilterSettingDialog::Settings getSettings(uint8_t id);
}

namespace TerminalView
{
uint32_t countItems();
uint32_t countItemsWithBackgroundColor(uint32_t color);
uint32_t countItemsWithFontColor(uint32_t color);
uint32_t countItemsWithText(const std::string& text);
}

namespace TraceView
{
uint32_t countItems();
uint32_t countItemsWithBackgroundColor(uint32_t color);
uint32_t countItemsWithFontColor(uint32_t color);
uint32_t countItemsWithText(const std::string& text);
}

namespace UserButtons
{
bool setSettings(uint8_t id, const Dialogs::UserButtonDialog::Settings& settings);
Dialogs::UserButtonDialog::Settings getSettings(uint8_t id);
}

}
