#include "PortSettingDialog.h"

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>

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
m_form(&m_dialog)
{

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
      UT_Log(MAIN, ERROR, "accepted!");
   }
}
void PortSettingDialog::addPortTypeComboBox(PortType current_selection)
{
   QString porttype_label = QString("Port type:");
   QComboBox* porttype_box = new QComboBox(&m_dialog);
   porttype_box->addItems({"SERIAL", "ETHERNET"});
   porttype_box->setCurrentText(QString(g_port_names[(size_t)current_selection].c_str()));
   m_form.addRow(porttype_label, porttype_box);

   QObject::connect(porttype_box, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));
}
void PortSettingDialog::addDialogButtons()
{
   QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &m_dialog);

   m_form.addWidget(buttonBox);
   QObject::connect(buttonBox, SIGNAL(accepted()), &m_dialog, SLOT(accept()));
   QObject::connect(buttonBox, SIGNAL(rejected()), &m_dialog, SLOT(reject()));
}
void PortSettingDialog::renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   QLineEdit* portname_edit = new QLineEdit(dialog);
   portname_edit->setText(QString(settings.port_name.c_str()));
   form->insertRow(1, portname_label, portname_edit);
   m_current_widgets.push_back(portname_edit);

   QString device_label = QString("Device name:");
   QLineEdit* device_edit = new QLineEdit(dialog);
   device_edit->setText(QString(settings.device.c_str()));
   form->insertRow(2, device_label, device_edit);
   m_current_widgets.push_back(device_edit);

   QString baudrate_label = QString("Baudrate:");
   QLineEdit* baudrate_edit = new QLineEdit(dialog);
   baudrate_edit->setText(QString(settings.baud_rate));
   form->insertRow(3, baudrate_label, baudrate_edit);
   m_current_widgets.push_back(baudrate_edit);

   QString databits_label = QString("Data Bits:");
   QComboBox* databits_box = new QComboBox(dialog);
   databits_box->addItems({"5", "6", "7", "8"});
   databits_box->setCurrentText(QString(settings.data_bits));
   form->insertRow(4, databits_label, databits_box);
   m_current_widgets.push_back(databits_box);

   QString paritybits_label = QString("Parity Bits:");
   QComboBox* paritybits_box = new QComboBox(dialog);
   paritybits_box->addItems({"NONE", "EVEN", "ODD", "MARK", "SPACE"});
   paritybits_box->setCurrentText(QString(settings.parity_bits));
   form->insertRow(5, paritybits_label, paritybits_box);
   m_current_widgets.push_back(paritybits_box);

   QString stopbits_label = QString("Stop Bits:");
   QComboBox* stopbits_box = new QComboBox(dialog);
   stopbits_box->addItems({"1", "2"});
   stopbits_box->setCurrentText(QString(settings.stop_bits));
   form->insertRow(6, stopbits_label, stopbits_box);
   m_current_widgets.push_back(stopbits_box);

}
void PortSettingDialog::renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   QLineEdit* portname_edit = new QLineEdit(dialog);
   portname_edit->setText(QString(settings.port_name.c_str()));
   form->insertRow(1, portname_label, portname_edit);
   m_current_widgets.push_back(portname_edit);

   QString device_label = QString("Device name:");
   QLineEdit* device_edit = new QLineEdit(dialog);
   device_edit->setText(QString(settings.device.c_str()));
   form->insertRow(2, device_label, device_edit);
   m_current_widgets.push_back(device_edit);

   QString address_label = QString("IP Address:");
   QLineEdit* address_edit = new QLineEdit(dialog);
   address_edit->setText(QString(settings.ip_address.c_str()));
   form->insertRow(3, address_label, address_edit);
   m_current_widgets.push_back(address_edit);

   QString port_label = QString("Port:");
   QLineEdit* port_edit = new QLineEdit(dialog);
   port_edit->setText(QString(settings.port));
   form->insertRow(4, port_label, port_edit);
   m_current_widgets.push_back(port_edit);

}
void PortSettingDialog::clearDialog()
{
   for (auto item : m_current_widgets)
   {
      m_form.removeRow(item);
   }
   m_current_widgets.clear();
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
