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

add_executable(UserButtonDialogTests
            UserButtonDialogTests.cpp
            ../../source/UserButtonDialog.cpp
)

target_include_directories(UserButtonDialogTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(UserButtonDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        QtMocks
)
add_test(NAME UserButtonDialogTests COMMAND UserButtonDialogTests)