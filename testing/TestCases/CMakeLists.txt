add_executable(TestExecutable
               Test.cpp
)

target_link_libraries(TestExecutable PUBLIC
        gtest_main
        gmock_main
        TestFramework
)
add_test(NAME TestExecutable COMMAND TestExecutable)