########################################
cmake_minimum_required(VERSION 3.1.0)
project(UltimateTerminal VERSION 1.0.0 LANGUAGES CXX)
########################################

#########################################
#  Global project specification
#########################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#########################################
#  File paths configurations
#########################################
set(CONFIG_FILE ".system_config.json") 
set(PERSISTENCE_FILE "$ENV{HOME}/.Ultimate_terminal_persistence.bin")

#########################################
#  Global project definitions
#########################################
add_definitions(-DCONFIG_FILE="${CONFIG_FILE}" -DAPPLICATION_VERSION={"${PROJECT_VERSION}"} -DPERSISTENCE_PATH="${PERSISTENCE_FILE}")

#########################################
#  QT related settings
#########################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/sw/gui)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

if (NOT UNIT_TESTS)
   #########################################
   #  Main executable
   #########################################
   add_executable(UltimateTerminal
      sw/main.cpp)
   target_link_libraries(UltimateTerminal
      pthread
      MainApplication)
   # copying-out system config file to bin
   add_custom_command(TARGET UltimateTerminal POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/config/system_config.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.system_config.json
                      COMMENT "Copying setitngs file")

else()
   #########################################
   #  Unit tests executable
   #########################################
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs -ftest-coverage -fno-exceptions -fPIC -O0")
   enable_testing()
   set(UNIT_TESTS ON)
   add_definitions(-DUNIT_TESTS)
   add_subdirectory(ext_libs/googletest)

endif()

#########################################
#  Include project directories
#########################################
add_subdirectory(ext_libs/json_parser)
add_subdirectory(sw/MainApplication)
add_subdirectory(sw/Utilities)
add_subdirectory(sw/Drivers)
add_subdirectory(sw/Settings)
add_subdirectory(sw/Logger)
add_subdirectory(config)
add_subdirectory(sw/Dialogs)