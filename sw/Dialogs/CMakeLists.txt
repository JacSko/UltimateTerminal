add_library(Dialogs STATIC 
   source/PortSettingDialog.cpp
   source/UserButtonDialog.cpp
   source/LoggingSettingDialog.cpp
   public/PortSettingDialog.h
   public/UserButtonDialog.h
   public/LoggingSettingDialog.h
)

target_include_directories(Dialogs PUBLIC 
   public/
)

target_link_libraries(Dialogs
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   Drivers
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()