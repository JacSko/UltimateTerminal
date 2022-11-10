#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "UserButtonDialog.h"

struct UserButtonDialogMock
{
   MOCK_METHOD4(showDialog,std::optional<bool>(QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings&, bool));
};

UserButtonDialogMock* g_user_button_dialog_mock;

void UserButtonDialogMock_init()
{
   if (!g_user_button_dialog_mock)
   {
      g_user_button_dialog_mock = new UserButtonDialogMock();
   }
}
void UserButtonDialogMock_deinit()
{
   if (g_user_button_dialog_mock)
   {
      delete g_user_button_dialog_mock;
      g_user_button_dialog_mock = nullptr;
   }
}
UserButtonDialogMock* UserButtonDialogMock_get()
{
   UT_Assert(g_user_button_dialog_mock && "Create UserButtonDialog mock first!");
   return g_user_button_dialog_mock;

}

namespace Dialogs
{

UserButtonDialog::UserButtonDialog() {}
UserButtonDialog::~UserButtonDialog() {}

std::optional<bool> UserButtonDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return g_user_button_dialog_mock->showDialog(parent, current_settings, out_settings, allow_edit);
}

}
