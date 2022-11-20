#pragma once
#include <gmock/gmock.h>
#include "IThemeController.h"


struct IThemeControllerMock
{
   MOCK_METHOD1(reloadTheme, void(Ui_MainWindow::Theme));
   MOCK_METHOD1(themeToName, std::string(Ui_MainWindow::Theme));
   MOCK_METHOD1(reloadTheme, Ui_MainWindow::Theme(const std::string&));
};
