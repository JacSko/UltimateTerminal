#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "PortSettingDialog.h"
#include "Logger.h"
#include "SerialDriverMock.h"

/**
 * @file PortSettingDialogTests.cpp
 *
 * @brief
 *    Class to test behavior of PortSettingDialog class.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

using namespace ::testing;

struct TestParam
{
   bool editable;
   bool accepted;
   PortSettingDialog::Settings current_settings;
   PortSettingDialog::Settings new_settings;

   friend std::ostream& operator<<(std::ostream& ost, const TestParam& param)
   {
      ost << "Test with settings: " << std::endl;
      ost << "editable: " << param.editable << std::endl;
      ost << "accepted: " << param.accepted << std::endl;
      ost << "old: " << param.current_settings.shortSettingsString() << std::endl;
      ost << "new: " << param.new_settings.shortSettingsString() << std::endl;
      return ost;
   }

};

struct PortSettingDialogParam : public testing::TestWithParam<TestParam>
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
   }
   void TearDown()
   {
      QtCoreMock_deinit();
      QtWidgetsMock_deinit();
   }
};

TEST_P(PortSettingDialogParam, some_test)
{
   /* widgets presented on GUI */
   /* common widgets */
   QDialog test_dialog;
   QFormLayout test_layout;
   QComboBox test_porttype;
   QDialogButtonBox test_buttonbox;
   QLineEdit test_portname;
   QPushButton test_color_button;

   /* widgets related to ETHERNET port */
   QLineEdit test_ipaddress;
   QLineEdit test_ipport;

   /* widgets related to SERIAL port */
   QLineEdit test_serial_device_name;
   QComboBox test_serial_baudrate;
   QComboBox test_serial_databits;
   QComboBox test_serial_paritybits;
   QComboBox test_serial_stopbits;

   printf("WIDGET ADDRESSES:\n");
   printf("test_porttype : %p\n", (void*)&test_porttype);
   printf("test_buttonbox : %p\n", (void*)&test_buttonbox);
   printf("test_portname : %p\n", (void*)&test_portname);
   printf("test_color_button : %p\n", (void*)&test_color_button);
   printf("test_ipaddress : %p\n", (void*)&test_ipaddress);
   printf("test_ipport : %p\n", (void*)&test_ipport);
   printf("test_serial_device_name : %p\n", (void*)&test_serial_device_name);
   printf("test_serial_baudrate : %p\n", (void*)&test_serial_baudrate);
   printf("test_serial_databits : %p\n", (void*)&test_serial_databits);
   printf("test_serial_paritybits : %p\n", (void*)&test_serial_paritybits);
   printf("test_serial_stopbits : %p\n", (void*)&test_serial_stopbits);

   bool editable = GetParam().editable;
   bool dialog_accepted = GetParam().accepted;
   PortSettingDialog::Settings current_settings = GetParam().current_settings;
   PortSettingDialog::Settings user_settings = GetParam().new_settings;
   bool settings_valid = user_settings.areValid();

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   if (current_settings.type == PortSettingDialog::PortType::ETHERNET)
   {
      if (current_settings.type == user_settings.type)
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype));
         /* creation of elements specific for ETHERNET view */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_ipaddress))
                                                           .WillOnce(Return(&test_ipport));
         EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color_button));
         /* created field with port name */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, _));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname));

         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color_button,!editable));

         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color_button));
      }
      else
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype))
                                                           .WillOnce(Return(&test_serial_baudrate))
                                                           .WillOnce(Return(&test_serial_databits))
                                                           .WillOnce(Return(&test_serial_paritybits))
                                                           .WillOnce(Return(&test_serial_stopbits));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_ipaddress))
                                                           .WillOnce(Return(&test_ipport))
                                                           .WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_serial_device_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color_button))
                                                             .WillOnce(Return(&test_color_button));
         /* created field with port name */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, _)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname)).Times(2);

         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname,!editable)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color_button,!editable)).Times(2);

         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_device_name,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_baudrate,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_databits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_paritybits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_stopbits,!editable));

         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color_button)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_device_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_baudrate));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_databits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_paritybits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_stopbits));

      }

      /* created field with IP address */
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipaddress, current_settings.ip_address));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipaddress));
      /* created field with IP port */
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipport, QString::number(current_settings.port)));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipport));

   }
   else
   {
      if (current_settings.type == user_settings.type)
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype))
                                                           .WillOnce(Return(&test_serial_baudrate))
                                                           .WillOnce(Return(&test_serial_databits))
                                                           .WillOnce(Return(&test_serial_paritybits))
                                                           .WillOnce(Return(&test_serial_stopbits));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_serial_device_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color_button));

         /* created field with port name */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, _));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname));

         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color_button,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_device_name,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_baudrate,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_databits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_paritybits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_stopbits,!editable));

         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color_button));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_device_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_baudrate));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_databits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_paritybits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_stopbits));

      }
      else
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype))
                                                           .WillOnce(Return(&test_serial_baudrate))
                                                           .WillOnce(Return(&test_serial_databits))
                                                           .WillOnce(Return(&test_serial_paritybits))
                                                           .WillOnce(Return(&test_serial_stopbits));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_serial_device_name))
                                                           .WillOnce(Return(&test_portname))
                                                           .WillOnce(Return(&test_ipaddress))
                                                           .WillOnce(Return(&test_ipport));
         EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color_button))
                                                             .WillOnce(Return(&test_color_button));
         /* created field with port name */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, _)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname)).Times(2);

         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname,!editable)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color_button,!editable)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_device_name,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_baudrate,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_databits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_paritybits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_stopbits,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress,!editable));
         EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport,!editable));

         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color_button)).Times(2);
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_device_name));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_baudrate));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_databits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_paritybits));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_stopbits));
      }

      /* created field with serial device namet */
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_serial_device_name, QString(current_settings.serialSettings.device.c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_serial_device_name));

      /* created field with baudrate */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_baudrate, _)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_baudrate, QString(current_settings.serialSettings.baudRate.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_baudrate));

      /* created field with data bits */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_databits, _)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_databits, QString(current_settings.serialSettings.dataBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_databits));

      /* created field with parity bits */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_paritybits, _)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_paritybits, QString(current_settings.serialSettings.parityBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_paritybits));

      /* created field with stop bits */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_stopbits, _)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_stopbits, QString(current_settings.serialSettings.stopBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_stopbits));

   }

   /* All port types added to combobox */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("SERIAL")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("ETHERNET")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_porttype, QString(current_settings.type.toName().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_porttype));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_porttype,_,_,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_porttype,!editable));

   /* button box creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_buttonbox,!editable));

   /* window should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* expect context switch */
   if (current_settings.type != user_settings.type)
   {
      /* created field with color selection */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_color_button, QString("Click!"))).Times(2);
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_color_button, QPalette(QPalette::Button, current_settings.trace_color))).Times(2);
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_color_button, QPalette(QPalette::Button, user_settings.trace_color)));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout,_,_, &test_color_button)).Times(2);
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_color_button,_,_,_)).Times(2);

      /* expect removal currently added widgets */
      if (current_settings.type == PortSettingDialog::PortType::ETHERNET)
      {

         /* created field with serial device namet */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_serial_device_name, QString(current_settings.serialSettings.device.c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_serial_device_name));

         /* created field with baudrate */
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_baudrate, _)).Times(AtLeast(1));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_baudrate, QString(current_settings.serialSettings.baudRate.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_baudrate));

         /* created field with data bits */
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_databits, _)).Times(AtLeast(1));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_databits, QString(current_settings.serialSettings.dataBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_databits));

         /* created field with parity bits */
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_paritybits, _)).Times(AtLeast(1));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_paritybits, QString(current_settings.serialSettings.parityBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_paritybits));

         /* created field with stop bits */
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_stopbits, _)).Times(AtLeast(1));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_stopbits, QString(current_settings.serialSettings.stopBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_stopbits));

      }
      else
      {
         /* created field with IP address */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipaddress, current_settings.ip_address));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipaddress));
         /* created field with IP port */
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipport, QString::number(current_settings.port)));
         EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipport));

      }
   }
   else
   {
      /* created field with color selection */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_color_button, QString("Click!")));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_color_button, QPalette(QPalette::Button, current_settings.trace_color)));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_color_button, QPalette(QPalette::Button, user_settings.trace_color)));
      EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout,_,_, &test_color_button));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_color_button,_,_,_));

   }

   /* simulate that user changed the color */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(QColor(current_settings.trace_color),_,_)).WillOnce(Return(QColor(user_settings.trace_color)));

   /* expect data readout when dialog accepted */
   if (dialog_accepted)
   {
      if (user_settings.type == PortSettingDialog::PortType::ETHERNET)
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_porttype)).WillOnce(Return(QString("ETHERNET")));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_ipaddress)).WillOnce(Return(QString(user_settings.ip_address.c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_ipport)).WillOnce(Return(QString::number(user_settings.port)));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_portname)).WillOnce(Return(QString(user_settings.port_name.c_str())));
      }
      else
      {
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_porttype)).WillOnce(Return(QString("SERIAL")));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_baudrate)).WillOnce(Return(QString(user_settings.serialSettings.baudRate.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_databits)).WillOnce(Return(QString(user_settings.serialSettings.dataBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_paritybits)).WillOnce(Return(QString(user_settings.serialSettings.parityBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_stopbits)).WillOnce(Return(QString(user_settings.serialSettings.stopBits.toName().c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_serial_device_name)).WillOnce(Return(QString(user_settings.serialSettings.device.c_str())));
         EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_portname)).WillOnce(Return(QString(user_settings.port_name.c_str())));
      }
   }

   PortSettingDialog test_subject;
   PortSettingDialog::Settings new_settings;
   /* accept the dialog */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(
            Invoke([&]()-> QDialog::DialogCode
            {
               if (current_settings.type != user_settings.type)
               {
                  test_subject.onPortTypeChanged(QString(user_settings.type.toName().c_str()));
               }
               test_subject.onColorButtonClicked();
               return dialog_accepted? QDialog::Accepted : QDialog::Rejected;
            }));
   std::optional<bool> result = test_subject.showDialog(nullptr, current_settings, new_settings, editable);

   if (dialog_accepted)
   {
      EXPECT_EQ(result.has_value(), true);
      if (user_settings.areValid())
      {
         EXPECT_TRUE(result.value());
         EXPECT_EQ(new_settings.type, user_settings.type);
         EXPECT_EQ(new_settings.port_name, user_settings.port_name);
         EXPECT_EQ(new_settings.trace_color, user_settings.trace_color);
         if (user_settings.type == PortSettingDialog::PortType::ETHERNET)
         {
            EXPECT_EQ(new_settings.ip_address, user_settings.ip_address);
            EXPECT_EQ(new_settings.port, user_settings.port);
         }
         else
         {
            EXPECT_EQ(new_settings.serialSettings.baudRate.value, user_settings.serialSettings.baudRate.value);
            EXPECT_EQ(new_settings.serialSettings.dataBits.value, user_settings.serialSettings.dataBits.value);
            EXPECT_EQ(new_settings.serialSettings.device, user_settings.serialSettings.device);
            EXPECT_EQ(new_settings.serialSettings.mode, user_settings.serialSettings.mode);
            EXPECT_EQ(new_settings.serialSettings.parityBits.value, user_settings.serialSettings.parityBits.value);
            EXPECT_EQ(new_settings.serialSettings.stopBits.value, user_settings.serialSettings.stopBits.value);
         }
      }
      else
      {
         EXPECT_FALSE(result.value());
      }
   }
   else
   {
      EXPECT_EQ(result.has_value(), false);
   }
}

constexpr const char* TEST_PORT_NAME = "OLD_NAME";
constexpr const char* TEST_NEW_PORT_NAME = "NEW_NAME";
constexpr const char* TEST_DEVICE_PATH = "/dev/ttyUSB0";
constexpr const char* TEST_NEW_DEVICE_PATH = "/dev/ttyUSB3";
constexpr const char* TEST_CORRECT_IP_ADDRESS = "127.0.0.1";
constexpr const char* TEST_NEW_CORRECT_IP_ADDRESS = "127.0.0.4";
constexpr const char* TEST_INCORRECT_IP_ADDRESS = "222.333.444.555";
constexpr uint32_t TEST_CORRECT_IP_PORT = 1234;
constexpr uint32_t TEST_NEW_CORRECT_IP_PORT = 4321;
constexpr uint32_t TEST_INCORRECT_IP_PORT = 9999999;
constexpr uint32_t TEST_TRACE_COLOR = 0x1234;
constexpr uint32_t TEST_NEW_TRACE_COLOR = 0x4321;


TestParam params[] =
{
      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, user changed the context from ETHERNET to SERIAL, dialog accepted, setting are valid <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should be correctly returned. <br>
       *       std::optional should contain the value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::ETHERNET,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_NEW_CORRECT_IP_ADDRESS,
                                       TEST_NEW_CORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },

      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, user changed the context from SERIAL to ETHERNET, dialog accepted, setting are valid <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should be correctly returned. <br>
       *       std::optional should contain the value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::ETHERNET,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_NEW_CORRECT_IP_ADDRESS,
                                       TEST_NEW_CORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },
      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, only IP address and port has changed, user accepted the dialog <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should be correctly returned. <br>
       *       std::optional should contain the value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::ETHERNET,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::ETHERNET,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_NEW_CORRECT_IP_ADDRESS,
                                       TEST_NEW_CORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },
      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, multiple serial settings has changed, user accepted the dialog <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should be correctly returned. <br>
       *       std::optional should contain the value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },
      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, invalid IP address entered by user, user accepted the dialog <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should not be returned. <br>
       *       std::optional should contain the FALSE value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_INCORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },
      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created, invalid IP port entered by user, user accepted the dialog <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       New settings should be read from dialog.<br>
       *       New settings should not be returned. <br>
       *       std::optional should contain the FALSE value. <br>
       * ************************************************
       */
      {
            true, // editable
            true, // accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_NEW_PORT_NAME,
                                       {TEST_NEW_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_9600,
                                        Drivers::Serial::ParityType::EVEN,
                                        Drivers::Serial::StopBitType::TWO,
                                        Drivers::Serial::DataBitType::FIVE
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_INCORRECT_IP_PORT,
                                       TEST_NEW_TRACE_COLOR}, // new settings
      },

      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created for SERIAL context, but when no edit possible <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       std::optional should not contain the value. <br>
       * ************************************************
       */
      {
            false, // editable
            false, // not accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::SERIAL,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{}, // new settings
      },

      /**
       * @test
       * <b>scenario</b>: <br>
       *       Dialog created for ETHERNET context, but when no edit possible <br>
       * <b>expected</b>: <br>
       *       Current settings should be correctly presented to user.<br>
       *       std::optional should not contain the value. <br>
       * ************************************************
       */
      {
            false, // editable
            false, // not accepted
            PortSettingDialog::Settings{PortSettingDialog::PortType::ETHERNET,
                                        TEST_PORT_NAME,
                                       {TEST_DEVICE_PATH,
                                        Drivers::Serial::DataMode::NEW_LINE_DELIMITER,
                                        Drivers::Serial::BaudRate::BR_115200,
                                        Drivers::Serial::ParityType::NONE,
                                        Drivers::Serial::StopBitType::ONE,
                                        Drivers::Serial::DataBitType::EIGHT
                                       },
                                       TEST_CORRECT_IP_ADDRESS,
                                       TEST_CORRECT_IP_PORT,
                                       TEST_TRACE_COLOR}, // old settings
            PortSettingDialog::Settings{}, // new settings
      }
};
INSTANTIATE_TEST_CASE_P(PortSettingDialogParam, PortSettingDialogParam, ValuesIn(params));
