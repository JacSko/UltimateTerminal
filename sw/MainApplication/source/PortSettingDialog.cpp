#include "PortSettingDialog.h"
#include <arpa/inet.h>

#undef DEF_PORT_TYPE
#define DEF_PORT_TYPE(a) #a,
std::array<std::string, (size_t)PortSettingDialog::PortType::PORT_TYPE_MAX> g_port_names = { DEF_PORT_TYPES };
#undef DEF_PORT_TYPE

#undef DEF_DATA_BIT
#define DEF_DATA_BIT(a) #a,
std::array<std::string, (size_t)PortSettingDialog::DataBits::DATA_BIT_MAX> g_databits_names = { DEF_DATA_BITS };
#undef DEF_DATA_BIT

#undef DEF_PARITY_BIT
#define DEF_PARITY_BIT(a) #a,
std::array<std::string, (size_t)PortSettingDialog::ParityBits::PARITY_BIT_MAX> g_paritybits_names = { DEF_PARITY_BITS };
#undef DEF_PARITY_BIT

#undef DEF_STOP_BIT
#define DEF_STOP_BIT(a) #a,
std::array<std::string, (size_t)PortSettingDialog::StopBits::STOP_BIT_MAX> g_stopbits_names = { DEF_STOP_BITS };
#undef DEF_STOP_BIT

PortSettingDialog::PortSettingDialog(QWidget* parent):
m_dialog(parent),
m_form(&m_dialog),
m_portTypeBox(&m_dialog),
m_buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &m_dialog),
m_portNameEdit(&m_dialog),
m_deviceNameEdit(&m_dialog),
m_baudRateEdit(&m_dialog),
m_dataBitsEdit(&m_dialog),
m_parityBitsEdit(&m_dialog),
m_stopBitsEdit(&m_dialog),
m_ipAddressEdit(&m_dialog),
m_ipPortEdit(&m_dialog)
{
   m_portTypeBox.addItems({"SERIAL", "ETHERNET"});
   m_dataBitsEdit.addItems({"5", "6", "7", "8"});
   m_parityBitsEdit.addItems({"NONE", "EVEN", "ODD", "MARK", "SPACE"});
   m_stopBitsEdit.addItems({"1", "2"});
}
PortSettingDialog::~PortSettingDialog()
{

}
PortSettingDialog::Settings PortSettingDialog::showDialog(const Settings& current_settings)
{
   addPortTypeComboBox(current_settings.type);
   if (current_settings.type == PortType::SERIAL)
   {
      renderSerialView(&m_dialog, &m_form);
   }
   else
   {
      renderEthernetView(&m_dialog, &m_form);
   }
   addDialogButtons();

   if (m_dialog.exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "accepted, gathering new settings");
      return convertGuiValues();
   }
   else
   {
      return {};
   }
}
std::string PortSettingDialog::toString(PortType type)
{
   UT_Assert(type < PortType::PORT_TYPE_MAX && "Invalid port type");
   return g_port_names[(size_t)type];
}
std::string PortSettingDialog::toString(DataBits data)
{
   UT_Assert(data < DataBits::DATA_BIT_MAX && "Invalid data bits");
   return g_databits_names[(size_t)data];
}
std::string PortSettingDialog::toString(ParityBits parity)
{
   UT_Assert(parity < ParityBits::PARITY_BIT_MAX && "Invalid parity bits");
   return g_paritybits_names[(size_t)parity];
}
std::string PortSettingDialog::toString(StopBits stop)
{
   UT_Assert(stop < StopBits::STOP_BIT_MAX && "Invalid stop bits");
   return g_stopbits_names[(size_t)stop];
}
void PortSettingDialog::addPortTypeComboBox(PortType current_selection)
{
   QString porttype_label = QString("Port type:");
   m_portTypeBox.setCurrentText(QString(PortSettingDialog::toString(current_selection).c_str()));
   m_form.addRow(porttype_label, &m_portTypeBox);

   QObject::connect(&m_portTypeBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));
}
void PortSettingDialog::addDialogButtons()
{
   m_form.addWidget(&m_buttonBox);
   QObject::connect(&m_buttonBox, SIGNAL(accepted()), &m_dialog, SLOT(accept()));
   QObject::connect(&m_buttonBox, SIGNAL(rejected()), &m_dialog, SLOT(reject()));
}
void PortSettingDialog::renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit.setText(QString(settings.port_name.c_str()));
   form->insertRow(1, portname_label, &m_portNameEdit);
   m_current_widgets.push_back(&m_portNameEdit);

   QString device_label = QString("Device name:");
   m_deviceNameEdit.setText(QString(settings.device.c_str()));
   form->insertRow(2, device_label, &m_deviceNameEdit);
   m_current_widgets.push_back(&m_deviceNameEdit);

   QString baudrate_label = QString("Baudrate:");
   m_baudRateEdit.setText(QString(settings.baud_rate));
   form->insertRow(3, baudrate_label, &m_baudRateEdit);
   m_current_widgets.push_back(&m_baudRateEdit);

   QString databits_label = QString("Data Bits:");
   m_dataBitsEdit.setCurrentText(QString(PortSettingDialog::toString(settings.data_bits).c_str()));
   form->insertRow(4, databits_label, &m_dataBitsEdit);
   m_current_widgets.push_back(&m_dataBitsEdit);

   QString paritybits_label = QString("Parity Bits:");
   m_parityBitsEdit.setCurrentText(QString(PortSettingDialog::toString(settings.parity_bits).c_str()));
   form->insertRow(5, paritybits_label, &m_parityBitsEdit);
   m_current_widgets.push_back(&m_parityBitsEdit);

   QString stopbits_label = QString("Stop Bits:");
   m_stopBitsEdit.setCurrentText(QString(PortSettingDialog::toString(settings.stop_bits).c_str()));
   form->insertRow(6, stopbits_label, &m_stopBitsEdit);
   m_current_widgets.push_back(&m_stopBitsEdit);

}
void PortSettingDialog::renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit.setText(QString(settings.port_name.c_str()));
   form->insertRow(1, portname_label, &m_portNameEdit);
   m_current_widgets.push_back(&m_portNameEdit);

   QString device_label = QString("Device name:");
   m_deviceNameEdit.setText(QString(settings.device.c_str()));
   form->insertRow(2, device_label, &m_deviceNameEdit);
   m_current_widgets.push_back(&m_deviceNameEdit);

   QString address_label = QString("IP Address:");
   m_ipAddressEdit.setText(QString(settings.ip_address.c_str()));
   form->insertRow(3, address_label, &m_ipAddressEdit);
   m_current_widgets.push_back(&m_ipAddressEdit);

   QString port_label = QString("Port:");
   m_ipPortEdit.setText(QString(settings.port));
   form->insertRow(4, port_label, &m_ipPortEdit);
   m_current_widgets.push_back(&m_ipPortEdit);

}
void PortSettingDialog::clearDialog()
{
   for (auto item : m_current_widgets)
   {
      m_form.removeRow(item);
   }
   m_current_widgets.clear();
}
PortSettingDialog::Settings PortSettingDialog::convertGuiValues()
{
   PortSettingDialog::Settings result = {};
   result.type = stringToPortType(m_portTypeBox.currentText());
   result.baud_rate = m_baudRateEdit.text().toUInt();
   result.data_bits = stringToDataBits(m_dataBitsEdit.currentText());
   result.parity_bits = stringToParityBits(m_parityBitsEdit.currentText());
   result.stop_bits = stringToStopBits(m_stopBitsEdit.currentText());
   result.device = m_deviceNameEdit.text().toStdString();
   result.ip_address = m_ipAddressEdit.text().toStdString();
   result.port = m_ipPortEdit.text().toUInt();
   result.port_name = m_portNameEdit.text().toStdString();

   return (validateSettings(result, result.type)? result : Settings{});
}
bool PortSettingDialog::validateSettings(const PortSettingDialog::Settings& settings, PortType type)
{
   bool result = true;
   if (type == PortType::SERIAL)
   {
      result &= validateBaudRate(settings.baud_rate);
   }
   else if (type == PortType::ETHERNET)
   {
      result &= validateIpAddress(settings.ip_address);
      result &= validatePort(settings.port);
   }
   else
   {
      UT_Log(MAIN_GUI, ERROR, "Unknown port type %u", (uint8_t)settings.type);
      result = false;
   }
   return result;
}
bool PortSettingDialog::validateBaudRate(uint32_t baudrate)
{
   return baudrate > 0;
}
bool PortSettingDialog::validateIpAddress(const std::string& ip_address)
{
   struct sockaddr_in sa;
   int result = inet_pton(AF_INET, ip_address.c_str(), &(sa.sin_addr));
   return result != 0;
}
bool PortSettingDialog::validatePort(uint32_t port)
{
   return (port > 0) && (port < 99999);
}
void PortSettingDialog::onPortTypeChanged(const QString & name)
{
   UT_Log(MAIN_GUI, ERROR, "new port type %s, rendering", name.toStdString().c_str());

   if (stringToPortType(name) == PortType::SERIAL)
   {
      renderSerialView(&m_dialog, &m_form);
   }
   else
   {
      renderEthernetView(&m_dialog, &m_form);
   }
}
PortSettingDialog::PortType PortSettingDialog::stringToPortType(const QString& name)
{
   auto it = std::find(g_port_names.begin(), g_port_names.end(), name.toStdString());
   if (it != g_port_names.end())
   {
      return ((PortSettingDialog::PortType)std::distance(g_port_names.begin(), it));
   }
   else
   {
      return PortSettingDialog::PortType::PORT_TYPE_MAX;
   }
}
PortSettingDialog::DataBits PortSettingDialog::stringToDataBits(const QString& name)
{
   auto it = std::find(g_databits_names.begin(), g_databits_names.end(), name.toStdString());
   if (it != g_databits_names.end())
   {
      return ((PortSettingDialog::DataBits)std::distance(g_databits_names.begin(), it));
   }
   else
   {
      return PortSettingDialog::DataBits::DATA_BIT_MAX;
   }
}
PortSettingDialog::ParityBits PortSettingDialog::stringToParityBits(const QString& name)
{
   auto it = std::find(g_paritybits_names.begin(), g_paritybits_names.end(), name.toStdString());
   if (it != g_paritybits_names.end())
   {
      return ((PortSettingDialog::ParityBits)std::distance(g_paritybits_names.begin(), it));
   }
   else
   {
      return PortSettingDialog::ParityBits::PARITY_BIT_MAX;
   }
}
PortSettingDialog::StopBits PortSettingDialog::stringToStopBits(const QString& name)
{
   auto it = std::find(g_stopbits_names.begin(), g_stopbits_names.end(), name.toStdString());
   if (it != g_stopbits_names.end())
   {
      return ((PortSettingDialog::StopBits)std::distance(g_stopbits_names.begin(), it));
   }
   else
   {
      return PortSettingDialog::StopBits::STOP_BIT_MAX;
   }
}
