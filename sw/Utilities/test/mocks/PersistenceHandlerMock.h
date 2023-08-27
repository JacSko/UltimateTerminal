#pragma once
#include <vector>
#include "PersistenceHandler.h"

namespace Persistence
{

struct PersistenceHandlerMock
{
   MOCK_METHOD1(loadFile, bool(const std::string&));
   MOCK_METHOD0(restore, void());
   MOCK_METHOD1(save, bool(const std::string&));
};

PersistenceHandlerMock* g_pers_handler_mock;

void PersistenceHandlerMock_init()
{
   if (!g_pers_handler_mock)
   {
      g_pers_handler_mock = new PersistenceHandlerMock();
   }
}

void PersistenceHandlerMock_deinit()
{
   if (g_pers_handler_mock)
   {
      delete g_pers_handler_mock;
      g_pers_handler_mock = nullptr;
   }
}

PersistenceHandlerMock* PersistenceHandlerMock_get()
{
   UT_Assert(g_pers_handler_mock && "Create persistence handler mock first!");
   return g_pers_handler_mock;
}
bool PersistenceHandler::loadFile(const std::string& name)
{
   return g_pers_handler_mock->loadFile(name);
}
void PersistenceHandler::restore()
{
   return g_pers_handler_mock->restore();
}
bool PersistenceHandler::save(const std::string& file_name)
{
   return g_pers_handler_mock->save(file_name);
}


}


