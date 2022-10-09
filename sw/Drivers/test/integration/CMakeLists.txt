add_executable(SocketDriverIntegrationTests
            SocketDriverIntegrationTests.cpp
            ../../source/SocketServer.cpp
            ../../source/ClientHandler.cpp
            ../../source/SocketClient.cpp
)

target_include_directories(SocketDriverIntegrationTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(SocketDriverIntegrationTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
)
add_test(NAME SocketDriverIntegrationTests COMMAND SocketDriverIntegrationTests)