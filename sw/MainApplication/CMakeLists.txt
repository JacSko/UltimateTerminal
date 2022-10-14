add_library(MainApplication STATIC 
   source/MainApplication.cpp
   source/PortSettingDialog.cpp
   source/PortHandler.cpp
   ../gui/MainWindow.ui
   include/MainApplication.h
   include/PortSettingDialog.h
   include/PortHandler.h
)

target_include_directories(MainApplication PUBLIC 
   include/
)

target_link_libraries(MainApplication
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
)