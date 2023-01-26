#include "QtWidgets/QColorDialog"
#include <QtCore/QString>
#include "TraceFilterSettingDialog.h"
#include "Logger.h"
#include "GUIController.h"

namespace Dialogs
{

TraceFilterSettingDialog::TraceFilterSettingDialog()
{
}
TraceFilterSettingDialog::~TraceFilterSettingDialog()
{
}
std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return GUIController::onTraceFilterSettingDialogShow(parent, current_settings, out_settings, allow_edit);
}
QLayout* TraceFilterSettingDialog::createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit)
{
}
void TraceFilterSettingDialog::destroyLayout()
{
}
bool TraceFilterSettingDialog::convertGuiValues(Settings& out_settings)
{
}

}
