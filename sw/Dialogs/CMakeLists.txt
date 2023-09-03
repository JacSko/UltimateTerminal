if(NOT SIMULATION)
add_library(Dialogs STATIC 
   source/PortSettingDialog.cpp
   source/UserButtonDialog.cpp
   source/LoggingSettingDialog.cpp
   source/TraceFilterSettingDialog.cpp
   source/ApplicationSettingsDialog.cpp
   source/MessageDialog.cpp
   source/TabNameDialog.cpp
   public/PortSettingDialog.h
   public/UserButtonDialog.h
   public/LoggingSettingDialog.h
   public/TraceFilterSettingDialog.h
   public/ApplicationSettingsDialog.h
   public/TabNameDialog.h
)

target_include_directories(Dialogs PUBLIC 
   public/
)

target_link_libraries(Dialogs
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   DriversIf
   MainApplicationIf
   GUIController
)
endif()

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

add_library(DialogsIf INTERFACE)
target_include_directories(DialogsIf INTERFACE 
   public/
)