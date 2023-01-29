add_library(TestFramework STATIC
			source/TestFramework.cpp
)
target_include_directories(TestFramework PUBLIC 
   public/
)
target_link_libraries(TestFramework Utilities NativeDrivers Logger RPC SettingsHandler DialogsSimulation)
