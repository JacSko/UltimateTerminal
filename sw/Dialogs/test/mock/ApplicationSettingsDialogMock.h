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

ApplicationSettingsDialog::ApplicationSettingsDialog(GUIController::GUIController& gui_controller,
                                                     std::vector<std::unique_ptr<MainApplication::Port>>& ports,
                                                     std::vector<std::unique_ptr<MainApplication::TraceFilter>>& filters,
                                                     std::unique_ptr<MainApplication::IFileLogger>& logger,
                                                     std::string& logging_path,
                                                     const std::string& persistence_path):
m_ports(ports),
m_filters(filters),
m_file_logging(logging_path, logger),
m_gui_controller(gui_controller),
m_persistence_path(persistence_path)
{

}
ApplicationSettingsDialog::~ApplicationSettingsDialog() {}

std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget* parent)
{
   return g_app_setting_dialog_mock->showDialog(parent);
}

}
