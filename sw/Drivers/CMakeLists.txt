add_library(Drivers
   source/SocketClient.cpp
   source/SocketServer.cpp
   source/ISocketDriverFactory.cpp
)
target_include_directories(Drivers PUBLIC public/ PRIVATE include/)
target_link_libraries(Drivers PRIVATE Logger Utilities)

add_library(DriversIf INTERFACE)
target_include_directories(DriversIf INTERFACE public/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()