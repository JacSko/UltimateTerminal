#pragma once
#include <string>

namespace TF
{

bool Connect();
void Disconnect();

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

}

namespace Ports
{
std::string getLabelText(uint8_t id);
std::string getLabelStylesheet(uint8_t id);
}

namespace TraceFilters
{
std::string getText(const std::string& filter_name);
uint32_t getBackgroundColor(const std::string& filter_name);
uint32_t getFontColor(const std::string& filter_name);
bool isEditable(const std::string& filter_name);
bool setText(const std::string& filter_name, const std::string& filter);
}

}
