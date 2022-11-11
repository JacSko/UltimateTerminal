#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "LoggingSettingDialog.h"


struct LoggingSettingDialogMock
{
   MOCK_METHOD3(showDialog,std::optional<std::string>(QWidget*, const std::string&, bool));
};

LoggingSettingDialogMock* g_log_setting_dialog_mock;

void LoggingSettingDialogMock_init()
{
   if (!g_log_setting_dialog_mock)
   {
      g_log_setting_dialog_mock = new LoggingSettingDialogMock();
   }
}
void LoggingSettingDialogMock_deinit()
{
   if (g_log_setting_dialog_mock)
   {
      delete g_log_setting_dialog_mock;
      g_log_setting_dialog_mock = nullptr;
   }
}
LoggingSettingDialogMock* LoggingSettingDialogMock_get()
{
   UT_Assert(g_log_setting_dialog_mock && "Create PortSettingDialogMock mock first!");
   return g_log_setting_dialog_mock;

}

namespace Dialogs
{

LoggingSettingDialog::LoggingSettingDialog() {}
LoggingSettingDialog::~LoggingSettingDialog() {}

std::optional<std::string> LoggingSettingDialog::showDialog(QWidget* parent, const std::string& current_path, bool allow_edit)
{
   return g_log_setting_dialog_mock->showDialog(parent, current_path, allow_edit);
}

}
