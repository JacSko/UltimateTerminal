add_library(QtMocks
            QtMocks/QtCore/QtCoreMock.cpp
            QtMocks/QtWidgets/QtWidgetsMock.cpp)
            
target_include_directories(QtMocks PUBLIC
                           QtMocks/
                           QtMocks/)

target_link_libraries(QtMocks PRIVATE 
                      Logger
                      gmock
                      )
                      
add_library(MainApplicationMocks INTERFACE)
            
target_include_directories(MainApplicationMocks INTERFACE
                           ./ )
target_link_libraries(MainApplicationMocks INTERFACE 
                      Logger
                      gmock
                      GUIControllerMock
                      DialogsMocks
                      QtMocks
                      )