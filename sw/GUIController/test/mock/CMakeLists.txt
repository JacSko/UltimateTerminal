add_library(GUIControllerMock INTERFACE)
            
target_include_directories(GUIControllerMock INTERFACE
                           ./ 
                           ../../public)
target_link_libraries(GUIControllerMock INTERFACE 
                      Logger
                      gmock
                      )