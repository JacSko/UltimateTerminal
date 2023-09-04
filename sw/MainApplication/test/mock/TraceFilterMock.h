#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilter.h"

namespace MainApplication
{

struct TraceFilterMock
{
   MOCK_METHOD1(tryMatch,std::optional<Dialogs::TraceFilterSettingDialog::Settings>(const std::string&));
   MOCK_METHOD0(refreshUi, void());
   MOCK_METHOD2(setSettings, bool(uint8_t id, const Dialogs::TraceFilterSettingDialog::Settings&));
   MOCK_METHOD0(getSettings, Dialogs::TraceFilterSettingDialog::Settings());
   MOCK_METHOD0(isActive, bool());
};

TraceFilterMock* g_trace_filter_mock;

void TraceFilterMock_init()
{
   if (!g_trace_filter_mock)
   {
      g_trace_filter_mock = new TraceFilterMock();
   }
}
void TraceFilterMock_deinit()
{
   if (g_trace_filter_mock)
   {
      delete g_trace_filter_mock;
      g_trace_filter_mock = nullptr;
   }
}
TraceFilterMock* TraceFilterMock_get()
{
   UT_Assert(g_trace_filter_mock && "Create TraceFilterMock mock first!");
   return g_trace_filter_mock;
}


TraceFilter::TraceFilter(uint8_t id, GUIController::GUIController& controller, const std::string&, Utilities::Persistence::Persistence& persistence):
m_gui_controller(controller),
m_persistence(persistence)
{
   m_settings.id = id;
}
TraceFilter::~TraceFilter(){}
std::optional<Dialogs::TraceFilterSettingDialog::Settings> TraceFilter::tryMatch(const std::string& text)
{
   return TraceFilterMock_get()->tryMatch(text);
}
void TraceFilter::refreshUi()
{
   TraceFilterMock_get()->refreshUi();
}
bool TraceFilter::setSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   return TraceFilterMock_get()->setSettings(m_settings.id, settings);
}
Dialogs::TraceFilterSettingDialog::Settings TraceFilter::getSettings()
{
   return TraceFilterMock_get()->getSettings();
}
bool TraceFilter::isActive()
{
   return TraceFilterMock_get()->isActive();
}
void TraceFilter::onPersistenceRead(const PersistenceItems&){};
void TraceFilter::onPersistenceWrite(PersistenceItems&){};
void TraceFilter::onButtonEvent(uint32_t, GUIController::ButtonEvent){};

}
