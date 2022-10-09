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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_definitions(-DCONFIG_FILE_PATH={"${CMAKE_SOURCE_DIR}/config/system_config.json"})

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

if (NOT UNIT_TESTS)
add_executable(UltimateTerminal
   sw/main.cpp
)
target_link_libraries(UltimateTerminal
   pthread
   MainWindow
)

else()
   enable_testing()
   set(UNIT_TESTS ON)
   add_definitions(-DUNIT_TESTS)
   add_subdirectory(ext_libs/googletest)
endif()

add_subdirectory(ext_libs/json_parser)
add_subdirectory(sw/MainWindow)
add_subdirectory(sw/Utilities)
add_subdirectory(sw/Drivers)
add_subdirectory(sw/Settings)
add_subdirectory(sw/Logger)
add_subdirectory(config)