#include "Settings.h"

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

template<>
std::string EnumValue<PortSettingDialog::PortType>::toName() const
{
   UT_Assert(value < PortSettingDialog::PortType::PORT_TYPE_MAX);
   return g_port_names[(size_t)value];
}
template<>
PortSettingDialog::PortType EnumValue<PortSettingDialog::PortType>::fromName(const std::string& name)
{
   value = PortSettingDialog::PortType::PORT_TYPE_MAX;
   auto it = std::find(g_port_names.begin(), g_port_names.end(), name);
   if (it != g_port_names.end())
   {
      value = (PortSettingDialog::PortType)(std::distance(g_port_names.begin(), it));
   }
   return value;
}

PortSettingDialog::PortSettingDialog():
m_dialog(nullptr),
m_form(nullptr),
m_portTypeBox(nullptr),
m_buttonBox(nullptr),
m_portNameEdit(nullptr),
m_deviceNameEdit(nullptr),
m_baudRateBox(nullptr),
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
   m_current_settings = current_settings;
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
   UT_Log(GUI_DIALOG, INFO, "%s for [%s] edit possible: %u", __func__, current_settings.port_name.c_str(), allow_edit);
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(GUI_DIALOG, HIGH, "dialog accepted, gathering new settings");
      result = convertGuiValues(out_settings);
   }

   clearDialog();
   delete m_form;
   delete m_dialog;
   UT_Log(GUI_DIALOG, INFO, "%s result %s", __func__, result.has_value()? (result.value()? "OK" : "NOK") : "NO_VALUE");
   return result;
}
void PortSettingDialog::addPortTypeComboBox(const EnumValue<PortType>& current_selection)
{
   QString porttype_label = QString("Port type:");
   m_portTypeBox = new QComboBox(m_dialog);

   for (const auto& name : g_port_names)
   {
      m_portTypeBox->addItem(QString(name.c_str()));
   }

   m_portTypeBox->setCurrentText(QString(current_selection.toName().c_str()));
   m_portTypeBox->setDisabled(!m_editable);
   m_form->addRow(porttype_label, m_portTypeBox);

   QObject::connect(m_portTypeBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));

   UT_Log(GUI_DIALOG, HIGH, "adding %s combobox with %u items", porttype_label.toStdString().c_str(), g_port_names.size());
}
void PortSettingDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_buttonBox->setDisabled(!m_editable);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));

   UT_Log(GUI_DIALOG, HIGH, "%s", __func__);
}
void PortSettingDialog::renderSerialView(QDialog* dialog, QFormLayout* form, const Settings& settings)
{
   UT_Log(GUI_DIALOG, LOW, "rendering view for SERIAL, settings %s", settings.shortSettingsString().c_str());
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_dialog);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(SETTING_GET_U32(PortSettingDialog_maxLineEditLength));
   form->insertRow(1, portname_label, m_portNameEdit);
   m_current_widgets.push_back(m_portNameEdit);

   QString device_label = QString("Device name:");
   m_deviceNameEdit = new QLineEdit(m_dialog);
   m_deviceNameEdit->setText(QString(settings.serialSettings.device.c_str()));
   m_deviceNameEdit->setDisabled(!m_editable);
   form->insertRow(2, device_label, m_deviceNameEdit);
   m_current_widgets.push_back(m_deviceNameEdit);

   QString baudrate_label = QString("Baudrate:");
   m_baudRateBox = new QComboBox(m_dialog);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::BaudRate::BAUDRATE_MAX; i++)
   {
      EnumValue<Drivers::Serial::BaudRate> item = (Drivers::Serial::BaudRate)i;
      m_baudRateBox->addItem(QString(item.toName().c_str()));
   }
   m_baudRateBox->setCurrentText(QString(settings.serialSettings.baudRate.toName().c_str()));
   m_baudRateBox->setDisabled(!m_editable);
   form->insertRow(3, baudrate_label, m_baudRateBox);
   m_current_widgets.push_back(m_baudRateBox);

   QString databits_label = QString("Data Bits:");
   m_dataBitsBox = new QComboBox(m_dialog);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::DataBitType::DATA_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::DataBitType> item = (Drivers::Serial::DataBitType)i;
      m_dataBitsBox->addItem(QString(item.toName().c_str()));
   }

   m_dataBitsBox->setCurrentText(QString(settings.serialSettings.dataBits.toName().c_str()));
   m_dataBitsBox->setDisabled(!m_editable);
   form->insertRow(4, databits_label, m_dataBitsBox);
   m_current_widgets.push_back(m_dataBitsBox);

   QString paritybits_label = QString("Parity Bits:");
   m_parityBitsBox = new QComboBox(m_dialog);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::ParityType::PARITY_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::ParityType> item = (Drivers::Serial::ParityType)i;
      m_parityBitsBox->addItem(QString(item.toName().c_str()));
   }
   m_parityBitsBox->setCurrentText(QString(settings.serialSettings.parityBits.toName().c_str()));
   m_parityBitsBox->setDisabled(!m_editable);
   form->insertRow(5, paritybits_label, m_parityBitsBox);
   m_current_widgets.push_back(m_parityBitsBox);

   QString stopbits_label = QString("Stop Bits:");
   m_stopBitsBox = new QComboBox(m_dialog);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::StopBitType::STOP_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::StopBitType> item = (Drivers::Serial::StopBitType)i;
      m_stopBitsBox->addItem(QString(item.toName().c_str()));
   }
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
   UT_Log(GUI_DIALOG, LOW, "rendering view for ETHERNET, settings %s", settings.shortSettingsString().c_str());
   clearDialog();

   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_dialog);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(SETTING_GET_U32(PortSettingDialog_maxLineEditLength));
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
   UT_Log(GUI_DIALOG, LOW, "color button clicked, current RGB #.6x", m_current_settings.trace_color);

   QColor color = QColorDialog::getColor(QColor(0xFF0000), m_dialog, "Select color");

   if (color.isValid())
   {
      QPalette palette = m_colorSelectionButton->palette();
      palette.setColor(QPalette::Button, color);
      m_colorSelectionButton->setPalette(palette);
      m_colorSelectionButton->update();
      UT_Log(GUI_DIALOG, LOW, "color dialog accepted, new RGB #.6x", color.rgb());
   }
}
void PortSettingDialog::clearDialog()
{
   UT_Log(GUI_DIALOG, MEDIUM, "removing %u widgets from GUI", m_current_widgets.size());
   for (auto item : m_current_widgets)
   {
      m_form->removeRow(item);
   }
   m_current_widgets.clear();
}
bool PortSettingDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.type.fromName(m_portTypeBox->currentText().toStdString());
   UT_Log(GUI_DIALOG, MEDIUM, "collecting settings for %s", out_settings.type.toName().c_str());

   if (out_settings.type == PortType::SERIAL)
   {
      out_settings.serialSettings.baudRate.fromName(m_baudRateBox->currentText().toStdString());
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
   UT_Log_If(!out_settings.areValid(), GUI_DIALOG, ERROR, "got invalid settings from GUI: %s", out_settings.shortSettingsString().c_str());

   return result;
}
void PortSettingDialog::onPortTypeChanged(const QString & name)
{
   UT_Log(MAIN_GUI, LOW, "new port type %s, rendering", name.toStdString().c_str());

   if (EnumValue<PortType>(name.toStdString()) == PortType::SERIAL)
   {
      renderSerialView(m_dialog, m_form, m_current_settings);
   }
   else
   {
      renderEthernetView(m_dialog, m_form, m_current_settings);
   }
}
