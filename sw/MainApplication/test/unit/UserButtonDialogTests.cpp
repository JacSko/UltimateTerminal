#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "UserButtonDialog.h"
#include "Logger.h"

/**
 * @file UserButtonDialogTests.cpp
 *
 * @brief
 *    Dialog class to allow user to change button data.
 *
 * @details
 *    Dialog contains two fields - Button Name and Command Edit. First allows to set the name of the button, second allows to enter custom commands.
 *    To show dialog to user, call showDialog() method (this is blocking until user reaction).
 *    It is possible to control fields editability by bool allow_edit.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

using namespace ::testing;

struct UserButtonDialogTests : public testing::Test
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

TEST_F(UserButtonDialogTests, dialog_presented_item_changed)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       User changed one of the fields, accepted the dialog.<br>
    *       New settings should be returned. <br>
    * ************************************************
    */
   QDialog test_dialog;
   QFormLayout test_layout;
   QLineEdit test_linedit;
   QTextEdit test_textedit;
   QDialogButtonBox test_buttonbox;

   UserButtonDialog dialog;
   UserButtonDialog::Settings current_settings;
   current_settings.button_name = "TEST_NAME";
   std::string new_button_name = "NEW_NAME";
   current_settings.raw_commands = "TEST_COMMAND1\n"
         "                          TEST_COMMAND2";

   UserButtonDialog::Settings new_settings;
   UserButtonDialog::Settings received_settings;

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_new()).WillOnce(Return(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));
   /* editable widgets should be enabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_linedit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_textedit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_buttonbox, true));
   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_linedit, current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(&test_textedit, current_settings.raw_commands));

   /* user entered the new button name */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_linedit)).WillOnce(Return(new_button_name.c_str()));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_toPlainText(&test_textedit)).WillOnce(Return(current_settings.raw_commands.c_str()));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* dialog accepted by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(QDialog::Accepted));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, true);

   EXPECT_TRUE(result.has_value());
   EXPECT_TRUE(result.value());
   EXPECT_EQ(received_settings.button_name, new_button_name);
}

TEST_F(UserButtonDialogTests, dialog_presented_but_rejected_by_user)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       User changed one of the fields, but declined the dialog.<br>
    *       Empty settings returned.<br>
    *       std::optional does not contain the value <br>
    * ************************************************
    */
   QDialog test_dialog;
   QFormLayout test_layout;
   QLineEdit test_linedit;
   QTextEdit test_textedit;
   QDialogButtonBox test_buttonbox;

   UserButtonDialog dialog;
   UserButtonDialog::Settings current_settings;
   current_settings.button_name = "TEST_NAME";
   std::string new_button_name = "NEW_NAME";
   current_settings.raw_commands = "TEST_COMMAND1\n"
         "                          TEST_COMMAND2";

   UserButtonDialog::Settings new_settings;
   UserButtonDialog::Settings received_settings;

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_new()).WillOnce(Return(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* editable widgets should be disabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_linedit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_textedit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_buttonbox, true));

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_linedit, current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(&test_textedit, current_settings.raw_commands));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* dialog rejected by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(QDialog::Rejected));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, true);

   EXPECT_FALSE(result.has_value());
   EXPECT_EQ(received_settings.button_name, "");
   EXPECT_EQ(received_settings.raw_commands, "");
}

TEST_F(UserButtonDialogTests, dialog_presented_but_not_editable)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       All elements shall be not editable.<br>
    *       Empty settings returned.<br>
    *       std::optional does not contain the value <br>
    * ************************************************
    */
   QDialog test_dialog;
   QFormLayout test_layout;
   QLineEdit test_linedit;
   QTextEdit test_textedit;
   QDialogButtonBox test_buttonbox;

   UserButtonDialog dialog;
   UserButtonDialog::Settings current_settings;
   current_settings.button_name = "TEST_NAME";
   std::string new_button_name = "NEW_NAME";
   current_settings.raw_commands = "TEST_COMMAND1\n"
         "                          TEST_COMMAND2";

   UserButtonDialog::Settings new_settings;
   UserButtonDialog::Settings received_settings;

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_new()).WillOnce(Return(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, &test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* editable widgets should be disabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_linedit, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_textedit, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_buttonbox, false));

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_linedit, current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(&test_textedit, current_settings.raw_commands));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* dialog rejected by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(QDialog::Rejected));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, false);

   EXPECT_FALSE(result.has_value());
   EXPECT_EQ(received_settings.button_name, "");
   EXPECT_EQ(received_settings.raw_commands, "");
}

