#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "PortSettingDialog.h"
#include "Logger.h"

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
   bool accepted;
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

TEST_P(PortSettingDialogParam, dialog_show_new_correct_settings)
{
   const uint32_t CURRENT_TRACE_COLOR = 0x112233;
   const uint32_t NEW_TRACE_COLOR = 0x332211;

   QDialog test_dialog;
   QFormLayout test_layout;
   QComboBox test_porttype;
   QDialogButtonBox test_buttonbox;

   QLineEdit test_portname;
   QLineEdit test_ipaddress;
   QLineEdit test_ipport;
   QPushButton test_color;
   QPalette test_palette;

   /* current settings to be presented on dialog */
   PortSettingDialog::Settings current_settings;
   current_settings.type = PortSettingDialog::PortType::ETHERNET;
   current_settings.port_name = "TEST NAME";
   current_settings.ip_address = "111.222.333.444";
   current_settings.port = 9876;
   current_settings.trace_color = CURRENT_TRACE_COLOR;

   /* new settings retreived from dialog */
   PortSettingDialog::Settings new_settings;

   /* settings entered by user */
   PortSettingDialog::Settings user_settings;
   QPalette test_new_palette;
   user_settings.type = PortSettingDialog::PortType::ETHERNET;
   user_settings.port_name = "NEW NAME";
   user_settings.ip_address = "127.0.0.100";
   user_settings.port = 6789;
   test_new_palette.setColor(QPalette::ColorRole::Button, QColor(NEW_TRACE_COLOR));
   ASSERT_TRUE(user_settings.areValid());

   /* creation of elements common across port types */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* creation of elements specific for ETHERNET view */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                     .WillOnce(Return(&test_ipaddress))
                                                     .WillOnce(Return(&test_ipport));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color));

   /* all possible settings should be added to porttype combobox */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("SERIAL")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("ETHERNET")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_porttype, QString("ETHERNET")));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_porttype));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_porttype,_,_,_));

   /* created field with port name */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, 20));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname));
   /* created field with IP address */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipaddress, current_settings.ip_address));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipaddress));
   /* created field with IP port */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipport, QString::number(current_settings.port)));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipport));
   /* created field with color selection */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_color, QString("Click!")));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setPalette(&test_color, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_update(&test_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout,_,_, &test_color));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_color,_,_,_));

   /* button box creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* window should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* all settings should be editable */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_porttype, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_buttonbox, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color, false));

   if (GetParam().accepted)
   {
      /* palette requested 2 times - rendering current settings and readout on accepting */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_palette(&test_color)).WillOnce(Return(test_palette))
                                                                         .WillOnce(Return(test_new_palette));
      /* expect readout of new settings */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_porttype)).WillOnce(Return(QString("ETHERNET")));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_ipaddress)).WillOnce(Return(QString(user_settings.ip_address.c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_ipport)).WillOnce(Return(QString::number(user_settings.port)));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_portname)).WillOnce(Return(QString(user_settings.port_name.c_str())));

   }
   else
   {
      /* palette requested 2 times - rendering current settings */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_palette(&test_color)).WillOnce(Return(test_palette));

   }

   /* expect widgets remove when closing dialogs */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color));

   PortSettingDialog test_subject;

   /* dialog result based on parameter of the test */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(
                                 Return(GetParam().accepted? QDialog::Accepted : QDialog::Rejected));
   std::optional<bool> result = test_subject.showDialog(nullptr, current_settings, new_settings, true);

   bool expected_result = GetParam().accepted? true : false;
   EXPECT_EQ(result.has_value(), expected_result);
   if (GetParam().accepted)
   {
      EXPECT_TRUE(result.value());
      EXPECT_EQ(new_settings.type, user_settings.type);
      EXPECT_EQ(new_settings.port_name, user_settings.port_name);
      EXPECT_EQ(new_settings.ip_address, user_settings.ip_address);
      EXPECT_EQ(new_settings.port, user_settings.port);
      EXPECT_EQ(new_settings.trace_color, NEW_TRACE_COLOR);
   }
}

TEST_P(PortSettingDialogParam, dialog_show_port_type_change_new_settings)
{
   const uint32_t CURRENT_TRACE_COLOR = 0x112233;
   const uint32_t NEW_TRACE_COLOR = 0x332211;

   /* common widgets */
   QDialog test_dialog;
   QFormLayout test_layout;
   QComboBox test_porttype;
   QDialogButtonBox test_buttonbox;
   QLineEdit test_portname;
   QPushButton test_color;
   QPalette test_palette;

   /* widgets related to ETHERNET port */
   QLineEdit test_ipaddress;
   QLineEdit test_ipport;

   /* widgets related to SERIAL port */
   QLineEdit test_serial_device_name;
   QComboBox test_serial_baudrate;
   QComboBox test_serial_databits;
   QComboBox test_serial_paritybits;
   QComboBox test_serial_stopbits;

   /* current settings to be presented on GUI */
   PortSettingDialog::Settings current_settings;
   current_settings.type = PortSettingDialog::PortType::ETHERNET;
   current_settings.port_name = "TEST NAME";
   current_settings.ip_address = "111.222.333.444";
   current_settings.port = 9876;
   current_settings.trace_color = 0xAABBCC;
   current_settings.serialSettings.device = "/dev/ttyUSB0";
   current_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_115200;
   current_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::ONE;
   current_settings.serialSettings.parityBits = Drivers::Serial::ParityType::NONE;
   current_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::EIGHT;

   /* new settings retreived from dialog when accepted */
   PortSettingDialog::Settings new_settings;

   /* new settings retreived from user */
   PortSettingDialog::Settings user_settings;
   QPalette test_new_palette;
   user_settings.type = PortSettingDialog::PortType::SERIAL;
   user_settings.port_name = "NEW_NAME";
   user_settings.trace_color = 0xDEAD;
   user_settings.serialSettings.device = "/dev/ttyUSB3";
   user_settings.serialSettings.baudRate = Drivers::Serial::BaudRate::BR_9600;
   user_settings.serialSettings.stopBits = Drivers::Serial::StopBitType::TWO;
   user_settings.serialSettings.parityBits = Drivers::Serial::ParityType::EVEN;
   user_settings.serialSettings.dataBits = Drivers::Serial::DataBitType::FIVE;
   test_new_palette.setColor(QPalette::ColorRole::Button, QColor(0x112233));
   ASSERT_TRUE(user_settings.areValid());

   /* creation of elements common across port types */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_porttype))
                                                    /* serial comboboxes creation */
                                                      .WillOnce(Return(&test_serial_baudrate))
                                                      .WillOnce(Return(&test_serial_databits))
                                                      .WillOnce(Return(&test_serial_paritybits))
                                                      .WillOnce(Return(&test_serial_stopbits));

   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* creation of elements specific for ETHERNET view */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_portname))
                                                     .WillOnce(Return(&test_ipaddress))
                                                     .WillOnce(Return(&test_ipport))
                                                     /* serial widgets */
                                                     .WillOnce(Return(&test_portname))
                                                     .WillOnce(Return(&test_serial_device_name));


   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_color))
                                                       .WillOnce(Return(&test_color));

   /* all possible settings should be added to porttype combobox */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("SERIAL")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_porttype, QString("ETHERNET")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_porttype, QString("ETHERNET")));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_porttype, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_porttype));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_porttype,_,_,_));

   /* created field with port name */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_portname, current_settings.port_name)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname, false)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setMaxLength(&test_portname, 20)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_portname)).Times(2);
   /* created field with IP address */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipaddress, current_settings.ip_address));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipaddress));
   /* created field with IP port */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_ipport, QString::number(current_settings.port)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_ipport));

   /* created field with serial device namet */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_serial_device_name, QString(current_settings.serialSettings.device.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_device_name, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _,_, &test_serial_device_name));

   /* created field with baudrate */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_baudrate, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_baudrate, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_baudrate, QString(current_settings.serialSettings.baudRate.toName().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_baudrate));

   /* created field with data bits */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_databits, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_databits, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_databits, QString(current_settings.serialSettings.dataBits.toName().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_databits));

   /* created field with parity bits */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_paritybits, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_paritybits, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_paritybits, QString(current_settings.serialSettings.parityBits.toName().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_paritybits));

   /* created field with stop bits */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_serial_stopbits, _)).Times(AtLeast(1));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_serial_stopbits, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_serial_stopbits, QString(current_settings.serialSettings.stopBits.toName().c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout, _, _, &test_serial_stopbits));

   /* created field with color selection */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_color, QString("Click!"))).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color, false)).Times(2);

   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setPalette(&test_color, _)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_update(&test_color)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout,_,_, &test_color)).Times(2);
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_color,_,_,_)).Times(2);

   /* button box creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_buttonbox, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   if (GetParam().accepted)
   {
      /* palette requested 3 times - renering current settings, port type change and readout on accepting */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_palette(&test_color)).WillOnce(Return(test_palette))
                                                                         .WillOnce(Return(test_palette))
                                                                         .WillOnce(Return(test_new_palette));
      /* expect readout of new serial settings */
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_porttype)).WillOnce(Return(QString("SERIAL")));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_baudrate)).WillOnce(Return(QString(user_settings.serialSettings.baudRate.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_databits)).WillOnce(Return(QString(user_settings.serialSettings.dataBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_paritybits)).WillOnce(Return(QString(user_settings.serialSettings.parityBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_serial_stopbits)).WillOnce(Return(QString(user_settings.serialSettings.stopBits.toName().c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_serial_device_name)).WillOnce(Return(QString(user_settings.serialSettings.device.c_str())));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_portname)).WillOnce(Return(QString(user_settings.port_name.c_str())));
   }
   else
   {
      /* palette requested 2 times - renering current settings, port type change */
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_palette(&test_color)).WillOnce(Return(test_palette))
                                                                         .WillOnce(Return(test_palette));

   }

   /* expect ethernet widgets removal when swotching context */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname)).Times(2);
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color)).Times(2);

   /* expect serial widgets removal when closing dialog */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_baudrate));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_databits));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_paritybits));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_stopbits));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_serial_device_name));

   PortSettingDialog test_subject;
   /* accept the dialog */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(
            Invoke([&]()-> QDialog::DialogCode
            {
               test_subject.onPortTypeChanged("SERIAL");
               return GetParam().accepted? QDialog::Accepted : QDialog::Rejected;
            }));
   std::optional<bool> result = test_subject.showDialog(nullptr, current_settings, new_settings, true);

   if (GetParam().accepted)
   {
      EXPECT_EQ(result.has_value(), true);
      EXPECT_TRUE(result.value());
      EXPECT_EQ(new_settings.type, PortSettingDialog::PortType::SERIAL);
      EXPECT_EQ(new_settings.serialSettings.baudRate.value, user_settings.serialSettings.baudRate.value);
      EXPECT_EQ(new_settings.serialSettings.dataBits.value, user_settings.serialSettings.dataBits.value);
      EXPECT_EQ(new_settings.serialSettings.device, user_settings.serialSettings.device);
      EXPECT_EQ(new_settings.serialSettings.parityBits.value, user_settings.serialSettings.parityBits.value);
      EXPECT_EQ(new_settings.serialSettings.stopBits.value, user_settings.serialSettings.stopBits.value);
   }
   else
   {
      EXPECT_EQ(result.has_value(), false);
   }
}


TestParam params[] = {{true},{false}};
INSTANTIATE_TEST_CASE_P(PortSettingDialogParam, PortSettingDialogParam, ValuesIn(params));
