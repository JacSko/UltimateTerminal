add_library(RPC
            source/RPCServer.cpp
            source/RPCClient.cpp
            source/ClientHandler.cpp
            source/SocketClient.cpp
            source/SocketServer.cpp)
         
target_include_directories(RPC PUBLIC public/ PRIVATE include/)
target_link_libraries(RPC PUBLIC Logger Utilities)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()
