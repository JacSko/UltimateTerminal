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
   DEF_SETTING_GROUP(Logger_supportFileLogging, bool, false) \
   DEF_SETTING_GROUP(Logger_supportStdoutLogging, bool, true) \
   DEF_SETTING_GROUP(Logger_supportSocketLogging, bool, false) \
   DEF_SETTING_GROUP(Logger_socketPort, uint32_t, 5555) \
   DEF_SETTING_GROUP(Logger_logfilePath, std::string, "/home/jskowronek/projects/HomeController/build/log.log") \
   DEF_SETTING_GROUP(Logger_group_MAIN, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SETTINGS, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_UTILITIES, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_SOCK_DRV, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_I2C_DRIVER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_DHT_DRIVER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_INT_DRIVER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_RPC_SERVER, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_RPC_CLIENT, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_HC_API, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_BOARD, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_ENV_CTRL, std::string, "HIGH") \
   DEF_SETTING_GROUP(Logger_group_TEST_CORE, std::string, "HIGH") \
   DEF_SETTING_GROUP(HWStub_RPC_Port, uint32_t, 4567) \
   DEF_SETTING_GROUP(HC_API_Port, uint32_t, 4444) \
   DEF_SETTING_GROUP(I2C_Bus_device, std::string, "/dev/i2c-1") \
   DEF_SETTING_GROUP(Relays_I2C_address, uint32_t, 40) \
   DEF_SETTING_GROUP(Relays_check_enabled, bool, true) \
   DEF_SETTING_GROUP(Relays_check_timeout, uint32_t, 5000) \
   DEF_SETTING_GROUP(Relays_use_inverted_logic, bool, true) \
   DEF_SETTING_GROUP(RelayID_Wardrobe_Led, uint32_t, 1) \
   DEF_SETTING_GROUP(RelayID_Bathroom_Led, uint32_t, 2) \
   DEF_SETTING_GROUP(RelayID_Staircase_Led, uint32_t, 8) \
   DEF_SETTING_GROUP(RelayID_Sockets_230, uint32_t, 9) \
   DEF_SETTING_GROUP(RelayID_BathroomFan_230, uint32_t, 10) \
   DEF_SETTING_GROUP(RelayID_KitchenBrickWall_230, uint32_t, 11) \
   DEF_SETTING_GROUP(RelayID_Staircase_230, uint32_t, 12) \
   DEF_SETTING_GROUP(RelayID_Bathroom_230, uint32_t, 13) \
   DEF_SETTING_GROUP(RelayID_Kitchen_230, uint32_t, 14) \
   DEF_SETTING_GROUP(RelayID_Bedroom_230, uint32_t, 15) \
   DEF_SETTING_GROUP(RelayID_Wardrobe_230, uint32_t, 16) \
   DEF_SETTING_GROUP(Inputs_I2C_address, uint32_t, 32) \
   DEF_SETTING_GROUP(Inputs_check_enabled, bool, true) \
   DEF_SETTING_GROUP(Inputs_check_timeout, uint32_t, 5000) \
   DEF_SETTING_GROUP(Inputs_use_inverted_logic, bool, true) \
   DEF_SETTING_GROUP(Inputs_Interrupt_use_falling_edge, bool, true) \
   DEF_SETTING_GROUP(Inputs_Interrupt_Gpio, uint32_t, 27) \
   DEF_SETTING_GROUP(InputID_Wardrobe_Led, uint32_t, 1) \
   DEF_SETTING_GROUP(InputID_Bathroom_Led, uint32_t, 2) \
   DEF_SETTING_GROUP(InputID_Sockets_230, uint32_t, 9) \
   DEF_SETTING_GROUP(InputID_Bedroom_230, uint32_t, 10) \
   DEF_SETTING_GROUP(InputID_Wardrobe_230, uint32_t, 11) \
   DEF_SETTING_GROUP(InputID_Kitchen_230, uint32_t, 12) \
   DEF_SETTING_GROUP(InputID_Bathroom_230, uint32_t, 13) \
   DEF_SETTING_GROUP(InputID_Staircase_230, uint32_t, 14) \
   DEF_SETTING_GROUP(InputID_StairsSensor_230, uint32_t, 15) \
   DEF_SETTING_GROUP(InputID_KitchenBrickWall_230, uint32_t, 16) \
   DEF_SETTING_GROUP(SLD_I2C_address, uint32_t, 44) \
   DEF_SETTING_GROUP(DHT_Bus_device, std::string, "/sys/devices/platform/dht11@11/iio:device0") \
   DEF_SETTING_GROUP(ENV_Wardrobe_SensorID, uint32_t, 1) \
   DEF_SETTING_GROUP(ENV_Bedroom_SensorID, uint32_t, 2) \
   DEF_SETTING_GROUP(ENV_Bathroom_SensorID, uint32_t, 3) \
   DEF_SETTING_GROUP(ENV_Kitchen_SensorID, uint32_t, 4) \
   DEF_SETTING_GROUP(ENV_Stairs_SensorID, uint32_t, 5) \
   DEF_SETTING_GROUP(ENV_Outside_SensorID, uint32_t, 6) \
   DEF_SETTING_GROUP(ENV_Sensor_Switch_Timeout, uint32_t, 1000) \
   DEF_SETTING_GROUP(FAN_Start_Humidity, uint32_t, 80) \
   DEF_SETTING_GROUP(FAN_Stop_Humidity, uint32_t, 75) \
   DEF_SETTING_GROUP(FAN_Max_Working_Time_sec, uint32_t, 3600) \
   DEF_SETTING_GROUP(FAN_Lock_Time_sec, uint32_t, 3600) \
   DEF_SETTING_GROUP(INPSEL_Line_A_Gpio, uint32_t, 11) \
   DEF_SETTING_GROUP(INPSEL_Line_B_Gpio, uint32_t, 12) \
   DEF_SETTING_GROUP(INPSEL_Line_C_Gpio, uint32_t, 13) \


#endif