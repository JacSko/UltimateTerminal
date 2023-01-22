message(STATUS "SIMULATION2")
add_library(TestFramework INTERFACE)
target_include_directories(TestFramework INTERFACE 
   public/
)
