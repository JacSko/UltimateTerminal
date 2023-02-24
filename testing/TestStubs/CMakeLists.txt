add_library(GUIControllerSimulation STATIC 	   
       source/GUIController.cpp
       public/GUIController.h)

target_include_directories(GUIControllerSimulation PUBLIC 
   public/
)

target_link_libraries(GUIControllerSimulation
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   DialogsSimulation
   MainApplicationIf
   RPC
   TestFramework
)

add_library(DialogsSimulation STATIC      
       source/ApplicationSettingsDialog.cpp
       source/LoggingSettingDialog.cpp
       source/MessageDialog.cpp
       source/PortSettingDialog.cpp
       source/TraceFilterSettingDialog.cpp
       source/UserButtonDialog.cpp)

target_link_libraries(DialogsSimulation
   Logger
   GUIControllerSimulation
)
target_include_directories(DialogsSimulation PUBLIC 
   public/
)