#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilterHandler.h"


struct TraceFilterHandlerMock
{
   MOCK_METHOD1(tryMatch,std::optional<Dialogs::TraceFilterSettingDialog::Settings>(const std::string&));
   MOCK_METHOD0(refreshUi, void());
   MOCK_METHOD2(setSettings, bool(uint8_t id, const Dialogs::TraceFilterSettingDialog::Settings&));
   MOCK_METHOD0(getSettings, Dialogs::TraceFilterSettingDialog::Settings());
   MOCK_METHOD0(isActive, bool());
};

TraceFilterHandlerMock* g_trace_filter_mock;

void TraceFilterHandlerMock_init()
{
   if (!g_trace_filter_mock)
   {
      g_trace_filter_mock = new TraceFilterHandlerMock();
   }
}
void TraceFilterHandlerMock_deinit()
{
   if (g_trace_filter_mock)
   {
      delete g_trace_filter_mock;
      g_trace_filter_mock = nullptr;
   }
}
TraceFilterHandlerMock* TraceFilterHandlerMock_get()
{
   UT_Assert(g_trace_filter_mock && "Create TraceFilterHandlerMock mock first!");
   return g_trace_filter_mock;
}


TraceFilterHandler::TraceFilterHandler(uint8_t id, GUIController& controller, const std::string&, Persistence::PersistenceHandler& persistence):
m_gui_controller(controller),
m_persistence(persistence)
{
   m_settings.id = id;
}
TraceFilterHandler::~TraceFilterHandler(){}
std::optional<Dialogs::TraceFilterSettingDialog::Settings> TraceFilterHandler::tryMatch(const std::string& text)
{
   return TraceFilterHandlerMock_get()->tryMatch(text);
}
void TraceFilterHandler::refreshUi()
{
   TraceFilterHandlerMock_get()->refreshUi();
}
bool TraceFilterHandler::setSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   return TraceFilterHandlerMock_get()->setSettings(m_settings.id, settings);
}
Dialogs::TraceFilterSettingDialog::Settings TraceFilterHandler::getSettings()
{
   return TraceFilterHandlerMock_get()->getSettings();
}
bool TraceFilterHandler::isActive()
{
   return TraceFilterHandlerMock_get()->isActive();
}
void TraceFilterHandler::onPersistenceRead(const std::vector<uint8_t>&){}
void TraceFilterHandler::onPersistenceWrite(std::vector<uint8_t>&){}
void TraceFilterHandler::onButtonEvent(uint32_t, ButtonEvent){};

