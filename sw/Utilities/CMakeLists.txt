add_library(Persistence source/Persistence.cpp)
target_include_directories(Persistence PUBLIC public/ PRIVATE include/)
target_link_libraries(Persistence Logger nlohmann_json::nlohmann_json)

add_library(Utilities
            source/TimersImpl.cpp
            source/Persistence.cpp)
target_include_directories(Utilities PUBLIC public/ PRIVATE include/)
target_link_libraries(Utilities PRIVATE Persistence Logger ThreadWorker)

add_library(UtilitiesIf INTERFACE)
target_include_directories(UtilitiesIf INTERFACE public/)

add_library(ThreadWorker
            source/ThreadWorker.cpp)
target_include_directories(ThreadWorker PUBLIC public/ PRIVATE include/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()