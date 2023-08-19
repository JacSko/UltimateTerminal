add_library(TestFramework STATIC
			source/TestFramework.cpp
)
target_include_directories(TestFramework PUBLIC 
   public/
)
target_link_libraries(TestFramework UtilitiesIf DialogsSimulation NativeDrivers RPC DialogsIf Qt5::Widgets gtest)
