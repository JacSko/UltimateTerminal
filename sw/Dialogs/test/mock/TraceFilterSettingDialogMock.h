#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilterSettingDialog.h"


struct TraceFilterSettingDialogMock
{
   MOCK_METHOD4(showDialog,std::optional<bool>(QWidget*, const Dialogs::TraceFilterSettingDialog::Settings&, Dialogs::TraceFilterSettingDialog::Settings&, bool));
   MOCK_METHOD3(createLayout, QLayout*(QWidget* parent, const Dialogs::TraceFilterSettingDialog::Settings& current_settings, bool allow_edit));
   MOCK_METHOD0(destroyLayout, void());
   MOCK_METHOD2(convertGuiValues, bool(uint8_t id, Dialogs::TraceFilterSettingDialog::Settings&));
};

TraceFilterSettingDialogMock* g_trace_filter_setting_dialog_mock;
static uint8_t TRACE_FILTER_DIALOG_ID;

void TraceFilterSettingDialogMock_init()
{
   if (!g_trace_filter_setting_dialog_mock)
   {
      TRACE_FILTER_DIALOG_ID = 0;
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

TraceFilterSettingDialog::TraceFilterSettingDialog()
{
   TRACE_FILTER_DIALOG_ID++;
   m_settings.id = TRACE_FILTER_DIALOG_ID;
}
TraceFilterSettingDialog::~TraceFilterSettingDialog() {}

std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   return g_trace_filter_setting_dialog_mock->showDialog(parent, current_settings, out_settings, allow_edit);
}
QLayout* TraceFilterSettingDialog::createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit)
{
   return g_trace_filter_setting_dialog_mock->createLayout(parent, current_settings, allow_edit);
}
void TraceFilterSettingDialog::destroyLayout()
{
   g_trace_filter_setting_dialog_mock->destroyLayout();
}
bool TraceFilterSettingDialog::convertGuiValues(Settings& out_settings)
{
   return g_trace_filter_setting_dialog_mock->convertGuiValues(m_settings.id, out_settings);
}
}
