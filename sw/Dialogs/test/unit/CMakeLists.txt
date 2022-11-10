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
        DriversMocks
)
add_test(NAME PortSettingDialogTests COMMAND PortSettingDialogTests)