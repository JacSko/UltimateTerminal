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

struct PortSettingDialogFixture : public testing::Test
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

TEST_F(PortSettingDialogFixture, current_settings_presentation)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user, variant with ETHERNET connection <br>
    * <b>expected</b>: <br>
    *       Corrent elements added to dialog. <br>
    *       Current settings should be correctly presented to user.<br>
    * ************************************************
    */

   QDialog test_dialog;
   QFormLayout test_layout;
   QComboBox test_porttype;
   QDialogButtonBox test_buttonbox;

   QLineEdit test_portname;
   QLineEdit test_ipaddress;
   QLineEdit test_ipport;
   QPushButton test_color;
   QPalette test_palette;
   PortSettingDialog::Settings current_settings;
   PortSettingDialog::Settings new_settings;
   current_settings.type = PortSettingDialog::PortType::ETHERNET;
   current_settings.port_name = "TEST NAME";
   current_settings.ip_address = "111.222.333.444";
   current_settings.port = 9876;
   current_settings.trace_color = 0xAABBCC;

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
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_palette(&test_color)).WillOnce(Return(test_palette));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setPalette(&test_color, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_update(&test_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_insertRow(&test_layout,_,_, &test_color));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_color,_,_,_));

   /* button box creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(QDialog::Rejected));

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_porttype, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_buttonbox, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_portname, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipaddress, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_ipport, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_color, false));


   /* expect widgets remove when closing dialogs */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_portname));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipaddress));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_ipport));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_removeRow(&test_layout, &test_color));

   PortSettingDialog test_subject;
   test_subject.showDialog(nullptr, current_settings, new_settings, true);


}
