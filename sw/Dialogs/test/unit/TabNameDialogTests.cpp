#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "TabNameDialog.h"

using namespace ::testing;
namespace Dialogs
{

struct TestParam
{
   bool accepted;
   friend std::ostream& operator<<(std::ostream& ost, const TestParam& param)
   {
      ost << "Test with settings -> accepted " << param.accepted << std::endl;
      return ost;
   }
};

struct TabNameDialogParam : public testing::TestWithParam<TestParam>
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

TEST_P(TabNameDialogParam, dialog_presented_item_changed)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       If dialog was accepted, the new name shall be notified to GUIController.<br>
    *       If dialog was rejected, notification with name shall not be sent to GUIController. <br>
    * ************************************************
    */
   QMainWindow test_parent;
   QDialog test_dialog;
   QFormLayout test_layout;
   QLineEdit test_linedit;
   QDialogButtonBox test_buttonbox;
   TabNameDialog dialog;

   std::string current_name = "TAB1";
   std::string new_name = "SOME TAB";

   /* prepare GUI elements */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));

   /* all GUI widgets added to layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_layout, QString("Tab name:"), &test_linedit));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addWidget(&test_layout, &test_buttonbox));

   /* all signals connected */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* expect correct values on GUI */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_linedit, current_name));

   if (GetParam().accepted)
   {
      /* user entered the new button name */
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_linedit)).WillOnce(Return(QString(new_name.c_str())));
   }
   /* dialog should be modal */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_setWindowModality(&test_dialog, Qt::ApplicationModal));

   /* user takes decision */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(GetParam().accepted? QDialog::Accepted : QDialog::Rejected));

   std::optional<std::string> result = dialog.showDialog(&test_parent, current_name);

   if (GetParam().accepted)
   {
      EXPECT_TRUE(result.has_value());
      EXPECT_EQ(result.value(), new_name);
   }
   else
   {
      EXPECT_FALSE(result.has_value());
   }
}

TestParam params[] = {{true},{false}};
INSTANTIATE_TEST_CASE_P(TabNameDialogParam, TabNameDialogParam, ValuesIn(params));

}
