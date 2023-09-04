add_library(TestFramework STATIC
			source/TestFramework.cpp
)
target_include_directories(TestFramework PUBLIC 
   public/
)
target_link_libraries(TestFramework UtilitiesIf DialogsIf
                      NativeDrivers RPCClient Qt5::Widgets gtest)

add_library(TestFrameworkIf INTERFACE)
target_include_directories(TestFrameworkIf INTERFACE public/)