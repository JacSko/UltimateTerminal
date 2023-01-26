#include <array>
#include <string>
#include "ApplicationSettingsDialog.h"
#include "Logger.h"
#include "Settings.h"
#include "GUIController.h"

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(GUIController& gui_controller,
                                                     std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                                                     std::unique_ptr<IFileLogger>& logger,
                                                     std::string& logging_path)
{
}
ApplicationSettingsDialog::~ApplicationSettingsDialog()
{

}
std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget* parent)
{
   return {};
}

}
