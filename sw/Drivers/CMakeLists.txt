if(DRIVER_VARIANT EQUAL DRIVER_VARIANT_QT)

set(DRIVER_SOURCES
   source/QtSerialDriver.cpp
   source/QtSocketClient.cpp
   source/QtSocketServer.cpp
   source/QtClientHandler.cpp
   source/ISocketDriverFactory.cpp
)
set(DRIVER_DEPENDENCIES Qt6::SerialPort Qt6::Network)

elseif(DRIVER_VARIANT EQUAL DRIVER_VARIANT_NATIVE)

set(DRIVER_SOURCES 
   source/SocketClient.cpp
   source/SocketServer.cpp
   source/ClientHandler.cpp
   source/SerialDriver.cpp
   source/ISocketDriverFactory.cpp
)
set(DRIVER_DEPENDENCIES )

else()

message(ERROR "Please provide drivers variant!")

endif()

add_library(Drivers ${DRIVER_SOURCES})
target_include_directories(Drivers PUBLIC public/ PRIVATE include/)
target_link_libraries(Drivers PRIVATE Logger Utilities ${DRIVER_DEPENDENCIES})

add_library(DriversIf INTERFACE)
target_include_directories(DriversIf INTERFACE public/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

