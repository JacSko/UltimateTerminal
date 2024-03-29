/* =============================
 *   Includes of project headers
 * =============================*/
#include "SettingsHandler.h"
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



SettingsHandler* SettingsHandler::get()
{
   static Utilities::Persistence::Persistence g_handler;
   static SettingsHandler g_settings(g_handler);
   return &g_settings;
}
void SettingsHandler::create(Utilities::Persistence::Persistence&)
{
}
void SettingsHandler::destroy()
{
}
SettingsHandler::SettingsHandler(Utilities::Persistence::Persistence& persistence):
m_persistence(persistence)
{

/* load default settings from SettingsHolder.h macro */
#undef DEF_SETTING_GROUP
#define DEF_SETTING_GROUP(name, type, default_value) (set_setting<type>(name, default_value));
   SETTING_GROUPS
#undef DEF_SETTING_GROUP

   UT_Log(SETTINGS, INFO, "Default setting loaded");
}
void SettingsHandler::start()
{
}
void SettingsHandler::stop()
{
}
void SettingsHandler::printSettings()
{
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
void SettingsHandler::registerListener(KeyID, SettingsListener*)
{
}
void SettingsHandler::unregisterListener(KeyID, SettingsListener*)
{
}
void SettingsHandler::notifyListeners(KeyID)
{
}
void SettingsHandler::onPersistenceRead(const PersistenceItems&)
{
}
void SettingsHandler::onPersistenceWrite(PersistenceItems&)
{
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
}
