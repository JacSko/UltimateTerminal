add_library(RPC
            source/RPCServer.cpp
            source/RPCClient.cpp)
         
target_include_directories(RPC PUBLIC public/ PRIVATE include/)
target_link_libraries(RPC PUBLIC 
                      Logger 
                      Utilities
                      NativeDrivers)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()
