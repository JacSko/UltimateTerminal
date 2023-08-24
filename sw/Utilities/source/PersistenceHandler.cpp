#include <sstream>
#include <fstream>
#include <numeric>
#include "PersistenceHandler.h"
#include "Logger.h"
#include "nlohmann/json.hpp"


namespace Persistence
{

bool PersistenceHandler::restore(const std::string& file_name)
{
   using json = nlohmann::json;
   std::ifstream file(file_name);
   bool result = false;
   if (file)
   {
      UT_Log(PERSISTENCE, LOW, "Persistence file opened [%s], reading items", file_name.c_str());
      json jsonFile = json::parse(file);
      for (auto& module : jsonFile.items())
      {
         UT_Log(PERSISTENCE, HIGH, "Processing module %s", module.key().c_str());
         PersistenceListener::PersistenceItems items;
         auto& moduleSettings = jsonFile[module.key()];
         for (auto& setting : moduleSettings.items())
         {
            std::string key = setting.key();
            std::string value = setting.value();
            UT_Log(PERSISTENCE, HIGH, "Got item - module %s key %s value %s", module.key().c_str(), key.c_str(), value.c_str());
            items.push_back(PersistenceListener::PersistenceItem{key, value});
         }
         GenericListener::notifyChange([&](PersistenceListener* l)
               {
                  if (l->getName() == module.key())
                  {
                     UT_Log(PERSISTENCE, HIGH, "Found handler for %s, sending %u items...", module.key().c_str(), items.size());
                     l->onPersistenceRead(items);
                  }
               });
      }
      result = true;
   }
   UT_Log_If(!result, PERSISTENCE, ERROR, "Cannot open [%s]", file_name.c_str());
   return result;
}
bool PersistenceHandler::save(const std::string& file_name)
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

}
