#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "ApplicationSettingsDialog.h"


struct ApplicationSettingsDialogMock
{
   MOCK_METHOD1(showDialog,std::optional<bool>(QWidget*));

};

ApplicationSettingsDialogMock* g_app_setting_dialog_mock;

void ApplicationSettingsDialogMock_init()
{
   if (!g_app_setting_dialog_mock)
   {
      g_app_setting_dialog_mock = new ApplicationSettingsDialogMock();
   }
}
void ApplicationSettingsDialogMock_deinit()
{
   if (g_app_setting_dialog_mock)
   {
      delete g_app_setting_dialog_mock;
      g_app_setting_dialog_mock = nullptr;
   }
}
ApplicationSettingsDialogMock* ApplicationSettingsDialogMock_get()
{
   UT_Assert(g_app_setting_dialog_mock && "Create ApplicationSettingsDialogMock mock first!");
   return g_app_setting_dialog_mock;
}

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                                                     std::unique_ptr<IFileLogger>& logger,
                                                     std::string& logging_path):
m_handlers(ports),
m_filters(filters),
m_file_logging(logging_path, logger)
{

}
ApplicationSettingsDialog::~ApplicationSettingsDialog() {}

std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget* parent)
{
   return g_app_setting_dialog_mock->showDialog(parent);
}

}
