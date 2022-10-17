#include "PortSettingDialog.h"
#include "QtWidgets/QColorDialog"

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

PortSettingDialog::PortSettingDialog():
m_dialog(nullptr),
m_form(nullptr),
m_portTypeBox(nullptr),
m_buttonBox(nullptr),
m_portNameEdit(nullptr),
m_deviceNameEdit(nullptr),
m_baudRateEdit(nullptr),
m_dataBitsBox(nullptr),
m_parityBitsBox(nullptr),
m_stopBitsBox(nullptr),
m_ipAddressEdit(nullptr),
m_ipPortEdit(nullptr),
m_colorSelectionButton(nullptr),
m_editable(true)
{
}
PortSettingDialog::~PortSettingDialog()
{

}
std::optional<bool> PortSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   std::optional<bool> result;
   m_dialog = new QDialog(parent);
   m_form = new QFormLayout(m_dialog);
   m_editable = allow_edit;

   addPortTypeComboBox(current_settings.type);

   if (current_settings.type == PortType::SERIAL)
   {
      renderSerialView(m_dialog, m_form, current_settings);
   }
   else if (current_settings.type == PortType::ETHERNET)
   {
      renderEthernetView(m_dialog, m_form, current_settings);
   }
   else
   {
      UT_Assert(false && "Invalid port type");
   }

   addDialogButtons();

   m_dialog->setWindowModality(Qt::ApplicationModal);
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "accepted, gathering new settings");
      result = convertGuiValues(out_settings);
   }

   clearDialog();
   delete m_form;
   delete m_dialog;

   return result;
}
std::string PortSettingDialog::toString(PortType type)
{
   UT_Assert(type < PortType::PORT_TYPE_MAX && "Invalid port type");
   return g_port_names[(size_t)type];
}
void PortSettingDialog::addPortTypeComboBox(PortType current_selection)
{
   QString porttype_label = QString("Port type:");
   m_portTypeBox = new QComboBox(m_dialog);

   for (const auto& name : g_port_names)
   {
      m_portTypeBox->addItem(QString(name.c_str()));
   }

   m_portTypeBox->setCurrentText(QString(PortSettingDialog::toString(current_selection).c_str()));
   m_portTypeBox->setDisabled(!m_editable);
   m_form->addRow(porttype_label, m_portTypeBox);

   QObject::connect(m_portTypeBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));
}
void PortSettingDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_buttonBox->setDisabled(!m_editable);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
}
void PortSettingDialog::addItemsToComboBox(QComboBox* box, const std::vector<std::string>& values)
{
   for (const auto& name : values)
   {
      box->addItem(QString(name.c_str()));
   }
}
void PortSettingDialog::renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_dialog);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(20);
   form->insertRow(1, portname_label, m_portNameEdit);
   m_current_widgets.push_back(m_portNameEdit);

   QString device_label = QString("Device name:");
   m_deviceNameEdit = new QLineEdit(m_dialog);
   m_deviceNameEdit->setText(QString(settings.serialSettings.device.c_str()));
   m_deviceNameEdit->setDisabled(!m_editable);
   form->insertRow(2, device_label, m_deviceNameEdit);
   m_current_widgets.push_back(m_deviceNameEdit);

   QString baudrate_label = QString("Baudrate:");
   m_baudRateEdit = new QLineEdit(m_dialog);
   m_baudRateEdit->setText(QString::number(settings.serialSettings.baudRate));
   m_baudRateEdit->setDisabled(!m_editable);
   form->insertRow(3, baudrate_label, m_baudRateEdit);
   m_current_widgets.push_back(m_baudRateEdit);

   QString databits_label = QString("Data Bits:");
   m_dataBitsBox = new QComboBox(m_dialog);
   addItemsToComboBox(m_dataBitsBox, g_databits_names);
   m_dataBitsBox->setCurrentText(QString(settings.serialSettings.dataBits.toName().c_str()));
   m_dataBitsBox->setDisabled(!m_editable);
   form->insertRow(4, databits_label, m_dataBitsBox);
   m_current_widgets.push_back(m_dataBitsBox);

   QString paritybits_label = QString("Parity Bits:");
   m_parityBitsBox = new QComboBox(m_dialog);
   addItemsToComboBox(m_parityBitsBox, g_paritybits_names);
   m_parityBitsBox->setCurrentText(QString(settings.serialSettings.parityBits.toName().c_str()));
   m_parityBitsBox->setDisabled(!m_editable);
   form->insertRow(5, paritybits_label, m_parityBitsBox);
   m_current_widgets.push_back(m_parityBitsBox);

   QString stopbits_label = QString("Stop Bits:");
   m_stopBitsBox = new QComboBox(m_dialog);
   addItemsToComboBox(m_stopBitsBox, g_stopbits_names);
   m_stopBitsBox->setCurrentText(QString(settings.serialSettings.stopBits.toName().c_str()));
   m_stopBitsBox->setDisabled(!m_editable);
   form->insertRow(6, stopbits_label, m_stopBitsBox);
   m_current_widgets.push_back(m_stopBitsBox);

   QString color_label = QString("Terminal color");
   m_colorSelectionButton = new QPushButton(m_dialog);
   m_colorSelectionButton->setText(QString("Click!"));
   m_colorSelectionButton->setDisabled(!m_editable);
   QPalette palette = m_colorSelectionButton->palette();
   palette.setColor(QPalette::Button, QColor(settings.trace_color));
   m_colorSelectionButton->setPalette(palette);
   m_colorSelectionButton->update();
   form->insertRow(7, color_label, m_colorSelectionButton);
   m_current_widgets.push_back(m_colorSelectionButton);
   QObject::connect(m_colorSelectionButton, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));


}
void PortSettingDialog::renderEthernetView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_dialog);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(20);
   form->insertRow(1, portname_label, m_portNameEdit);
   m_current_widgets.push_back(m_portNameEdit);

   QString address_label = QString("IP Address:");
   m_ipAddressEdit = new QLineEdit(m_dialog);
   m_ipAddressEdit->setText(QString(settings.ip_address.c_str()));
   m_ipAddressEdit->setDisabled(!m_editable);
   form->insertRow(2, address_label, m_ipAddressEdit);
   m_current_widgets.push_back(m_ipAddressEdit);

   QString port_label = QString("Port:");
   m_ipPortEdit = new QLineEdit(m_dialog);
   m_ipPortEdit->setText(QString::number(settings.port));
   m_ipPortEdit->setDisabled(!m_editable);
   form->insertRow(3, port_label, m_ipPortEdit);
   m_current_widgets.push_back(m_ipPortEdit);

   QString color_label = QString("Terminal color");
   m_colorSelectionButton = new QPushButton(m_dialog);
   m_colorSelectionButton->setText(QString("Click!"));
   m_colorSelectionButton->setDisabled(!m_editable);
   QPalette palette = m_colorSelectionButton->palette();
   palette.setColor(QPalette::Button, QColor(settings.trace_color));
   m_colorSelectionButton->setPalette(palette);
   m_colorSelectionButton->update();
   form->insertRow(4, color_label, m_colorSelectionButton);
   m_current_widgets.push_back(m_colorSelectionButton);
   QObject::connect(m_colorSelectionButton, SIGNAL(clicked()), this, SLOT(onColorButtonClicked()));

}
void PortSettingDialog::onColorButtonClicked()
{
   QColor color = QColorDialog::getColor(QColor(0xFF0000), m_dialog, "test title");

   if (color.isValid())
   {
      QPalette palette = m_colorSelectionButton->palette();
      palette.setColor(QPalette::Button, color);
      m_colorSelectionButton->setPalette(palette);
      m_colorSelectionButton->update();
   }

}
void PortSettingDialog::clearDialog()
{
   for (auto item : m_current_widgets)
   {
      m_form->removeRow(item);
   }
   m_current_widgets.clear();
}
bool PortSettingDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.type = stringToPortType(m_portTypeBox->currentText());

   if (out_settings.type == PortType::SERIAL)
   {
      out_settings.serialSettings.baudRate = m_baudRateEdit->text().toUInt();
      std::string text = m_baudRateEdit->text().toStdString();
      out_settings.serialSettings.dataBits.fromName(m_dataBitsBox->currentText().toStdString());
      out_settings.serialSettings.parityBits.fromName(m_parityBitsBox->currentText().toStdString());
      out_settings.serialSettings.stopBits.fromName(m_stopBitsBox->currentText().toStdString());
      out_settings.serialSettings.device = m_deviceNameEdit->text().toStdString();
   }
   else if (out_settings.type == PortType::ETHERNET)
   {
      out_settings.ip_address = m_ipAddressEdit->text().toStdString();
      out_settings.port = m_ipPortEdit->text().toUInt();
   }

   out_settings.port_name = m_portNameEdit->text().toStdString();
   out_settings.trace_color = m_colorSelectionButton->palette().color(QPalette::Button).rgb();
   bool result = out_settings.areValid()? true : false;
   return result;
}
void PortSettingDialog::onPortTypeChanged(const QString & name)
{
   UT_Log(MAIN_GUI, ERROR, "new port type %s, rendering", name.toStdString().c_str());

   if (stringToPortType(name) == PortType::SERIAL)
   {
      renderSerialView(m_dialog, m_form);
   }
   else
   {
      renderEthernetView(m_dialog, m_form);
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
