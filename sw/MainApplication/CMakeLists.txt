if(SIMULATION)
# This library is created in sw/testing/TestStubs
	set(DIALOGS_LIB DialogsSimulation)
else()
	set(DIALOGS_LIB Dialogs)
endif()

add_library(MainApplication STATIC
       source/MainApplication.cpp
       source/Port.cpp
       source/UserButtonsTab.cpp
       source/UserButton.cpp
       source/TraceFilter.cpp
       source/FileLogger.cpp
       include/MainApplication.h
       include/UserButtonsTab.h
       include/UserButton.h
       public/Port.h
       public/TraceFilter.h)

target_include_directories(MainApplication PUBLIC
   include/
   public/
)

target_link_libraries(MainApplication
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   GUIController
   ${DIALOGS_LIB}
   ${DRIVER_VARIANT}
)

add_library(MainApplicationIf INTERFACE)
target_include_directories(MainApplicationIf INTERFACE
   public/
)
target_link_libraries(MainApplicationIf INTERFACE
   GUIControllerIf
)


if (UNIT_TESTS)
   add_subdirectory(test)
endif()
