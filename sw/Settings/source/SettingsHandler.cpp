/* =============================
 *   Includes of project headers
 * =============================*/
#include "SettingsHandler.h"
#include "nlohmann/json.hpp"
#include "Logger.h"
#include "Settings.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <array>
#include <map>
#include <fstream>


namespace Settings
{

#undef DEF_SETTING_GROUP
#define DEF_SETTING_GROUP(name, type, default_value) #name,
std::array<std::string, SETTING_GROUP_MAX> m_setting_names = {SETTING_GROUPS};
#undef DEF_SETTING_GROUP

static std::map<KeyID, uint32_t> m_u32_items;
static std::map<KeyID, bool> m_bool_items;
static std::map<KeyID, std::string> m_string_items;

template<typename T>
void set_setting(KeyID id, T){}

template<>
void set_setting<uint32_t>(KeyID id, uint32_t setting)
{
   m_u32_items[id] = setting;
}
template<>
void set_setting<std::string>(KeyID id, std::string setting)
{
   m_string_items[id] = setting;
}
template<>
void set_setting<bool>(KeyID id, bool setting)
{
   m_bool_items[id] = setting;
}


SettingsHandler* g_settings;

SettingsHandler* SettingsHandler::get()
{
   UT_Assert(g_settings);
   return g_settings;
}

void SettingsHandler::create()
{
   g_settings = new SettingsHandler();
}

void SettingsHandler::destroy()
{
   delete g_settings;
}

SettingsHandler::SettingsHandler()
{

/* load default settings from SettingsHolder.h macro */
#undef DEF_SETTING_GROUP
#define DEF_SETTING_GROUP(name, type, default_value) (set_setting<type>(name, default_value));
   SETTING_GROUPS
#undef DEF_SETTING_GROUP

   UT_Log(SETTINGS, INFO, "Default setting loaded");
}

void SettingsHandler::start(const std::string& settings_file_path)
{
   m_file_path = settings_file_path;
   parseSettings();
}

void SettingsHandler::stop()
{
}

bool SettingsHandler::parseSettings()
{
   bool result = false;
   std::ifstream file(m_file_path);
   if (file)
   {
      applySettings();
      printSettings();
      result = true;
   }
   return result;
}
std::vector<KeyID> SettingsHandler::applySettings()
{
   std::vector<KeyID> result;
   using json = nlohmann::json;
   json j;
   std::ifstream file(m_file_path);
   if (file)
   {
      file >> j;
      for (auto it = m_setting_names.begin(); it < m_setting_names.end(); it++)
      {
         json::value_t type = j[m_setting_names[std::distance(m_setting_names.begin(), it)]].type();
         switch(type)
         {
         case json::value_t::number_integer:
         case json::value_t::number_unsigned:
            if (setU32((KeyID)std::distance(m_setting_names.begin(), it), j[m_setting_names[std::distance(m_setting_names.begin(), it)]].get<uint32_t>()))
            {
               result.push_back((KeyID)std::distance(m_setting_names.begin(), it));
            }
            break;
         case json::value_t::boolean:
            if (setBool((KeyID)std::distance(m_setting_names.begin(), it), j[m_setting_names[std::distance(m_setting_names.begin(), it)]].get<bool>()))
            {
               result.push_back((KeyID)std::distance(m_setting_names.begin(), it));
            }
            break;
         case json::value_t::string:
            if(setString((KeyID)std::distance(m_setting_names.begin(), it), j[m_setting_names[std::distance(m_setting_names.begin(), it)]].get<std::string>()))
            {
               result.push_back((KeyID)std::distance(m_setting_names.begin(), it));
            }
            break;
         default:
            break;
         }
      }
   }
   return result;
}

void SettingsHandler::printSettings()
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Log(SETTINGS, INFO, "U32 SETTINGS:");
   for (auto& it : m_u32_items)
   {
      UT_Log(SETTINGS, INFO, "%s => %u", m_setting_names[(uint32_t)it.first].c_str(), it.second);
   }
   UT_Log(SETTINGS, INFO, "BOOL SETTINGS:");
   for (auto& it : m_bool_items)
   {
      UT_Log(SETTINGS, INFO, "%s => %s", m_setting_names[(uint32_t)it.first].c_str(), it.second? "true" : "false");
   }
   UT_Log(SETTINGS, INFO, "STRING SETTINGS:");
   for (auto& it : m_string_items)
   {
      UT_Log(SETTINGS, INFO, "%s => %s", m_setting_names[(uint32_t)it.first].c_str(), it.second.c_str());
   }

}

uint32_t SettingsHandler::getU32(KeyID id)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   return m_u32_items[id];
}

std::string SettingsHandler::getString(KeyID id)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   return m_string_items[id];
}

bool SettingsHandler::getBool(KeyID id)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   return m_bool_items[id];
}

bool SettingsHandler::setU32(KeyID id, uint32_t value)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   uint32_t old = m_u32_items[id];
   m_u32_items[id] = value;
   bool changed = old != value;
   notifyListeners(id);
   return changed;
}

bool SettingsHandler::setString(KeyID id, const std::string& value)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   std::string old = m_string_items[id];
   m_string_items[id] = value;
   bool changed = old != value;
   notifyListeners(id);
   return changed;
}

KeyID SettingsHandler::getID(const std::string& name)
{
   KeyID result = SETTING_GROUP_MAX;
   auto it = std::find(m_setting_names.begin(), m_setting_names.end(), name);
   if (it != m_setting_names.end())
   {
      result = (KeyID) std::distance(m_setting_names.begin(), it);
   }
   return result;
}

bool SettingsHandler::setBool(KeyID id, bool value)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Assert(id < SETTING_GROUP_MAX);
   bool old = m_bool_items[id];
   m_bool_items[id] = value;
   bool changed = old != value;
   notifyListeners(id);
   return changed;
}

void SettingsHandler::registerListener(KeyID id, SettingsListener* listener)
{
   if (listener)
   {
      std::lock_guard<std::mutex> lock(m_settings_mutex);
      m_listeners.push_back(std::pair<KeyID, SettingsListener*>(id, listener));
   }
}

void SettingsHandler::unregisterListener(KeyID id, SettingsListener* listener)
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), std::make_pair(id, listener));
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}

void SettingsHandler::notifyListeners(KeyID id)
{
   for (auto& listener : m_listeners)
   {
      if (listener.first == id)
      {
         listener.second->onSettingsChange(id);
      }
   }
}


}
