#include "Settings.h"

#include "PortSettingDialog.h"
#include "GUITestServer.h"

namespace Dialogs
{

PortSettingDialog::PortSettingDialog()
{
}
PortSettingDialog::~PortSettingDialog()
{

}
std::optional<bool> PortSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return GUITestServer::onPortSettingsDialogShow(parent, current_settings, out_settings, allow_edit);
}
QLayout* PortSettingDialog::createLayout(QWidget*, const Settings&, bool)
{
   return nullptr;
}
void PortSettingDialog::destroyLayout()
{
}
bool PortSettingDialog::convertGuiValues(Settings&)
{
   return false;
}
void PortSettingDialog::onPortTypeChanged(const QString&) {};
void PortSettingDialog::onBackgroundColorButtonClicked() {};
void PortSettingDialog::onFontColorButtonClicked() {};


}
