#ifndef _SETTINGS_CONFIG_H
#define _SETTINGS_CONFIG_H

/* ==========================================================*/
/* ========== auto-generated file - do not modify! ==========*/
/* ==========================================================*/
/* ==== edit system_config.json and rebuild the target! =====*/
/* ==========================================================*/


#define SETTING_GROUPS\
   DEF_SETTING_GROUP(Settings_reloadOnFileChange, bool, true) \
   DEF_SETTING_GROUP(Settings_fileCheckPeriod, uint32_t, 5000) \
   DEF_SETTING_GROUP(Persistence_filePath, std::string, "/home/jskowronek/projects/UltimateTerminal/build/persistence.bin") \
   DEF_SETTING_GROUP(Logger_supportFileLogging, bool, false) \
   DEF_SETTING_GROUP(Logger_supportStdoutLogging, bool, true) \
   DEF_SETTING_GROUP(Logger_supportSocketLogging, bool, false) \
   DEF_SETTING_GROUP(Logger_socketPort, uint32_t, 5555) \
   DEF_SETTING_GROUP(Logger_logfilePath, std::string, "/home/jskowronek/projects/UltimateTerminal/build/log.log") \
   DEF_SETTING_GROUP(Logger_group_MAIN, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_MAIN_GUI, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SETTINGS, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_UTILITIES, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SOCK_DRV, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SERIAL_DRV, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_TRACE_FILTER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_PORT_HANDLER, std::string, "HIGH") \
   DEF_SETTING_GROUP(PortSettingDialog_maxLineEditLength, uint32_t, 30) \


#endif