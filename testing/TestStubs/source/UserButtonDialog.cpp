#include <QtCore/QString>
#include "UserButtonDialog.h"
#include "GUITestServer.h"
#include <sstream>

namespace Dialogs
{

UserButtonDialog::UserButtonDialog()
{
}
UserButtonDialog::~UserButtonDialog()
{
}
std::optional<bool> UserButtonDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return GUITestServer::onUserButtonSettingsDialogShow(parent, current_settings, out_settings, allow_edit);
}
void UserButtonDialog::showHelp()
{

}
}
