add_library(MainApplication STATIC 
   source/MainApplication.cpp
   source/PortHandler.cpp
   source/UserButtonHandler.cpp
   source/TraceFilterHandler.cpp
   source/FileLogger.cpp
   include/MainApplication.h
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
   Dialogs
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()