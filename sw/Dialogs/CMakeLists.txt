if(NOT SIMULATION)

execute_process(
   COMMAND git describe --always --dirty
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   OUTPUT_VARIABLE GIT_COMMIT_HASH
   OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(ApplicationSettingsDialog
   source/ApplicationSettingsDialog.cpp
   public/ApplicationSettingsDialog.h)
target_compile_definitions(ApplicationSettingsDialog PRIVATE "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
target_include_directories(ApplicationSettingsDialog PUBLIC
   public/)

target_link_libraries(ApplicationSettingsDialog
   Qt5::Widgets
   MainApplicationIf
   Utilities
   DriversIf
   SettingsHandler
   Logger)


add_library(Dialogs STATIC
   source/PortSettingDialog.cpp
   source/UserButtonDialog.cpp
   source/LoggingSettingDialog.cpp
   source/TraceFilterSettingDialog.cpp
   source/MessageDialog.cpp
   source/TabNameDialog.cpp
   public/PortSettingDialog.h
   public/UserButtonDialog.h
   public/LoggingSettingDialog.h
   public/TraceFilterSettingDialog.h
   public/TabNameDialog.h
)

target_include_directories(Dialogs PUBLIC
   public/
)

target_link_libraries(Dialogs
   Qt5::Widgets
   SettingsHandler
   Logger
   Utilities
   DriversIf
   MainApplicationIf
   GUIController
   ApplicationSettingsDialog
)
endif()

if (UNIT_TESTS)
   add_subdirectory(test)
endif()

add_library(DialogsIf INTERFACE)
target_include_directories(DialogsIf INTERFACE
   public/
)
