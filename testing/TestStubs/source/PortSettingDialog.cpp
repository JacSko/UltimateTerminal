#include "Settings.h"

#include "PortSettingDialog.h"
#include "GUIController.h"

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

namespace Dialogs
{

PortSettingDialog::PortSettingDialog()
{
}
PortSettingDialog::~PortSettingDialog()
{

}
std::optional<bool> PortSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return GUIController::onPortSettingsDialogShow(parent, current_settings, out_settings, allow_edit);
}
QLayout* PortSettingDialog::createLayout(QWidget*, const Settings&, bool)
{
   UT_Assert(false);
   return nullptr;
}
void PortSettingDialog::destroyLayout()
{
   UT_Assert(false);
}
bool PortSettingDialog::convertGuiValues(Settings&)
{
   UT_Assert(false);
   return false;
}

}
