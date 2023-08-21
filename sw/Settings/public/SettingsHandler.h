#ifndef _I_SETTINGS_HANDLER_H
#define _I_SETTINGS_HANDLER_H

/**
 * @file ISettingsHandler.h
 *
 * @brief
 *    Interface for reading, writing settings to file and notifying application about them.
 *
 * @details
 *    Settings are stored in defined file on filesystem. File path is provided via compilation variable.
 *    If settings file is broken or missing, the default settings are sent out to other components.
 *
 * @author Jacek Skowronek
 * @date   22/03/2022
 *
 */

/* =============================
 *   Includes of common headers
 * =============================*/
#include <stdint.h>
#include <string>
#include <array>
#include <mutex>
#include <vector>

#include "Settings_config.h"
#include "PersistenceHandler.h"

#undef DEF_SETTING_GROUP
#define DEF_SETTING_GROUP(name, type, default_value) name,
typedef enum
{
   SETTING_GROUPS
   SETTING_GROUP_MAX
} KeyID;
#undef DEF_SETTING_GROUP

namespace Settings
{

enum class SettingType
{
   U32,
   BOOL,
   STRING,
   UNKNOWN,
};

class SettingsListener
{
public:
   virtual ~SettingsListener(){};
   /**
    * @brief Callback called when new data arrives.
    * @param[in] id - ID of the settings.
    * @return None.
    */
   virtual void onSettingsChange(KeyID key_id) = 0;

};

class SettingsHandler : public Persistence::PersistenceListener
{
public:
   SettingsHandler();
   /**
    * @brief Returns instance of SettingsHandler object.
    * @return Pointer to SettingsHandler object
    */
   static SettingsHandler* get();
   /**
    * @brief Creates SettingsHandler object.
    * @return void
    */
   static void create();
   /**
    * @brief Destroys SettingsHandler object.
    * @return void
    */
   static void destroy();
   /**
    * @brief Starts settings handler module.
    * @details Call to start() reads content from JSON file and parsing the settings.
    * @param[in] settings_file_path - path to JSON file with settings.
    * @return void
    */
   void start(const std::string& settings_file_path);
   /**
    * @brief Stops settings handler module.
    * @return void
    */
   void stop();
   /**
    * @brief Reads settings from file.
    * @param[in] settings_file_path - path to settings file.
    * @return True if parsed correctly.
    */
   bool parseSettings();
   /**
    * @brief Get setting value.
    * @param[in] id - id of the setting.
    * @return setting value
    */
   bool getBool(KeyID id);
   /**
    * @brief Get setting value.
    * @param[in] id - id of the setting.
    * @return setting value
    */
   uint32_t getU32(KeyID id);
   /**
    * @brief Get setting value.
    * @param[in] id - id of the setting.
    * @return setting value
    */
   std::string getString(KeyID id);
   /**
    * @brief Set setting value.
    * @param[in] id - id of the setting.
    * @param[in] value - value of the setting.
    * @return True if value was different than previous
    */
   bool setBool(KeyID id, bool value);
   /**
    * @brief Set setting value.
    * @param[in] id - id of the setting.
    * @param[in] value - value of the setting.
    * @return True if value was different than previous
    */
   bool setU32(KeyID id, uint32_t value);
   /**
    * @brief Set setting value.
    * @param[in] id - id of the setting.
    * @param[in] value - value of the setting.
    * @return True if value was different than previous
    */
   bool setString(KeyID id, const std::string& value);
   /**
    * @brief Convert setting name to ID.
    * @param[in] name - name of the setting.
    * @return KeyID if found, otherwise SETTING_GROUP_MAX
    */
   KeyID getID(const std::string& name);
   /**
    * @brief Register callback to be called on settings change.
    * @param[in] id - id of the setting.
    * @param[in] callback - pointer to function to be called.
    * @return void
    */
   void registerListener(KeyID id, SettingsListener*);
   /**
    * @brief Unregister callback.
    * @param[in] id - id of the setting.
    * @param[in] callback - pointer to already registered callback.
    * @return void
    */
   void unregisterListener(KeyID id, SettingsListener*);
   /**
    * @brief Prints all the system settings.
    * @return void
    */
   void printSettings();
   /**
    * @brief Get settings name in textual form.
    * @param[in] id - id of the string.
    * @return Name of the setting
    */
   std::string toString(KeyID id);
   /**
    * @brief Get settings id from name.
    * @param[in] name - name of the string.
    * @return id of the setting
    */
   KeyID fromString(const std::string& name);
   /**
    * @brief Get type of the settings basing on id.
    * @param[in] id - id of the setting.
    * @return Type of the setting
    */
   SettingType getType(KeyID id);
   /**
    * @brief Get path to file with settings.
    * @return Path as a string.
    */
   std::string getFilePath();
private:
   void notifyListeners(KeyID id);
   std::vector<KeyID> applySettings();
   void onPersistenceRead(const std::vector<uint8_t>& data);
   bool settingExist(const std::string& name);
   void onPersistenceWrite(std::vector<uint8_t>& data);

   std::string m_file_path;
   std::mutex m_settings_mutex;
   std::vector<std::pair<KeyID, SettingsListener*>> m_listeners;
   Persistence::PersistenceHandler m_persistence;
};

}

#endif
