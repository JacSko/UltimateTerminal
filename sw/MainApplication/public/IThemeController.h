#pragma once
#include <string>
#include "ui_MainWindow.h"

class IThemeController
{
public:
   virtual ~IThemeController(){};
   virtual void reloadTheme(Ui_MainWindow::Theme) = 0;
   virtual std::string themeToName(Ui_MainWindow::Theme) = 0;
   virtual Ui_MainWindow::Theme nameToTheme(const std::string& name) = 0;
};
