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

#########################################
#  File paths configurations
#########################################
set(CONFIG_FILE ".system_config.json") 
set(LOGS_FILE ".logs.log") 
if (UNIX)
   message(STATUS "Building for UNIX")
   set(PERSISTENCE_FILE "$ENV{HOME}/.Ultimate_terminal_persistence.bin")
   set(SETTINGS_PERSISTENCE_FILE "$ENV{HOME}/.Ultimate_terminal_settings_persistence.bin")
   set(DRIVER_VARIANT NativeDrivers)
else()
   message(STATUS "Building for WINDOWS")
   cmake_path(SET PERSISTENCE_FILE "$ENV{LOCALAPPDATA}/.Ultimate_terminal_persistence.bin")
   cmake_path(SET SETTINGS_PERSISTENCE_FILE "$ENV{LOCALAPPDATA}/.Ultimate_terminal_settings_persistence.bin")
   set(DRIVER_VARIANT QtDrivers)
endif()
#########################################
#  Global project definitions
#########################################
add_definitions(-D_UNICODE
			    -DUNICODE
				-DCONFIG_FILE="${CONFIG_FILE}"
				-DLOGS_FILE="${LOGS_FILE}"
                -DAPPLICATION_VERSION={"${PROJECT_VERSION}"} 
                -DPERSISTENCE_PATH="${PERSISTENCE_FILE}" 
                -DSETTINGS_PERSISTENCE_PATH="${SETTINGS_PERSISTENCE_FILE}"
                -DRUNTIME_OUTPUT_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

#########################################
#  QT related settings
#########################################
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS SerialPort REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

if(UNIT_TESTS)
   #########################################
   #  Unit tests executable
   #########################################
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-exceptions -fPIC -O0")
   enable_testing()
   set(UNIT_TESTS ON)
   add_definitions(-DUNIT_TESTS)
   if(UT_NO_DEBUG_OUTPUT)
   add_definitions(-DUT_NO_DEBUG_OUTPUT)
   endif()
   add_subdirectory(ext_libs/googletest)
else()
#########################################
#  Main executable
#########################################
   add_executable(UltimateTerminal
                  sw/main.cpp)
   target_link_libraries(UltimateTerminal
                  MainApplication)
   if (NOT UNIX)
      target_link_libraries(UltimateTerminal shlwapi)
   else()
      target_link_libraries(UltimateTerminal pthread)
   endif()
   # copying-out system config file to bin
   add_custom_command(TARGET UltimateTerminal POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/config/system_config.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/.system_config.json
                      COMMENT "Copying setitngs file")
   if (SIMULATION)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-exceptions -fPIC -O0")
      enable_testing()
      add_subdirectory(ext_libs/googletest)
      add_subdirectory(testing)
      add_subdirectory(tools)
      add_definitions(-DSIMULATION) 
   endif()

endif()

#########################################
#  Include project directories
#########################################
add_subdirectory(ext_libs/json_parser)
add_subdirectory(sw/GUIController)
add_subdirectory(sw/MainApplication)
add_subdirectory(sw/Utilities)
add_subdirectory(sw/Drivers)
add_subdirectory(sw/Settings)
add_subdirectory(sw/Logger)
add_subdirectory(config)
add_subdirectory(sw/Dialogs)

#########################################
#  Print project summary
#########################################
function(printVariable name type)
   set(value ${ARGN})
   if(type STREQUAL "BOOL")
      if(value)
         set(value "ON")
      else()
         set(value "OFF")
      endif()
   endif()
   message(STATUS "${name} : ${value}")
endfunction()


message(STATUS "Project configuration summary:")
printVariable("Version" STRING ${PROJECT_VERSION})
printVariable("Target OS UNIX" BOOL ${UNIX})
printVariable("Target OS WIN32" BOOL ${WIN32})
printVariable("Unit Tests" BOOL ${UNIT_TESTS})
printVariable("Simulation Testing" BOOL ${SIMULATION})
printVariable("Logger disabled" BOOL ${UT_NO_DEBUG_OUTPUT})
printVariable("Config file path" STRING ${CONFIG_FILE})
printVariable("Persistence path" STRING ${PERSISTENCE_FILE})
printVariable("Settings persistence path" STRING ${SETTINGS_PERSISTENCE_FILE})
printVariable("CXX flags" STRING ${CMAKE_CXX_FLAGS})
printVariable("C flags" STRING ${CMAKE_C_FLAGS})



