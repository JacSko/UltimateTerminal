add_library(GUITestServer STATIC 	   
       source/GUITestServer.cpp
       public/GUITestServer.h)

target_include_directories(GUITestServer PUBLIC 
   public/
)

target_link_libraries(GUITestServer
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
       source/UserButtonDialog.cpp
       ../../sw/Dialogs/public/PortSettingDialog.h
       ../../sw/Dialogs/public/UserButtonDialog.h
       ../../sw/Dialogs/public/LoggingSettingDialog.h
       ../../sw/Dialogs/public/TraceFilterSettingDialog.h
       ../../sw/Dialogs/public/ApplicationSettingsDialog.h)

target_link_libraries(DialogsSimulation
   Logger
   DialogsIf
   GUITestServer
   MainApplicationIf
)
target_include_directories(DialogsSimulation PUBLIC 
   public/
)