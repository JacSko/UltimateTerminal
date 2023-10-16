add_executable(UserButtonDialogTests
            UserButtonDialogTests.cpp
            ../../source/UserButtonDialog.cpp
)

target_include_directories(UserButtonDialogTests PUBLIC
        ../../include
        ../../public
        ../mock/
)
target_link_libraries(UserButtonDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        QtMocks
        GUIControllerMock
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
        GUIControllerMock
)
add_test(NAME PortSettingDialogTests COMMAND PortSettingDialogTests)

#####################################################################################

add_executable(TraceFilterSettingDialogTests
            TraceFilterSettingDialogTests.cpp
            ../../source/TraceFilterSettingDialog.cpp
)

target_include_directories(TraceFilterSettingDialogTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(TraceFilterSettingDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        SettingsHandler
        Utilities
        QtMocks
        DriversMocks
        GUIControllerMock
)
add_test(NAME TraceFilterSettingDialogTests COMMAND TraceFilterSettingDialogTests)

#####################################################################################

add_executable(ApplicationSettingsDialogTests
            ApplicationSettingsDialogTests.cpp
            ../../source/ApplicationSettingsDialog.cpp
)
target_compile_definitions(ApplicationSettingsDialogTests PRIVATE "-DGIT_COMMIT_HASH=\"\"")
target_include_directories(ApplicationSettingsDialogTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(ApplicationSettingsDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        SettingsHandler
        Utilities
        QtMocks
        DriversMocks
        DialogsMocks
        MainApplicationIf
        MainApplicationMocks
        UtilitiesMocks
        GUIControllerMock
)
add_test(NAME ApplicationSettingsDialogTests COMMAND ApplicationSettingsDialogTests)

#####################################################################################

add_executable(TabNameDialogTests
            TabNameDialogTests.cpp
            ../../source/TabNameDialog.cpp
)

target_include_directories(TabNameDialogTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(TabNameDialogTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        QtMocks
        GUIControllerMock
)
add_test(NAME TabNameDialogTests COMMAND TabNameDialogTests)
