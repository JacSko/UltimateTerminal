add_library(Dialogs STATIC 
   source/PortSettingDialog.cpp
   source/UserButtonDialog.cpp
   source/LoggingSettingDialog.cpp
   source/TraceFilterSettingDialog.cpp
   source/ApplicationSettingsDialog.cpp
   public/PortSettingDialog.h
   public/UserButtonDialog.h
   public/LoggingSettingDialog.h
   public/TraceFilterSettingDialog.h
   public/ApplicationSettingsDialog.h
)

target_include_directories(Dialogs PUBLIC 
   public/
)

target_link_libraries(Dialogs
   Qt6::Widgets
   SettingsHandler
   Logger
   Utilities
   Drivers
   MainApplicationIf
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()