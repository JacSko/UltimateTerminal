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
