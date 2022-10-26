#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"

#include "UserButtonDialog.h"
#include "Logger.h"

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

TEST_F(UserButtonDialogTests, settings_presented_on_gui)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
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

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(current_settings.button_name));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setText(current_settings.raw_commands));

   /* user entered the new button name */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text()).WillOnce(Return(QString(new_button_name.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_toPlainText()).WillOnce(Return(QString(current_settings.raw_commands.c_str())));

   /* button accepted by user */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec()).WillOnce(Return(QDialog::Accepted));

   std::optional<bool> result = dialog.showDialog(nullptr, current_settings, received_settings, true);

   EXPECT_TRUE(result.has_value());
   EXPECT_TRUE(result.value());
   EXPECT_EQ(received_settings.button_name, new_button_name);
}
