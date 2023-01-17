#include "MainApplication.h"
#include "QtWidgets/QApplication"


int main(int argc, char *argv[])
{

   QApplication a(argc, argv);
   MainApplication app;
   return a.exec();
}
