add_executable(SocketClientTests
            SocketClientTests.cpp
            ../../source/SocketClient.cpp
)

target_include_directories(SocketClientTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(SocketClientTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
)
add_test(NAME SocketClientTests COMMAND SocketClientTests)
