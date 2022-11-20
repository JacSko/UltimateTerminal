#pragma once
#include <gmock/gmock.h>
#include "IThemeController.h"


struct IThemeControllerMock : public IThemeController
{
   MOCK_METHOD1(reloadTheme, void(IThemeController::Theme));
   MOCK_METHOD1(themeToName, std::string(IThemeController::Theme));
   MOCK_METHOD1(nameToTheme, IThemeController::Theme(const std::string&));
};
