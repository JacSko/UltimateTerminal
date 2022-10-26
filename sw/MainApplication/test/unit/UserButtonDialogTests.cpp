#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"

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
      QtWidgetsMock_init();
   }
   void TearDown()
   {
      QtWidgetsMock_deinit();
   }
};

TEST_F(UserButtonDialogTests, dialog_presented_item_changed)
{
   /**
    * @test
    * <b>scenario</b>: Dialog created and show to the user <br>
    * <b>expected</b>: -> Current settings should be correctly presented to user.
    *                  -> User changed one of the fields, accepted the dialog.
    *                  -> New settings should be returned. <br>
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
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_buttonbox));

   /* editable widgets should be enabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(true)).Times(3);

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(current_settings.raw_commands));

   /* user entered the new button name */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text()).WillOnce(Return(new_button_name.c_str()));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_toPlainText()).WillOnce(Return(current_settings.raw_commands.c_str()));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(Qt::ApplicationModal));

   /* dialog accepted by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec()).WillOnce(Return(QDialog::Accepted));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, true);

   EXPECT_TRUE(result.has_value());
   EXPECT_TRUE(result.value());
   EXPECT_EQ(received_settings.button_name, new_button_name);
}

TEST_F(UserButtonDialogTests, dialog_presented_but_rejected_by_user)
{
   /**
    * <b>scenario</b>: Dialog created and show to the user <br>
    * <b>expected</b>: -> Current settings should be correctly presented to user.
    *                  -> User changed one of the fields, but declined the dialog.
    *                  -> Empty settings returned.
    *                  -> std::optional does not contain the value <br>
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
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_buttonbox));

   /* editable widgets should be disabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(true)).Times(3);

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(current_settings.raw_commands));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(Qt::ApplicationModal));

   /* dialog rejected by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec()).WillOnce(Return(QDialog::Rejected));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, true);

   EXPECT_FALSE(result.has_value());
   EXPECT_EQ(received_settings.button_name, "");
   EXPECT_EQ(received_settings.raw_commands, "");
}

TEST_F(UserButtonDialogTests, dialog_presented_but_not_editable)
{
   /**
    * <b>scenario</b>: Dialog created and show to the user <br>
    * <b>expected</b>: -> Current settings should be correctly presented to user.
    *                  -> All elements shall be not editable.
    *                  -> Empty settings returned.
    *                  -> std::optional does not contain the value <br>
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
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_textedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_buttonbox));

   /* editable widgets should be disabled */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(false)).Times(3);

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(current_settings.raw_commands));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(Qt::ApplicationModal));

   /* dialog rejected by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec()).WillOnce(Return(QDialog::Rejected));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, false);

   EXPECT_FALSE(result.has_value());
   EXPECT_EQ(received_settings.button_name, "");
   EXPECT_EQ(received_settings.raw_commands, "");
}

