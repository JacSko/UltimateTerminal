#include "TabNameDialog.h"
#include "GUITestServer.h"
#include "Logger.h"

namespace Dialogs
{

TabNameDialog::TabNameDialog()
{
}
TabNameDialog::~TabNameDialog()
{
}
std::optional<std::string> TabNameDialog::showDialog(QWidget* parent, const std::string& current_name)
{
   return GUITestServer::onTabNameDialogShow(parent, current_name);
}
}
