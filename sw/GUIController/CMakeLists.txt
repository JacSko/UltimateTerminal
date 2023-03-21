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
   MainApplicationIf
)

if (SIMULATION)
target_link_libraries(GUIController
   GUITestServer
)
endif()

add_library(GUIControllerIf INTERFACE)
target_include_directories(GUIControllerIf INTERFACE 
   public/
)


if (UNIT_TESTS)
   add_subdirectory(test)
endif()