add_library(GUIControllerSimulation STATIC 	   
       source/GUIController.cpp
       public/GUIController.h)

target_include_directories(GUIControllerSimulation PUBLIC 
   include/
   public/
)

target_link_libraries(GUIControllerSimulation
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   Dialogs
   MainApplicationIf
   RPC
   TestFramework
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()