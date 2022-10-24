add_executable(ButtonCommandExecutorTests
            ButtonCommandExecutorTests.cpp
)

target_include_directories(ButtonCommandExecutorTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(ButtonCommandExecutorTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
)
add_test(NAME ButtonCommandExecutorTests COMMAND ButtonCommandExecutorTests)
