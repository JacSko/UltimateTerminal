#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "LoggingSettingDialog.h"


struct LoggingSettingDialogMock
{
   MOCK_METHOD4(showDialog,std::optional<bool>(QWidget*, const LoggingSettingDialog::Settings&, LoggingSettingDialog::Settings&, bool));
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

LoggingSettingDialog::LoggingSettingDialog() {}
LoggingSettingDialog::~LoggingSettingDialog() {}

std::optional<bool> LoggingSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return g_log_setting_dialog_mock->showDialog(parent, current_settings, out_settings, allow_edit);
}
