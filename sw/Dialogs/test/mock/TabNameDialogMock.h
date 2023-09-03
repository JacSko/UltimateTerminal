#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TabNameDialog.h"

struct TabNameDialogMock
{
   MOCK_METHOD2(showDialog,std::optional<std::string>(QWidget*, const std::string&));
};

TabNameDialogMock* g_tab_name_dialog_mock;

void TabNameDialogMock_init()
{
   if (!g_tab_name_dialog_mock)
   {
      g_tab_name_dialog_mock = new TabNameDialogMock();
   }
}
void TabNameDialogMock_deinit()
{
   if (g_tab_name_dialog_mock)
   {
      delete g_tab_name_dialog_mock;
      g_tab_name_dialog_mock = nullptr;
   }
}
TabNameDialogMock* TabNameDialogMock_get()
{
   UT_Assert(g_tab_name_dialog_mock && "Create TabNameDialog mock first!");
   return g_tab_name_dialog_mock;

}

namespace Dialogs
{

TabNameDialog::TabNameDialog() {}
TabNameDialog::~TabNameDialog() {}

std::optional<std::string> TabNameDialog::showDialog(QWidget* parent, const std::string& current_name)
{
   return g_tab_name_dialog_mock->showDialog(parent, current_name);
}

}
