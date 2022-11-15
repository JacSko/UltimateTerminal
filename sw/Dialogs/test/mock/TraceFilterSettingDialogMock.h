#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilterSettingDialog.h"


struct TraceFilterSettingDialogMock
{
   MOCK_METHOD3(showDialog,std::optional<bool>(QWidget*, const Dialogs::TraceFilterSettingDialog::ColorSet&, Dialogs::TraceFilterSettingDialog::ColorSet&));
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

std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const ColorSet& current_settings, ColorSet& out_settings)
{
   return g_trace_filter_setting_dialog_mock->showDialog(parent, current_settings, out_settings);
}

}
