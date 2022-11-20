#pragma once
#include <string>

#define APPLICATION_THEMES       \
      APPLICATION_THEME(DEFAULT) \
      APPLICATION_THEME(DARK)    \

class IThemeController
{
public:

#undef APPLICATION_THEME
#define APPLICATION_THEME(name) name,
enum class Theme
{
   APPLICATION_THEMES
   APPLICATION_THEMES_MAX
};
#undef APPLICATION_THEME


   virtual ~IThemeController(){};
   virtual void reloadTheme(Theme) = 0;
   virtual std::string themeToName(Theme) = 0;
   virtual Theme nameToTheme(const std::string& name) = 0;
};
