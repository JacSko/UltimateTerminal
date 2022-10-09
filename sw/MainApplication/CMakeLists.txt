add_library(MainApplication STATIC 
   source/MainApplication.cpp
   ../gui/MainWindow.ui
   include/MainApplication.h
)

target_include_directories(MainApplication PUBLIC 
   include/
)

target_link_libraries(MainApplication
   Qt5::Widgets
)