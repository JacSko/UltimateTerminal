#ifndef _SETTINGS_CONFIG_H
#define _SETTINGS_CONFIG_H

#define SETTING_GROUPS\
   DEF_SETTING_GROUP(Logger_supportFileLogging, bool, true) \
   DEF_SETTING_GROUP(Logger_supportStdoutLogging, bool, false) \
   DEF_SETTING_GROUP(Logger_supportSocketLogging, bool, true) \
   DEF_SETTING_GROUP(Logger_socketPort, uint32_t, 5555) \
   DEF_SETTING_GROUP(Logger_group_MAIN, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_GUI_CONTROLLER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SETTINGS, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_UTILITIES, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SOCK_DRV, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SERIAL_DRV, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_TRACE_FILTER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_PORT_HANDLER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_USER_BTN_HANDLER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_PERSISTENCE, std::string, "HIGH") \
   DEF_SETTING_GROUP(MainApplication_maxTerminalTraces, uint32_t, 10000) \
   DEF_SETTING_GROUP(MainApplication_statusBarTimeout, uint32_t, 10000) \
   DEF_SETTING_GROUP(PortSettingDialog_maxLineEditLength, uint32_t, 30) \
   DEF_SETTING_GROUP(GUI_Dark_WindowBackground, uint32_t, 3947580) \
   DEF_SETTING_GROUP(GUI_Dark_WindowText, uint32_t, 11842740) \
   DEF_SETTING_GROUP(GUI_Dark_TerminalBackground, uint32_t, 5263440) \
   DEF_SETTING_GROUP(GUI_UserButtons_Tabs, uint32_t, 3) \
   DEF_SETTING_GROUP(GUI_UserButtons_RowsPerTab, uint32_t, 2) \
   DEF_SETTING_GROUP(GUI_UserButtons_ButtonsPerRow, uint32_t, 5) \
   DEF_SETTING_GROUP(Throughput_Calculation_Window, uint32_t, 200) \


#endif
