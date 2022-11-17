#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "TraceFilterHandler.h"


struct TraceFilterHandlerMock
{
   MOCK_METHOD1(tryMatch,std::optional<Dialogs::TraceFilterSettingDialog::ColorSet>(const std::string&));
   MOCK_METHOD0(refreshUi, void());
};

TraceFilterHandlerMock* g_trace_filter_mock;;

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


TraceFilterHandler::TraceFilterHandler(QWidget*, QLineEdit*, QPushButton*, Persistence::PersistenceHandler& persistence):
m_persistence(persistence)
{
}
TraceFilterHandler::~TraceFilterHandler(){}
std::optional<Dialogs::TraceFilterSettingDialog::ColorSet> TraceFilterHandler::tryMatch(const std::string& text)
{
   return TraceFilterHandlerMock_get()->tryMatch(text);
}
void TraceFilterHandler::refreshUi()
{
   TraceFilterHandlerMock_get()->refreshUi();
}
void TraceFilterHandler::onPersistenceRead(const std::vector<uint8_t>&){}
void TraceFilterHandler::onPersistenceWrite(std::vector<uint8_t>&){}

