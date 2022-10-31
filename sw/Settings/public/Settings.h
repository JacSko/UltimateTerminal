#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "SettingsHandler.h"


#define SETTING_GET_U32(name) ::Settings::SettingsHandler::get()->getU32(name)
#define SETTING_GET_BOOL(name) ::Settings::SettingsHandler::get()->getBool(name)
#define SETTING_GET_STRING(name) ::Settings::SettingsHandler::get()->getString(name)

#define SETTING_SET_U32(name, value) ::Settings::SettingsHandler::get()->setU32(name, value)
#define SETTING_SET_BOOL(name, value) ::Settings::SettingsHandler::get()->setBool(name, value)
#define SETTING_SET_STRING(name, value) ::Settings::SettingsHandler::get()->setString(name, value)

#define SETTING_ADD_LISTENER(id, listener) ::Settings::SettingsHandler::get()->registerListener(id, listener)
#define SETTING_REMOVE_LISTENER(id, listener) ::Settings::SettingsHandler::get()->unregisterListener(id, listener)

#define SETTING_STRING_TO_ID(stringname) ::Settings::SettingsHandler::get()->getID(stringname)

#endif
