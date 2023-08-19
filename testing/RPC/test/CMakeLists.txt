if (UNIT_TESTS)
   include_directories(include/)
   add_subdirectory(integration)
   
add_library(RPC_UT_API INTERFACE)
target_include_directories(RPC_UT_API INTERFACE test_api/)
endif()