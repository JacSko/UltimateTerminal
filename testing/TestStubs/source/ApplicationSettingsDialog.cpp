#include <array>
#include <string>
#include "ApplicationSettingsDialog.h"
#include "Logger.h"
#include "Settings.h"
#include "GUIController.h"

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(GUIController&,
                                                     std::vector<std::unique_ptr<GUI::PortHandler>>&,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>&,
                                                     std::unique_ptr<IFileLogger>&,
                                                     std::string&)
{
}
ApplicationSettingsDialog::~ApplicationSettingsDialog()
{

}
std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget*)
{
   return {};
}

}
