add_library(NativeDrivers source/SocketClient.cpp
                          source/SocketServer.cpp
                          source/ClientHandler.cpp
                          source/SerialDriver.cpp)
target_include_directories(NativeDrivers PUBLIC public/ PRIVATE include/)
target_link_libraries(NativeDrivers PRIVATE 
                      Logger 
                      Utilities)
	
add_library(QtDrivers source/QtSerialDriver.cpp
                      source/QtSocketClient.cpp
                      source/QtSocketServer.cpp
                      source/QtClientHandler.cpp)
target_include_directories(QtDrivers PUBLIC public/ PRIVATE include/)
target_link_libraries(QtDrivers PRIVATE 
                      Logger 
                      Utilities 
                      Qt5::SerialPort 
                      Qt5::Network)

add_library(DriversIf INTERFACE)
target_include_directories(DriversIf INTERFACE public/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

