add_executable(TestExecutable
               Test.cpp
)

target_link_libraries(TestExecutable PUBLIC
        gtest_main
        gmock_main
        TestFramework
)
add_test(NAME TestExecutable COMMAND TestExecutable)


add_executable(SerialPortReadWrite SerialPortReadWrite.cpp)
target_link_libraries(SerialPortReadWrite PUBLIC gtest_main TestFramework)
add_test(NAME SerialPortReadWrite COMMAND SerialPortReadWrite)