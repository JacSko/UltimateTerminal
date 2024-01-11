#include <sstream>
#include <fstream>
#include <numeric>
#include "nlohmann/json.hpp"
#include "Persistence.h"
#include "Logger.h"


nlohmann::json g_jsonFile;

namespace Utilities
{
namespace Persistence
{
bool Persistence::loadFile(const std::string& file_name)
{
   std::ifstream file(file_name);
   bool result = false;
   if (file)
   {
      UT_Log(PERSISTENCE, LOW, "Persistence file opened [%s], reading file content", file_name.c_str());
      g_jsonFile = nlohmann::json::parse(file);
      result = true;
   }
   UT_Log_If(!result, PERSISTENCE, ERROR, "Cannot open [%s]", file_name.c_str());
   return result;
}
void Persistence::restore()
{
   UT_Log(PERSISTENCE, LOW, "Restoring persistence items");

   GenericListener::notifyChange([&](PersistenceListener* l)
         {
               UT_Assert(l);
               restoreModule(*l);
         });
}
bool Persistence::save(const std::string& file_name)
{
   using json = nlohmann::json;
   json jsonFile;
   std::ofstream file(file_name);
   bool result = false;
   if (file)
   {
      UT_Log(PERSISTENCE, LOW, "Persistence file opened [%s], collect settings", file_name.c_str());
      GenericListener::notifyChange([&](PersistenceListener* l)
            {
               UT_Log(PERSISTENCE, HIGH, "Gathering settings for %s", l->getName().c_str());
               PersistenceListener::PersistenceItems items = {};
               l->onPersistenceWrite(items);
               UT_Log(PERSISTENCE, HIGH, "Got %u items from %s", items.size(), l->getName().c_str());
               auto& moduleJson = jsonFile[l->getName()];
               for (auto& item : items)
               {
                  moduleJson[item.key] = item.value;
               }
            });
      UT_Log(PERSISTENCE, LOW, "All settings collected (%u entries), writing to file", jsonFile.size());
      file << jsonFile;
      file.flush();
      file.close();
      result = true;
   }
   UT_Log_If(!result, PERSISTENCE, ERROR, "Cannot open [%s]", file_name.c_str());
   return result;
}
void Persistence::restoreModule(PersistenceListener& listener)
{
   UT_Log(PERSISTENCE, HIGH, "%s %s", __func__, listener.getName().c_str());
   PersistenceListener::PersistenceItems items = {};
   auto& moduleSettings = g_jsonFile[listener.getName()];
   for (auto& setting : moduleSettings.items())
   {
      std::string key = setting.key();
      std::string value = setting.value();
      UT_Log(PERSISTENCE, HIGH, "Got item - module %s key %s value %s", listener.getName().c_str(), key.c_str(), value.c_str());
      items.push_back(PersistenceListener::PersistenceItem{key, value});
   }
   UT_Log_If(items.empty(), PERSISTENCE, LOW, "No keys found for [%s]", listener.getName().c_str());
   if (!items.empty())
   {
      listener.onPersistenceRead(items);
   }
}
void Persistence::clearFile()
{
   g_jsonFile = {};
}

}
}
