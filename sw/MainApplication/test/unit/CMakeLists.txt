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
        DialogsMocks
)
add_test(NAME ButtonCommandExecutorTests COMMAND ButtonCommandExecutorTests)

#####################################################################################

add_executable(UserButtonTests
            UserButtonTests.cpp
            ../../source/UserButton.cpp
)

target_include_directories(UserButtonTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(UserButtonTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        SettingsHandler
        UtilitiesMocks
        QtMocks
        DialogsMocks
        GUIControllerMock
)
add_test(NAME UserButtonTests COMMAND UserButtonTests)

#####################################################################################

add_executable(TraceFilterTests
            TraceFilterTests.cpp
            ../../source/TraceFilter.cpp
)

target_include_directories(TraceFilterTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(TraceFilterTests PUBLIC
        gtest_main
        gmock_main
        Logger
        SettingsHandler
        Utilities
        UtilitiesMocks
        QtMocks
        DialogsMocks
        GUIControllerMock
)
add_test(NAME TraceFilterTests COMMAND TraceFilterTests)

#####################################################################################

add_executable(PortTests
            PortTests.cpp
            ../../source/Port.cpp
)

target_include_directories(PortTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(PortTests PUBLIC
        gtest_main
        gmock_main
        Logger
        SettingsHandler
        Utilities
        UtilitiesMocks
        QtMocks
        DriversMocks
        DialogsMocks
        GUIControllerMock
)
add_test(NAME PortTests COMMAND PortTests)

#####################################################################################

add_executable(MainApplicationTests
            MainApplicationTests.cpp
            ../../source/MainApplication.cpp
)

target_include_directories(MainApplicationTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(MainApplicationTests PUBLIC
        gtest_main
        gmock_main
        Logger
        UtilitiesIf
        UtilitiesMocks
        QtMocks
        SettingsHandler
        ThreadWorker
        DialogsMocks
        GUIControllerMock
)
add_test(NAME MainApplicationTests COMMAND MainApplicationTests)

#####################################################################################

add_executable(UserButtonsTabTests
            UserButtonsTabTests.cpp
            ../../source/UserButtonsTab.cpp
)

target_include_directories(UserButtonsTabTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(UserButtonsTabTests PUBLIC
        gtest_main
        gmock_main
        Logger
        UtilitiesMocks
        SettingsHandler
        MainApplicationMocks
)
add_test(NAME UserButtonsTabTests COMMAND UserButtonsTabTests)
