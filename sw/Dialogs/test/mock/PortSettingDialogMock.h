#pragma once

#include <gmock/gmock.h>
#include "Logger.h"
#include "PortSettingDialog.h"

struct PortSettingDialogMock
{
   MOCK_METHOD4(showDialog,std::optional<bool>(QWidget*, const Dialogs::PortSettingDialog::Settings&, Dialogs::PortSettingDialog::Settings&, bool));
   MOCK_METHOD3(createLayout, QLayout*(QWidget* parent, const Dialogs::PortSettingDialog::Settings& current_settings, bool allow_edit));
   MOCK_METHOD0(destroyLayout, void());
   MOCK_METHOD2(convertGuiValues, bool(uint8_t id, Dialogs::PortSettingDialog::Settings&));
};

PortSettingDialogMock* g_port_setting_dialog_mock;
static uint8_t DIALOG_ID = 0;

void PortSettingDialogMock_init()
{
   if (!g_port_setting_dialog_mock)
   {
      g_port_setting_dialog_mock = new PortSettingDialogMock();
      DIALOG_ID = 0;
   }
}
void PortSettingDialogMock_deinit()
{
   if (g_port_setting_dialog_mock)
   {
      delete g_port_setting_dialog_mock;
      g_port_setting_dialog_mock = nullptr;
   }
}
PortSettingDialogMock* PortSettingDialogMock_get()
{
   UT_Assert(g_port_setting_dialog_mock && "Create PortSettingDialogMock mock first!");
   return g_port_setting_dialog_mock;

}

namespace Dialogs
{

PortSettingDialog::PortSettingDialog()
{
   m_current_settings.port_id = DIALOG_ID;
   DIALOG_ID++;
}
PortSettingDialog::~PortSettingDialog() {}

std::optional<bool> PortSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return g_port_setting_dialog_mock->showDialog(parent, current_settings, out_settings, allow_edit);
}
QLayout* PortSettingDialog::createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit)
{
   return g_port_setting_dialog_mock->createLayout(parent, current_settings, allow_edit);
}
void PortSettingDialog::destroyLayout()
{
   g_port_setting_dialog_mock->destroyLayout();
}
bool PortSettingDialog::convertGuiValues(Settings& out_settings)
{
   return g_port_setting_dialog_mock->convertGuiValues(m_current_settings.port_id, out_settings);
}
}
