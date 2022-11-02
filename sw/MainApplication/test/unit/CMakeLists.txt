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

#####################################################################################

add_executable(PortSettingDialogTests
            PortSettingDialogTests.cpp
            ../../source/PortSettingDialog.cpp
)

target_include_directories(PortSettingDialogTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(PortSettingDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        SettingsHandler
        Utilities
        QtMocks
)
add_test(NAME PortSettingDialogTests COMMAND PortSettingDialogTests)

#####################################################################################

add_executable(UserButtonHandlerTests
            UserButtonHandlerTests.cpp
            ../../source/UserButtonHandler.cpp
)

target_include_directories(UserButtonHandlerTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(UserButtonHandlerTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        UtilitiesMocks
        QtMocks
)
add_test(NAME UserButtonHandlerTests COMMAND UserButtonHandlerTests)
