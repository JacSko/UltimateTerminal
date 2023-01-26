add_library(GUIController STATIC 	   
       source/GUIController.cpp
       public/GUIController.h)

target_include_directories(GUIController PUBLIC 
   include/
   public/
)

target_link_libraries(GUIController
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   DriversIf
   Dialogs
   MainApplicationIf
)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()