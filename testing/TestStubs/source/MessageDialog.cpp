#include "MessageDialog.h"

#include <QtWidgets/QMessageBox>
#include "GUIController.h"

namespace Dialogs
{

void MessageDialog::show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette)
{
   GUIController::onMessageBoxShow(icon, window_title, text, palette);
}

}
