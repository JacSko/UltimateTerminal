########################################
cmake_minimum_required(VERSION 3.1.0)
########################################

project(UltimateTerminal VERSION 1.0.0 LANGUAGES CXX)

########################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#########################################
#  QT related settings
#########################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/sw/gui)

#########################################
#  Main executable
#########################################
add_executable(UltimateTerminal
   sw/main.cpp
)
target_link_libraries(UltimateTerminal
   pthread
   MainWindow
)

add_subdirectory(sw/MainWindow)