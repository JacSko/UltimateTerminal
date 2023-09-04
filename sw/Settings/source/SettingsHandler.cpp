/* =============================
 *   Includes of project headers
 * =============================*/
#include "SettingsHandler.h"
#include "nlohmann/json.hpp"
#include "Logger.h"
#include "Settings.h"
#include "Serialize.hpp"
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
template<typename T>
T get_setting(KeyID id){}

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
template<>
uint32_t get_setting<uint32_t>(KeyID id)
{
   return m_u32_items[id];
}
template<>
std::string get_setting<std::string>(KeyID id)
{
   return m_string_items[id];
}
template<>
bool get_setting<bool>(KeyID id)
{
   return m_bool_items[id];
}


SettingsHandler* g_settings;

SettingsHandler* SettingsHandler::get()
{
   UT_Assert(g_settings);
   return g_settings;
}

void SettingsHandler::create(Utilities::Persistence::Persistence& persistence)
{
   g_settings = new SettingsHandler(persistence);
}

void SettingsHandler::destroy()
{
   delete g_settings;
}

SettingsHandler::SettingsHandler(Utilities::Persistence::Persistence& persistence):
m_persistence(persistence)
{
   Utilities::Persistence::PersistenceListener::setName("SETTINGS");
/* load default settings from SettingsHolder.h macro */
#undef DEF_SETTING_GROUP
#define DEF_SETTING_GROUP(name, type, default_value) (set_setting<type>(name, default_value));
   SETTING_GROUPS
#undef DEF_SETTING_GROUP

}

void SettingsHandler::start()
{
   m_persistence.addListener(*this);
   m_persistence.restoreModule(*this);
   printSettings();
}

void SettingsHandler::stop()
{
   m_persistence.removeListener(*this);
}
bool SettingsHandler::settingExist(const std::string& name)
{
   return std::find(m_setting_names.begin(), m_setting_names.end(), name) != m_setting_names.end();
}
SettingType SettingsHandler::getType(KeyID id)
{
   for (const auto& item : m_u32_items)
   {
      if (item.first == id)
      {
         return SettingType::U32;
      }
   }
   for (const auto& item : m_bool_items)
   {
      if (item.first == id)
      {
         return SettingType::BOOL;
      }
   }
   for (const auto& item : m_string_items)
   {
      if (item.first == id)
      {
         return SettingType::STRING;
      }
   }
   return SettingType::UNKNOWN;
}

void SettingsHandler::onPersistenceRead(const PersistenceItems& items)
{
   UT_Log(SETTINGS, LOW, "Restoring %u settings from persistence", items.size());
   for (const auto& item : items)
   {
      std::string settingType = "";
      std::string settingValue = "";
      KeyID settingID = fromString(item.key);

      std::stringstream value (item.value);
      std::getline(value, settingType, '.');
      std::getline(value, settingValue);
      UT_Log(SETTINGS, HIGH, "Got setting %s, type %s, value %s", item.key.c_str(), settingType.c_str(), settingValue.c_str());

      if (settingType == "bool")
      {
         bool boolValue = settingValue == "1"? true : false;
         if (settingExist(item.key)) set_setting<bool>(settingID, boolValue);
      }
      else if (settingType == "uint32_t")
      {
         uint32_t u32Value = std::stoi(settingValue);
         if (settingExist(item.key)) set_setting<uint32_t>(settingID, u32Value);
      }
      else if (settingType == "std::string")
      {
         if (settingExist(item.key)) set_setting<std::string>(settingID,settingValue);
      }
   }
   printSettings();
}
void SettingsHandler::onPersistenceWrite(PersistenceItems& buffer)
{
   for (uint32_t i = 0; i < m_setting_names.size(); i++)
   {
      KeyID settingID = fromString(m_setting_names[i]);
      switch(getType(settingID))
      {
      case SettingType::BOOL:
         buffer.push_back(PersistenceItem{m_setting_names[i], std::string("bool") + '.' + std::to_string(get_setting<bool>(settingID))});
         break;
      case SettingType::U32:
         buffer.push_back(PersistenceItem{m_setting_names[i], std::string("uint32_t") + '.' + std::to_string(get_setting<uint32_t>(settingID))});
         break;
      default:
         buffer.push_back(PersistenceItem{m_setting_names[i], std::string("std::string") + '.' + get_setting<std::string>(settingID)});
         break;
      }
   }
   UT_Log(SETTINGS, LOW, "%u settings saved to persistence!", buffer.size());
}

void SettingsHandler::printSettings()
{
   std::lock_guard<std::mutex> lock(m_settings_mutex);
   UT_Log(SETTINGS, LOW, "U32 SETTINGS:");
   for (auto& it : m_u32_items)
   {
      UT_Log(SETTINGS, LOW, "%s => %u", m_setting_names[(uint32_t)it.first].c_str(), it.second);
   }
   UT_Log(SETTINGS, LOW, "BOOL SETTINGS:");
   for (auto& it : m_bool_items)
   {
      UT_Log(SETTINGS, LOW, "%s => %s", m_setting_names[(uint32_t)it.first].c_str(), it.second? "true" : "false");
   }
   UT_Log(SETTINGS, LOW, "STRING SETTINGS:");
   for (auto& it : m_string_items)
   {
      UT_Log(SETTINGS, LOW, "%s => %s", m_setting_names[(uint32_t)it.first].c_str(), it.second.c_str());
   }
}
std::string SettingsHandler::toString(KeyID id)
{
   UT_Assert(id < SETTING_GROUP_MAX);
   return m_setting_names[id];
}
KeyID SettingsHandler::fromString(const std::string& name)
{
   KeyID result = SETTING_GROUP_MAX;
   auto it = std::find(m_setting_names.begin(), m_setting_names.end(), name);
   if (it != m_setting_names.end())
   {
      result = (KeyID)(std::distance(m_setting_names.begin(), it));
   }
   return result;
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
