set(LOGGER_SOURCES)

if(NOT UNIT_TESTS)
set (LOGGER_SOURCES ${LOGGER_SOURCES} source/LoggerEngine.cpp)
else()
set (LOGGER_SOURCES ${LOGGER_SOURCES} test/stubs/LoggerEngineStub.cpp)
endif()
   
add_library(Logger ${LOGGER_SOURCES})
target_include_directories(Logger PUBLIC public/ include/)
target_link_libraries(Logger PRIVATE UtilitiesIf SettingsHandler PUBLIC ${DRIVER_VARIANT})
