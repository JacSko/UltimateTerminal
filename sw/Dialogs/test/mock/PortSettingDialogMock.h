#pragma once

#include <gmock/gmock.h>
#include "Logger.h"
#include "PortSettingDialog.h"

#undef DEF_PORT_TYPE
#define DEF_PORT_TYPE(a) #a,
std::vector<std::string> g_port_names = { DEF_PORT_TYPES };
#undef DEF_PORT_TYPE

#undef DEF_DATA_BIT
#define DEF_DATA_BIT(a) #a,
std::vector<std::string> g_databits_names = { DEF_DATA_BITS };
#undef DEF_DATA_BIT

#undef DEF_PARITY_BIT
#define DEF_PARITY_BIT(a) #a,
std::vector<std::string> g_paritybits_names = { DEF_PARITY_BITS };
#undef DEF_PARITY_BIT

#undef DEF_STOP_BIT
#define DEF_STOP_BIT(a) #a,
std::vector<std::string> g_stopbits_names = { DEF_STOP_BITS };
#undef DEF_STOP_BIT

template<>
std::string EnumValue<Dialogs::PortSettingDialog::PortType>::toName() const
{
   UT_Assert(value < Dialogs::PortSettingDialog::PortType::PORT_TYPE_MAX);
   return g_port_names[(size_t)value];
}
template<>
Dialogs::PortSettingDialog::PortType EnumValue<Dialogs::PortSettingDialog::PortType>::fromName(const std::string& name)
{
   value = Dialogs::PortSettingDialog::PortType::PORT_TYPE_MAX;
   auto it = std::find(g_port_names.begin(), g_port_names.end(), name);
   if (it != g_port_names.end())
   {
      value = (Dialogs::PortSettingDialog::PortType)(std::distance(g_port_names.begin(), it));
   }
   return value;
}

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
   DIALOG_ID++;
   m_current_settings.port_id = DIALOG_ID;
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
