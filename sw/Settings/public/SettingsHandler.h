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
 *    Settings has it's own thread, which monitors the file content.
 *    It is possible to force settings reload by changing the key in settings file.
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
#include <sys/stat.h>

#include "ITimers.h"
#include "Settings_config.h"

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

class SettingsHandler : public Utilities::ITimerClient
{
public:
   SettingsHandler();
   /**
    * @brief Returns instance of SettingsHandler object.
    * @return Pointer to ISettings object
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
   void start(const std::string& settings_file_path, Utilities::ITimers* timers);
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



private:

   void onTimeout(uint32_t timer_id) override;
   void notifyListeners(KeyID id);
   std::vector<KeyID> applySettings();
   bool checkAndRefresh();
   void reloadSettings();
   void startFileObservation();
   void stopFileObservation();

   Utilities::ITimers* m_timers;
   uint32_t m_timer_id;
   struct timespec m_last_file_modification;
   std::string m_file_path;
   std::mutex m_settings_mutex;
   std::vector<std::pair<KeyID, SettingsListener*>> m_listeners;
};

}

#endif
