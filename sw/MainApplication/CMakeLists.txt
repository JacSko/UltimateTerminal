add_library(MainApplication STATIC 
   source/MainApplication.cpp
   source/PortSettingDialog.cpp
   source/UserButtonDialog.cpp
   source/LoggingSettingDialog.cpp
   source/PortHandler.cpp
   source/UserButtonHandler.cpp
   source/TraceFilterHandler.cpp
   ../gui/MainWindow.ui
   include/MainApplication.h
   include/PortSettingDialog.h
   include/UserButtonDialog.h
   include/LoggingSettingDialog.h
   include/PortHandler.h
   include/UserButtonHandler.h
   include/TraceFilterHandler.h
)

target_include_directories(MainApplication PUBLIC 
   include/
)

target_link_libraries(MainApplication
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   Drivers
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()