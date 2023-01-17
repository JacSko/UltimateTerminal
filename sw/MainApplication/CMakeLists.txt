add_library(MainApplication STATIC 
   source/MainApplication.cpp
   source/PortHandler.cpp
   source/UserButtonHandler.cpp
   source/TraceFilterHandler.cpp
   source/FileLogger.cpp
   source/GUIController.cpp
   include/MainApplication.h
   include/UserButtonHandler.h
   public/PortHandler.h
   public/TraceFilterHandler.h
   public/GUIController.h
)

target_include_directories(MainApplication PUBLIC 
   include/
   public/
)

target_link_libraries(MainApplication
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   Drivers
   Dialogs
)

add_library(MainApplicationIf INTERFACE)
target_include_directories(MainApplicationIf INTERFACE 
   public/
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()