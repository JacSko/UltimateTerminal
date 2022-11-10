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
        DialogsMocks
)
add_test(NAME UserButtonHandlerTests COMMAND UserButtonHandlerTests)

#####################################################################################

add_executable(TraceFilterHandlerTests
            TraceFilterHandlerTests.cpp
            ../../source/TraceFilterHandler.cpp
)

target_include_directories(TraceFilterHandlerTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(TraceFilterHandlerTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        UtilitiesMocks
        QtMocks
        DialogsMocks
)
add_test(NAME TraceFilterHandlerTests COMMAND TraceFilterHandlerTests)

#####################################################################################

add_executable(PortHandlerTests
            PortHandlerTests.cpp
            ../../source/PortHandler.cpp
)

target_include_directories(PortHandlerTests PUBLIC
        ../../include
        ../../public
        ../mock
)
target_link_libraries(PortHandlerTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        UtilitiesMocks
        QtMocks
        DriversMocks
        DialogsMocks
)
add_test(NAME PortHandlerTests COMMAND PortHandlerTests)

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
)
add_test(NAME MainApplicationTests COMMAND MainApplicationTests)
