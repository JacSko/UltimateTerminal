add_executable(GUIControllerTests
            GUIControllerTests.cpp
            ../../source/GUIController.cpp
)

target_include_directories(GUIControllerTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(GUIControllerTests PUBLIC
        gtest_main
        gmock_main
        Logger
        Utilities
        DialogsMocks
        QtMocks
        MainApplicationIf
        SettingsHandler
)
add_test(NAME GUIControllerTests COMMAND GUIControllerTests)