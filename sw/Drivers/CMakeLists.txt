if(DRIVER_VARIANT EQUAL DRIVER_VARIANT_QT)

set(DRIVER_SOURCES
   source/QtSerialDriver.cpp
# TEMPORARY ADD NATIVE DRIVERS   
   source/SocketClient.cpp
   source/SocketServer.cpp
   source/ISocketDriverFactory.cpp
   source/ClientHandler.cpp
)
set(DRIVER_DEPENDENCIES Qt5::SerialPort)

else()

set(DRIVER_SOURCES 
   source/SocketClient.cpp
   source/SocketServer.cpp
   source/ISocketDriverFactory.cpp
   source/ClientHandler.cpp
   source/SerialDriver.cpp)
set(DRIVER_DEPENDENCIES )

endif()

add_library(Drivers ${DRIVER_SOURCES})
target_include_directories(Drivers PUBLIC public/ PRIVATE include/)
target_link_libraries(Drivers PRIVATE Logger Utilities ${DRIVER_DEPENDENCIES})

add_library(DriversIf INTERFACE)
target_include_directories(DriversIf INTERFACE public/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

