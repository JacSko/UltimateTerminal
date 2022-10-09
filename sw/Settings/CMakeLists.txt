set(SETTINGS_SOURCES)

if(NOT UNIT_TESTS)
set (SETTINGS_SOURCES ${SETTINGS_SOURCES} source/SettingsHandler.cpp)
else()
set (SETTINGS_SOURCES ${SETTINGS_SOURCES} test/stubs/SettingsHandlerStub.cpp)
endif()
   
      
add_library(SettingsHandler ${SETTINGS_SOURCES})
target_include_directories(SettingsHandler PUBLIC public/ PRIVATE include/)
target_link_libraries(SettingsHandler PUBLIC nlohmann_json::nlohmann_json GeneratedSettings PRIVATE Logger Utilities)
