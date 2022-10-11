#ifndef PORT_SETTING_DIALOG_H_
#define PORT_SETTING_DIALOG_H_

#include <stdint.h>
#include <string>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include "Logger.h"

namespace GUI
{

class PortSettingDialog : public QObject
{
   Q_OBJECT

public:
   PortSettingDialog(){};
   ~PortSettingDialog(){};

   enum class PortType
   {
      SERIAL,
      ETHERNET,
   };

   enum class DataBits
   {
      BITS5,
      BITS6,
      BITS7,
      BITS8,
   };

   enum class ParityBits
   {
      NONE,
      EVEN,
      ODD,
      MARK,
      SPACE
   };

   enum class StopBits
   {
      ONE,
      TWO,
   };

   struct Settings
   {
      PortType type;
      std::string port_name;
      std::string device;
      uint32_t baud_rate;
      uint8_t data_bits;
      uint8_t parity_bits;
      uint8_t stop_bits;
      std::string ip_address;
      uint32_t port;
   };

   Settings showDialog(QWidget* parent, const Settings& current_settings)
   {

      QDialog dialog (parent);
      QFormLayout form(&dialog);

      QString porttype_label = QString("Port type:");
      QComboBox* porttype_box = new QComboBox(&dialog);
      porttype_box->addItems({"SERIAL", "ETHERNET"});
      form.addRow(porttype_label, porttype_box);

      QObject::connect(porttype_box, SIGNAL(currentTextChanged(const QString &)), this, SLOT(onPortTypeChanged(const QString &)));


      if (current_settings.type == PortType::SERIAL)
      {
         renderSerialView(&dialog, &form);
      }
      else
      {
         renderEthernetView(&dialog, &form);
      }

      QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                 Qt::Horizontal, &dialog);

      form.addWidget(&buttonBox);
      QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
      QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

      if (dialog.exec() == QDialog::Accepted)
      {
         UT_Log(MAIN, ERROR, "accepted!");
      }
   }

private:
   void renderSerialView(QDialog* dialog, QFormLayout* form)
   {
      QString portname_label = QString("Port name:");
      QLineEdit* portname_edit = new QLineEdit(dialog);
      form->addRow(portname_label, portname_edit);

      QString device_label = QString("Device name:");
      QLineEdit* device_edit = new QLineEdit(dialog);
      form->addRow(device_label, device_edit);

      QString baudrate_label = QString("Baudrate:");
      QLineEdit* baudrate_edit = new QLineEdit(dialog);
      form->addRow(baudrate_label, baudrate_edit);

      QString databits_label = QString("Data Bits:");
      QComboBox* databits_box = new QComboBox(dialog);
      databits_box->addItems({"5", "6", "7", "8"});
      form->addRow(databits_label, databits_box);

      QString paritybits_label = QString("Parity Bits:");
      QComboBox* paritybits_box = new QComboBox(dialog);
      paritybits_box->addItems({"NONE", "EVEN", "ODD", "MARK", "SPACE"});
      form->addRow(paritybits_label, paritybits_box);

      QString stopbits_label = QString("Stop Bits:");
      QComboBox* stopbits_box = new QComboBox(dialog);
      stopbits_box->addItems({"1", "2"});
      form->addRow(stopbits_label, stopbits_box);


   }

   void renderEthernetView(QDialog* dialog, QFormLayout* form)
   {
      QString portname_label = QString("Port name:");
      QLineEdit* portname_edit = new QLineEdit(dialog);
      form->addRow(portname_label, portname_edit);

      QString device_label = QString("Device name:");
      QLineEdit* device_edit = new QLineEdit(dialog);
      form->addRow(device_label, device_edit);

      QString address_label = QString("IP Address:");
      QLineEdit* address_edit = new QLineEdit(dialog);
      form->addRow(address_label, address_edit);

      QString port_label = QString("Port:");
      QLineEdit* port_edit = new QLineEdit(dialog);
      form->addRow(port_label, port_edit);

   }

public slots:
   void onPortTypeChanged(const QString& port_name)
   {
      UT_Log(MAIN_GUI, HIGH, "new port type %s", port_name.toStdString().c_str());
   }

};


}

#endif
