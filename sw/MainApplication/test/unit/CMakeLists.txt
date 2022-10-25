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

#####################################################################################

add_executable(UserButtonHandlerTests
            UserButtonHandlerTests.cpp
            ../../source/UserButtonHandler.cpp
)

target_include_directories(UserButtonHandlerTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(UserButtonHandlerTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        QtMocks
)
add_test(NAME UserButtonHandlerTests COMMAND UserButtonHandlerTests)