if(SIMULATION)
# This library is created in sw/testing/TestStubs
	set(GUI_CONTROLLER_LIB GUIControllerSimulation)
else()
	set(GUI_CONTROLLER_LIB GUIController)
endif()

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
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   Drivers
   MainApplicationIf
   ${GUI_CONTROLLER_LIB}
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()