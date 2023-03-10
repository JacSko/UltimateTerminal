#include "MessageDialog.h"

#include <QtWidgets/QMessageBox>
#include "GUITestServer.h"

namespace Dialogs
{

void MessageDialog::show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette)
{
   GUITestServer::onMessageBoxShow(icon, window_title, text, palette);
}

}
