add_executable(RPCIntegrationTests
            RPCIntegrationTests.cpp
)

target_include_directories(RPCIntegrationTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(RPCIntegrationTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        Drivers
        RPC
        RPC_UT_API
)
add_test(NAME RPCIntegrationTests COMMAND RPCIntegrationTests)