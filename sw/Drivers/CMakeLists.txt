if (UNIX)
add_library(NativeDrivers source/SocketClient.cpp
                          source/SocketServer.cpp
                          source/ClientHandler.cpp
                          source/SerialDriver.cpp
                          source/ProcessDriver.cpp)
target_include_directories(NativeDrivers PUBLIC public/ PRIVATE include/)
target_link_libraries(NativeDrivers PRIVATE
                      UtilitiesIf
                      ThreadWorker
                      ${LOGGER_VARIANT})

endif()

add_library(QtDrivers source/QtSerialDriver.cpp
                      source/QtSocketClient.cpp
                      source/QtSocketServer.cpp
                      source/QtClientHandler.cpp)
target_include_directories(QtDrivers PUBLIC public/ PRIVATE include/)
target_link_libraries(QtDrivers PRIVATE
                      UtilitiesIf
                      ThreadWorker
                      Qt5::SerialPort
                      Qt5::Network)

add_library(DriversIf INTERFACE)
target_include_directories(DriversIf INTERFACE public/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

