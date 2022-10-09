#include "MainApplication.h"
#include "QtWidgets/QApplication"


int main(int argc, char *argv[])
{

   QApplication a(argc, argv);
   MainApplication app;
//   w.setWindowState(Qt::WindowFullScreen);
   app.show();

   return a.exec();
}
