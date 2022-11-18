#include "ApplicationSettingsDialog.h"
#include "Logger.h"

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                                                     std::unique_ptr<IFileLogger>& logger):
m_port_handlers(ports),
m_trace_filters(filters),
m_file_logger(logger)
{
}
ApplicationSettingsDialog::~ApplicationSettingsDialog()
{

}
std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget* parent)
{
}


}
