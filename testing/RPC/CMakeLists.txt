add_library(RPCClient source/RPCClient.cpp)
         
target_include_directories(RPCClient PUBLIC public/ PRIVATE include/)
target_link_libraries(RPCClient PUBLIC 
                      TestLogger
                      UtilitiesIf
                      NativeDrivers)

add_library(RPCServer source/RPCServer.cpp)

target_include_directories(RPCServer PUBLIC public/ PRIVATE include/)
target_link_libraries(RPCServer PUBLIC 
                      Logger
                      UtilitiesIf
                      NativeDrivers)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()
