#include "Settings.h"
#include "Logger.h"
#include "PortSettingDialog.h"
#include "QtWidgets/QColorDialog"
#include <QtWidgets/qabstractitemview.h>


namespace Dialogs
{

PortSettingDialog::PortSettingDialog():
m_parent(nullptr),
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
m_traceColorSelectionButton(nullptr),
m_fontColorSelectionButton(nullptr),
m_info_label(nullptr),
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
   m_dialog->setPalette(parent->palette());
   m_parent = m_dialog;
   std::string window_title = "PORT" + std::to_string(current_settings.port_id) + " Settings";
   m_dialog->setWindowTitle(QString(window_title.c_str()));
   m_dialog->setWindowModality(Qt::ApplicationModal);
   m_dialog->setLayout(createLayout(m_dialog, current_settings, allow_edit));
   addDialogButtons();

   UT_Log(GUI_DIALOG, LOW, "%s settings [%s] edit possible: %u", __func__, current_settings.settingsString().c_str(), allow_edit);
   if (m_dialog->exec() == QDialog::Accepted)
   {
      result = convertGuiValues(out_settings);
      UT_Log(GUI_DIALOG, LOW, "%s new settings [%s]", __func__, out_settings.settingsString().c_str());
   }

   clearDialog();
   delete m_form;
   delete m_dialog;
   UT_Log(GUI_DIALOG, LOW, "%s result %s", __func__, result.has_value()? (result.value()? "OK" : "NOK") : "NO_VALUE");
   return result;
}
QLayout* PortSettingDialog::createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit)
{
   m_current_settings = current_settings;
   m_editable = allow_edit;
   m_parent = parent;
   m_form = new QFormLayout();

   addPortTypeComboBox(current_settings.type);

   if (current_settings.type == PortType::SERIAL)
   {
      renderSerialView(m_form, current_settings);
   }
   else if (current_settings.type == PortType::ETHERNET)
   {
      renderEthernetView(m_form, current_settings);
   }
   else
   {
      UT_Assert(false && "Invalid port type");
   }

   m_info_label = new QLabel();
   if (!allow_edit)
   {
      m_info_label->setText("Change locked!\nDisable this port to change settings");
   }
   m_form->addRow("", m_info_label);

   return m_form;
}
void PortSettingDialog::destroyLayout()
{
   if (m_form)
   {
      delete m_form;
      m_form = nullptr;
   }
}
void PortSettingDialog::addPortTypeComboBox(const Utilities::EnumValue<PortType>& current_selection)
{
   QString porttype_label = QString("Port type:");
   m_portTypeBox = new QComboBox(m_parent);

   for (const auto& name : Settings::port_names)
   {
      m_portTypeBox->addItem(QString(name.c_str()));
   }

   m_portTypeBox->setCurrentText(QString(current_selection.toName().c_str()));
   m_portTypeBox->setDisabled(!m_editable);
   m_portTypeBox->setPalette(m_parent->palette());
   m_portTypeBox->view()->setPalette(m_parent->palette());
   m_form->addRow(porttype_label, m_portTypeBox);

   QObject::connect(m_portTypeBox, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));

   UT_Log(GUI_DIALOG, HIGH, "adding %s combobox with %u items", porttype_label.toStdString().c_str(), Settings::port_names.size());
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
void PortSettingDialog::renderSerialView(QFormLayout* form, const Settings& settings)
{
   UT_Log(GUI_DIALOG, HIGH, "rendering view for SERIAL, settings %s", settings.settingsString().c_str());
   clearDialog();

   /* create port name */
   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_parent);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(SETTING_GET_U32(PortSettingDialog_maxLineEditLength));
   form->insertRow(1, portname_label, m_portNameEdit);
   m_current_widgets.push_back(m_portNameEdit);

   /* create device name */
   QString device_label = QString("Device name:");
   m_deviceNameEdit = new QLineEdit(m_parent);
   m_deviceNameEdit->setText(QString(settings.serialSettings.device.c_str()));
   m_deviceNameEdit->setDisabled(!m_editable);
   form->insertRow(2, device_label, m_deviceNameEdit);
   m_current_widgets.push_back(m_deviceNameEdit);

   /* create baudrate box */
   QString baudrate_label = QString("Baudrate:");
   m_baudRateBox = new QComboBox(m_parent);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::BaudRate::BAUDRATE_MAX; i++)
   {
      Utilities::EnumValue<Drivers::Serial::BaudRate> item = (Drivers::Serial::BaudRate)i;
      m_baudRateBox->addItem(QString(item.toName().c_str()));
   }
   m_baudRateBox->setCurrentText(QString(settings.serialSettings.baudRate.toName().c_str()));
   m_baudRateBox->setDisabled(!m_editable);
   m_baudRateBox->setPalette(m_parent->palette());
   m_baudRateBox->view()->setPalette(m_parent->palette());
   form->insertRow(3, baudrate_label, m_baudRateBox);
   m_current_widgets.push_back(m_baudRateBox);

   /* create databits box */
   QString databits_label = QString("Data Bits:");
   m_dataBitsBox = new QComboBox(m_parent);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::DataBitType::DATA_BIT_MAX; i++)
   {
      Utilities::EnumValue<Drivers::Serial::DataBitType> item = (Drivers::Serial::DataBitType)i;
      m_dataBitsBox->addItem(QString(item.toName().c_str()));
   }

   m_dataBitsBox->setCurrentText(QString(settings.serialSettings.dataBits.toName().c_str()));
   m_dataBitsBox->setDisabled(!m_editable);
   m_dataBitsBox->setPalette(m_parent->palette());
   m_dataBitsBox->view()->setPalette(m_parent->palette());
   form->insertRow(4, databits_label, m_dataBitsBox);
   m_current_widgets.push_back(m_dataBitsBox);

   /* create paritybit box */
   QString paritybits_label = QString("Parity Bits:");
   m_parityBitsBox = new QComboBox(m_parent);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::ParityType::PARITY_BIT_MAX; i++)
   {
      Utilities::EnumValue<Drivers::Serial::ParityType> item = (Drivers::Serial::ParityType)i;
      m_parityBitsBox->addItem(QString(item.toName().c_str()));
   }
   m_parityBitsBox->setCurrentText(QString(settings.serialSettings.parityBits.toName().c_str()));
   m_parityBitsBox->setDisabled(!m_editable);
   m_parityBitsBox->setPalette(m_parent->palette());
   m_parityBitsBox->view()->setPalette(m_parent->palette());
   form->insertRow(5, paritybits_label, m_parityBitsBox);
   m_current_widgets.push_back(m_parityBitsBox);

   /* create stopbit box */
   QString stopbits_label = QString("Stop Bits:");
   m_stopBitsBox = new QComboBox(m_parent);
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::StopBitType::STOP_BIT_MAX; i++)
   {
      Utilities::EnumValue<Drivers::Serial::StopBitType> item = (Drivers::Serial::StopBitType)i;
      m_stopBitsBox->addItem(QString(item.toName().c_str()));
   }
   m_stopBitsBox->setCurrentText(QString(settings.serialSettings.stopBits.toName().c_str()));
   m_stopBitsBox->setDisabled(!m_editable);
   m_stopBitsBox->setPalette(m_parent->palette());
   m_stopBitsBox->view()->setPalette(m_parent->palette());
   form->insertRow(6, stopbits_label, m_stopBitsBox);
   m_current_widgets.push_back(m_stopBitsBox);

   /* create trace color button */
   QString color_label = QString("Background color");
   m_traceColorSelectionButton = new QPushButton(m_parent);
   m_traceColorSelectionButton->setDisabled(!m_editable);
   QPalette palette = m_traceColorSelectionButton->palette();
   palette.setColor(QPalette::Button, QColor(settings.trace_color));
   m_traceColorSelectionButton->setPalette(palette);
   m_traceColorSelectionButton->update();
   form->insertRow(7, color_label, m_traceColorSelectionButton);
   m_current_widgets.push_back(m_traceColorSelectionButton);
   QObject::connect(m_traceColorSelectionButton, SIGNAL(clicked()), this, SLOT(onBackgroundColorButtonClicked()));

   /* create font color button */
   QString font_color_label = QString("Font color");
   m_fontColorSelectionButton = new QPushButton(m_parent);
   m_fontColorSelectionButton->setDisabled(!m_editable);
   QPalette font_palette = m_fontColorSelectionButton->palette();
   font_palette.setColor(QPalette::Button, QColor(settings.font_color));
   m_fontColorSelectionButton->setPalette(font_palette);
   m_fontColorSelectionButton->update();
   form->insertRow(8, font_color_label, m_fontColorSelectionButton);
   m_current_widgets.push_back(m_fontColorSelectionButton);
   QObject::connect(m_fontColorSelectionButton, SIGNAL(clicked()), this, SLOT(onFontColorButtonClicked()));
}
void PortSettingDialog::renderEthernetView(QFormLayout* form, const Settings& settings)
{
   UT_Log(GUI_DIALOG, HIGH, "rendering view for ETHERNET, settings %s", settings.settingsString().c_str());
   clearDialog();

   /* create port name */
   QString portname_label = QString("Port name:");
   m_portNameEdit = new QLineEdit(m_parent);
   m_portNameEdit->setText(QString(settings.port_name.c_str()));
   m_portNameEdit->setDisabled(!m_editable);
   m_portNameEdit->setMaxLength(SETTING_GET_U32(PortSettingDialog_maxLineEditLength));
   form->insertRow(1, portname_label, m_portNameEdit);
   m_current_widgets.push_back(m_portNameEdit);

   /* create ip address field */
   QString address_label = QString("IP Address:");
   m_ipAddressEdit = new QLineEdit(m_parent);
   m_ipAddressEdit->setText(QString(settings.ip_address.c_str()));
   m_ipAddressEdit->setDisabled(!m_editable);
   form->insertRow(2, address_label, m_ipAddressEdit);
   m_current_widgets.push_back(m_ipAddressEdit);

   /* create ip port field */
   QString port_label = QString("Port:");
   m_ipPortEdit = new QLineEdit(m_parent);
   m_ipPortEdit->setText(QString::number(settings.port));
   m_ipPortEdit->setDisabled(!m_editable);
   form->insertRow(3, port_label, m_ipPortEdit);
   m_current_widgets.push_back(m_ipPortEdit);

   /* create color button */
   QString color_label = QString("Background color");
   m_traceColorSelectionButton = new QPushButton(m_parent);
   m_traceColorSelectionButton->setDisabled(!m_editable);
   QPalette palette = m_traceColorSelectionButton->palette();
   palette.setColor(QPalette::Button, QColor(settings.trace_color));
   m_traceColorSelectionButton->setPalette(palette);
   m_traceColorSelectionButton->update();
   form->insertRow(4, color_label, m_traceColorSelectionButton);
   m_current_widgets.push_back(m_traceColorSelectionButton);
   QObject::connect(m_traceColorSelectionButton, SIGNAL(clicked()), this, SLOT(onBackgroundColorButtonClicked()));

   /* create font color button */
   QString font_color_label = QString("Font color");
   m_fontColorSelectionButton = new QPushButton(m_parent);
   m_fontColorSelectionButton->setDisabled(!m_editable);
   QPalette font_palette = m_fontColorSelectionButton->palette();
   font_palette.setColor(QPalette::Button, QColor(settings.font_color));
   m_fontColorSelectionButton->setPalette(font_palette);
   m_fontColorSelectionButton->update();
   form->insertRow(5, font_color_label, m_fontColorSelectionButton);
   m_current_widgets.push_back(m_fontColorSelectionButton);
   QObject::connect(m_fontColorSelectionButton, SIGNAL(clicked()), this, SLOT(onFontColorButtonClicked()));

}
void PortSettingDialog::onBackgroundColorButtonClicked()
{
   UT_Log(GUI_DIALOG, HIGH, "color button clicked, current RGB %.6x", m_current_settings.trace_color);

   QColor color = QColorDialog::getColor(m_current_settings.trace_color, m_parent, "Select background color");

   if (color.isValid())
   {
      QPalette palette = m_traceColorSelectionButton->palette();
      palette.setColor(QPalette::Button, color);
      m_traceColorSelectionButton->setPalette(palette);
      m_traceColorSelectionButton->update();
      m_current_settings.trace_color = color.rgb();
      UT_Log(GUI_DIALOG, LOW, "color dialog accepted, new RGB %.6x", color.rgb());
   }
}
void PortSettingDialog::onFontColorButtonClicked()
{
   UT_Log(GUI_DIALOG, HIGH, "font color button clicked, current RGB %.6x", m_current_settings.font_color);

   QColor color = QColorDialog::getColor(m_current_settings.font_color, m_parent, "Select font color");

   if (color.isValid())
   {
      QPalette palette = m_fontColorSelectionButton->palette();
      palette.setColor(QPalette::Button, color);
      m_fontColorSelectionButton->setPalette(palette);
      m_fontColorSelectionButton->update();
      m_current_settings.font_color = color.rgb();
      UT_Log(GUI_DIALOG, LOW, "font color dialog accepted, new RGB %.6x", color.rgb());
   }
}
void PortSettingDialog::clearDialog()
{
   UT_Log(GUI_DIALOG, HIGH, "removing %u widgets from GUI", m_current_widgets.size());
   for (auto item : m_current_widgets)
   {
      m_form->removeRow(item);
   }
   m_current_widgets.clear();
}
bool PortSettingDialog::convertGuiValues(Settings& out_settings)
{
   out_settings = m_current_settings;
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
   out_settings.trace_color = m_current_settings.trace_color;
   out_settings.font_color = m_current_settings.font_color;

   return true;
}
void PortSettingDialog::onPortTypeChanged(const QString & name)
{
   UT_Log(GUI_DIALOG, HIGH, "new port type %s, rendering", name.toStdString().c_str());

   if (Utilities::EnumValue<PortType>(name.toStdString()) == PortType::SERIAL)
   {
      renderSerialView(m_form, m_current_settings);
   }
   else
   {
      renderEthernetView(m_form, m_current_settings);
   }
}

}
