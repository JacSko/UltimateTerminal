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
        NativeDrivers
)
add_test(NAME SocketDriverIntegrationTests COMMAND SocketDriverIntegrationTests)

add_executable(QtSocketDriverIntegrationTests
            QtSocketDriverIntegrationTests.cpp
            ../../source/QtSocketServer.cpp
            ../../source/QtClientHandler.cpp
            ../../source/QtSocketClient.cpp
)

target_include_directories(QtSocketDriverIntegrationTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(QtSocketDriverIntegrationTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        QtDrivers
        Qt5::Network
)
add_test(NAME QtSocketDriverIntegrationTests COMMAND QtSocketDriverIntegrationTests)