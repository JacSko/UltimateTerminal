#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "TraceFilterSettingDialog.h"
#include "Logger.h"

/**
 * @file TraceFilterSettingDialogTests.h
 *
 * @brief
 *    Tests for dialog class which allows user to configure trace filer entity.
 *
 * @details
 *    Dialog contains two fields - background and font color. These settings allows to configure how trace item will be visible in trace terminal view.
 *    To show dialog to user, call showDialog() method (this is blocking until user reaction).
 *    There is no possibility to block editability.
 *
 * @author Jacek Skowronek
 * @date   15/11/2022
 *
 */

using namespace ::testing;
namespace Dialogs
{

struct TraceFilterSettingDialogFixture : public testing::Test
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

TEST_F(TraceFilterSettingDialogFixture, dialog_presented_user_accepted)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user, user accepted the dialog. <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       User changed both colors.<br>
    *       New settings should be returned. <br>
    * ************************************************
    */
   QMainWindow test_parent;
   QDialog test_dialog;
   QFormLayout test_layout;
   QPushButton test_backgroundButton;
   QPushButton test_fontButton;
   QLineEdit test_regexEdit;
   QDialogButtonBox test_buttonbox;
   TraceFilterSettingDialog dialog;

   /* current settings */
   TraceFilterSettingDialog::Settings current_settings;
   current_settings.background = 0x000001;
   current_settings.font = 0x000002;
   current_settings.regex = "test_regex";

   TraceFilterSettingDialog::Settings received_settings;

   /* new settings retreived from user */
   TraceFilterSettingDialog::Settings user_settings;
   user_settings.background = 0x000003;
   user_settings.font = 0x000004;
   user_settings.regex = "user_regex";

   TraceFilterSettingDialog::Settings set_presented_to_user;

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_regexEdit));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_backgroundButton))
                                                       .WillOnce(Return(&test_fontButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_backgroundButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_fontButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_regexEdit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_backgroundButton,"clicked()",&dialog,"onBackgroundColorButtonClicked()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_fontButton,"clicked()",&dialog,"onFontColorButtonClicked()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* expect current regex presented on regexEdit */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_regexEdit, HasSubstr("test_regex")));
   /* expect readout of new settings */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_regexEdit)).WillOnce(Return(QString(user_settings.regex.c_str())));

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Invoke([&]()->int
         {
            set_presented_to_user.background = test_backgroundButton.palette().color(QPalette::Button).rgb();
            set_presented_to_user.font = test_fontButton.palette().color(QPalette::Button).rgb();
            dialog.onBackgroundColorButtonClicked();
            dialog.onFontColorButtonClicked();
            return QDialog::Accepted;
         }));
   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(QColor(user_settings.background)))
                                                                  .WillOnce(Return(QColor(user_settings.font)));

   std::optional<bool> result = dialog.showDialog(&test_parent, current_settings, received_settings);

   EXPECT_TRUE(result.has_value());
   EXPECT_TRUE(result.value());

   EXPECT_EQ(set_presented_to_user.background, current_settings.background);
   EXPECT_EQ(set_presented_to_user.font, current_settings.font);
   EXPECT_EQ(received_settings.background, user_settings.background);
   EXPECT_EQ(received_settings.font, user_settings.font);
   EXPECT_EQ(received_settings.regex, user_settings.regex);
}

TEST_F(TraceFilterSettingDialogFixture, dialog_presented_user_rejected)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user, user rejected the dialog. <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       std::optional shall not contain the value. <br>
    * ************************************************
    */
   QMainWindow test_parent;
   QDialog test_dialog;
   QFormLayout test_layout;
   QPushButton test_backgroundButton;
   QPushButton test_fontButton;
   QLineEdit test_regexEdit;
   QDialogButtonBox test_buttonbox;
   TraceFilterSettingDialog dialog;

   /* current settings */
   TraceFilterSettingDialog::Settings current_settings;
   current_settings.background = 0x000001;
   current_settings.font = 0x000002;
   current_settings.regex = "test_regex";

   TraceFilterSettingDialog::Settings received_settings;

   /* new settings retreived from user */
   TraceFilterSettingDialog::Settings user_settings;
   user_settings.background = 0x000003;
   user_settings.font = 0x000004;

   TraceFilterSettingDialog::Settings set_presented_to_user;

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_regexEdit));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_backgroundButton))
                                                       .WillOnce(Return(&test_fontButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_backgroundButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_fontButton));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, _, &test_regexEdit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_backgroundButton,"clicked()",&dialog,"onBackgroundColorButtonClicked()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_fontButton,"clicked()",&dialog,"onFontColorButtonClicked()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* expect current regex presented on regexEdit */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_regexEdit, HasSubstr("test_regex")));

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Invoke([&]()->int
         {
            set_presented_to_user.background = test_backgroundButton.palette().color(QPalette::Button).rgb();
            set_presented_to_user.font = test_fontButton.palette().color(QPalette::Button).rgb();
            dialog.onBackgroundColorButtonClicked();
            dialog.onFontColorButtonClicked();
            return QDialog::Rejected;
         }));
   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(QColor(user_settings.background)))
                                                                  .WillOnce(Return(QColor(user_settings.font)));

   std::optional<bool> result = dialog.showDialog(&test_parent, current_settings, received_settings);

   EXPECT_FALSE(result.has_value());
   EXPECT_EQ(set_presented_to_user.background, current_settings.background);
   EXPECT_EQ(set_presented_to_user.font, current_settings.font);
}


}
