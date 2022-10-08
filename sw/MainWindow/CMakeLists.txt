add_library(MainWindow STATIC 
   source/MainWindow.cpp
   ../gui/MainWindow.ui
   include/MainWindow.h
)

target_include_directories(MainWindow PUBLIC 
   include/
)

target_link_libraries(MainWindow
   Qt5::Widgets
)