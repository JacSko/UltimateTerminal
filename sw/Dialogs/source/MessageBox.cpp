#include "MessageBox.h"

#include <QtWidgets/QMessageBox>


namespace Dialogs
{

QMessageBox::Icon translateIconType(MessageBox::Icon icon)
{
   switch(icon)
   {
      case MessageBox::Icon::Information:
      {
         return QMessageBox::Information;
      }
      case MessageBox::Icon::Warning:
      {
         return QMessageBox::Icon::Warning;
      }
      default:
      {
         return QMessageBox::Icon::Critical;
      }
   }
}


void MessageBox::show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette)
{
   QMessageBox messageBox;
   messageBox.setText(QString(text.c_str()));
   messageBox.setWindowTitle(QString(window_title.c_str()));
   messageBox.setIcon(translateIconType(icon));
   messageBox.setPalette(palette);
   messageBox.exec();
}

}
