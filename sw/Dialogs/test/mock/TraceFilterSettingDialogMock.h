#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilterSettingDialog.h"


struct TraceFilterSettingDialogMock
{
   MOCK_METHOD4(showDialog,std::optional<bool>(QWidget*, const Dialogs::TraceFilterSettingDialog::Settings&, Dialogs::TraceFilterSettingDialog::Settings&, bool));
};

TraceFilterSettingDialogMock* g_trace_filter_setting_dialog_mock;

void TraceFilterSettingDialogMock_init()
{
   if (!g_trace_filter_setting_dialog_mock)
   {
      g_trace_filter_setting_dialog_mock = new TraceFilterSettingDialogMock();
   }
}
void TraceFilterSettingDialogMock_deinit()
{
   if (g_trace_filter_setting_dialog_mock)
   {
      delete g_trace_filter_setting_dialog_mock;
      g_trace_filter_setting_dialog_mock = nullptr;
   }
}
TraceFilterSettingDialogMock* TraceFilterSettingDialogMock_get()
{
   UT_Assert(g_trace_filter_setting_dialog_mock && "Create TraceFilterSettingDialogMock mock first!");
   return g_trace_filter_setting_dialog_mock;

}

namespace Dialogs
{

TraceFilterSettingDialog::TraceFilterSettingDialog() {}
TraceFilterSettingDialog::~TraceFilterSettingDialog() {}

std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return g_trace_filter_setting_dialog_mock->showDialog(parent, current_settings, out_settings, allow_edit);
}

}
