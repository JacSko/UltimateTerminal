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


}
