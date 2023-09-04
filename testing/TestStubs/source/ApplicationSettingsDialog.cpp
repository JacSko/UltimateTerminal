#include <array>
#include <string>
#include "ApplicationSettingsDialog.h"
#include "Settings.h"
#include "GUIController.h"

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(GUIController::GUIController& gui_controller,
                                                     std::vector<std::unique_ptr<MainApplication::Port>>& ports,
                                                     std::vector<std::unique_ptr<MainApplication::TraceFilter>>& filters,
                                                     std::unique_ptr<MainApplication::IFileLogger>& logger,
                                                     std::string& logging_path,
                                                     const std::string& persistence_path):
m_ports(ports),
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
