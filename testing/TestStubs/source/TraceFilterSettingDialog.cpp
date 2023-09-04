#include "QtWidgets/QColorDialog"
#include <QtCore/QString>
#include "TraceFilterSettingDialog.h"
#include "GUITestServer.h"

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
   return GUITestServer::onTraceFilterSettingDialogShow(parent, current_settings, out_settings, allow_edit);
}
QLayout* TraceFilterSettingDialog::createLayout(QWidget*, const Settings&, bool)
{
   return nullptr;
}
void TraceFilterSettingDialog::destroyLayout()
{
}
bool TraceFilterSettingDialog::convertGuiValues(Settings&)
{
   return true;
}
void TraceFilterSettingDialog::onBackgroundColorButtonClicked(){}
void TraceFilterSettingDialog::onFontColorButtonClicked(){}

}
