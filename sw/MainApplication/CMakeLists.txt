if(SIMULATION)
# This library is created in sw/testing/TestStubs
	set(GUI_CONTROLLER_LIB GUIControllerSimulation)
else()
	set(GUI_CONTROLLER_LIB GUIController)
endif()

add_library(MainApplication STATIC 	   
	   source/MainApplication.cpp
	   source/PortHandler.cpp
       source/UserButtonHandler.cpp
       source/TraceFilterHandler.cpp
       source/FileLogger.cpp
       include/MainApplication.h
       include/UserButtonHandler.h
       public/PortHandler.h
       public/TraceFilterHandler.h)

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
   ${GUI_CONTROLLER_LIB}
)

add_library(MainApplicationIf INTERFACE)
target_include_directories(MainApplicationIf INTERFACE 
   public/
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()