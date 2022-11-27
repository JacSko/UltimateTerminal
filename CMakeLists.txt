########################################
cmake_minimum_required(VERSION 3.1.0)
project(UltimateTerminal VERSION 1.0.0 LANGUAGES CXX)
########################################

#########################################
#  Global project specification
#########################################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-function")
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(DRIVER_VARIANT_NATIVE 0)
set(DRIVER_VARIANT_QT     1)
set(DRIVER_VARIANT ${DRIVER_VARIANT_QT})

#########################################
#  File paths configurations
#########################################
set(CONFIG_FILE ".system_config.json") 
set(PERSISTENCE_FILE "$ENV{HOME}/.Ultimate_terminal_persistence.bin")
set(SETTINGS_PERSISTENCE_FILE "$ENV{HOME}/.Ultimate_terminal_settings_persistence.bin")
#########################################
#  Global project definitions
#########################################
add_definitions(-DCONFIG_FILE="${CONFIG_FILE}" 
                -DAPPLICATION_VERSION={"${PROJECT_VERSION}"} 
                -DPERSISTENCE_PATH="${PERSISTENCE_FILE}" 
                -DSETTINGS_PERSISTENCE_PATH="${SETTINGS_PERSISTENCE_FILE}")

#########################################
#  QT related settings
#########################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

if (NOT UNIT_TESTS)
   #########################################
   #  Main executable
   #########################################
   add_executable(UltimateTerminal
      sw/main.cpp)
   target_link_libraries(UltimateTerminal
      MainApplication)
   # copying-out system config file to bin
   add_custom_command(TARGET UltimateTerminal POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/config/system_config.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.system_config.json
                      COMMENT "Copying setitngs file")

else()
   #########################################
   #  Unit tests executable
   #########################################
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage -fno-exceptions -fPIC -O0")
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