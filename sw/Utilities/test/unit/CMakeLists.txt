add_executable(TimersImplTests
            TimersImplTests.cpp
            ../../source/TimersImpl.cpp
)

target_include_directories(TimersImplTests PUBLIC
        ../../include
        ../../public
)
target_link_libraries(TimersImplTests PUBLIC
        gtest_main
        gmock_main
        Logger
        ThreadWorker
)
add_test(NAME TimersImplTests COMMAND TimersImplTests)