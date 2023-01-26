#include <QtCore/QObject>

#include "LoggingSettingDialog.h"
#include "Logger.h"
#include "GUIController.h"


namespace Dialogs
{

LoggingSettingDialog::LoggingSettingDialog()
{
}
LoggingSettingDialog::~LoggingSettingDialog()
{

}
std::optional<std::string> LoggingSettingDialog::showDialog(QWidget* parent, const std::string& current_path, bool allow_edit)
{
   return GUIController::onLoggingPathDialogShow(parent, current_path, allow_edit);
}
QLayout* LoggingSettingDialog::createLayout(const std::string& current_path, bool allow_edit)
{
   UT_Assert(false);
   return nullptr;
}
void LoggingSettingDialog::destroyLayout()
{
   UT_Assert(false);
}
std::string LoggingSettingDialog::convertGuiValues()
{
   UT_Assert(false);
   return "";
}

}
