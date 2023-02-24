#pragma once
#include <QtGui/QPalette>
#include <string>


namespace Dialogs
{

class MessageDialog
{
public:
   enum class Icon
   {
      Information,
      Warning,
      Critical,
   };

   static void show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette = {});
};

}
