add_executable(SocketDataGenerator SocketDataGenerator.cpp)
target_link_libraries(SocketDataGenerator PRIVATE Drivers)

add_executable(SerialDataGenerator SerialDataGenerator.cpp)
target_link_libraries(SerialDataGenerator PRIVATE Drivers UtilitiesIf)

if (UNIX)
   target_link_libraries(SerialDataGenerator PRIVATE pthread)
   target_link_libraries(SocketDataGenerator PRIVATE pthread)
endif()