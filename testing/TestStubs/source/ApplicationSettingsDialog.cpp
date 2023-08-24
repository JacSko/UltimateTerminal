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
                                                     std::string& logging_path,
                                                     const std::string& persistence_path):
m_handlers(ports),
m_filters(filters),
m_file_logging(logging_path, logger),
m_gui_controller(gui_controller),
m_theme_combobox(nullptr),
m_max_traces_edit(nullptr),
m_persistence_path(persistence_path)
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
