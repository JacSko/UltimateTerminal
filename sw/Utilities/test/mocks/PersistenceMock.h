#pragma once
#include <gmock/gmock.h>
#include <vector>
#include "Persistence.h"
#include "Logger.h"

namespace Utilities
{
namespace Persistence
{

struct PersistenceMock
{
   MOCK_METHOD1(loadFile, bool(const std::string&));
   MOCK_METHOD0(restore, void());
   MOCK_METHOD1(save, bool(const std::string&));
   MOCK_METHOD0(clearFile, void());
};

PersistenceMock* g_pers_mock;

void PersistenceMock_init()
{
   if (!g_pers_mock)
   {
      g_pers_mock = new PersistenceMock();
   }
}

void PersistenceMock_deinit()
{
   if (g_pers_mock)
   {
      delete g_pers_mock;
      g_pers_mock = nullptr;
   }
}

PersistenceMock* PersistenceMock_get()
{
   UT_Assert(g_pers_mock && "Create persistence handler mock first!");
   return g_pers_mock;
}
void Persistence::clearFile()
{
   return g_pers_mock->clearFile();
}
bool Persistence::loadFile(const std::string& name)
{
   return g_pers_mock->loadFile(name);
}
void Persistence::restore()
{
   return g_pers_mock->restore();
}
bool Persistence::save(const std::string& file_name)
{
   return g_pers_mock->save(file_name);
}


}
}


