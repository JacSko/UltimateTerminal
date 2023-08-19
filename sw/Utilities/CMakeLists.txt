add_library(Utilities
            source/TimersImpl.cpp
            source/PersistenceHandler.cpp)
target_include_directories(Utilities PUBLIC public/ PRIVATE include/)
target_link_libraries(Utilities PRIVATE Logger ThreadWorker)

add_library(UtilitiesIf INTERFACE)
target_include_directories(UtilitiesIf INTERFACE public/)

add_library(ThreadWorker
            source/ThreadWorker.cpp)
target_include_directories(ThreadWorker PUBLIC public/ PRIVATE include/)

if (UNIT_TESTS)
   add_subdirectory(test)
endif()