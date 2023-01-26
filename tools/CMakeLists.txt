add_executable(SocketDataGenerator SocketDataGenerator.cpp)
target_link_libraries(SocketDataGenerator PRIVATE NativeDrivers)

add_executable(SerialDataGenerator SerialDataGenerator.cpp)
target_link_libraries(SerialDataGenerator PRIVATE NativeDrivers UtilitiesIf)

if (UNIX)
   target_link_libraries(SerialDataGenerator PRIVATE pthread)
   target_link_libraries(SocketDataGenerator PRIVATE pthread)
endif()