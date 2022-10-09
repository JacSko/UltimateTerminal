if (UNIT_TESTS)

add_subdirectory(unit)
add_library(UtilitiesMocks INTERFACE)
target_include_directories(UtilitiesMocks INTERFACE mocks/)
target_link_libraries(UtilitiesMocks INTERFACE UtilitiesIf)

endif()