#Generate header files
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/Settings_config.h
   COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/settings_header_gen.py ${CMAKE_CURRENT_SOURCE_DIR}/system_config.json ${CMAKE_CURRENT_SOURCE_DIR}/generated/Settings_config.h
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/settings_header_gen.py ${CMAKE_CURRENT_SOURCE_DIR}/system_config.json
)
add_custom_target(SettingsGeneration
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/system_config.json ${CMAKE_CURRENT_SOURCE_DIR}/generated/Settings_config.h
)

add_library(GeneratedSettings INTERFACE)
add_dependencies(GeneratedSettings SettingsGeneration)    
target_include_directories(GeneratedSettings INTERFACE generated/)