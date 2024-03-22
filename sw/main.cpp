#include "MainApplication.h"
#include "QtWidgets/QApplication"

#include <execinfo.h>
#include <signal.h>

int main(int argc, char *argv[])
{
   signal(SIGCHLD, SIG_IGN);
   QApplication a(argc, argv);
   MainApplication::MainApplication app;
   return a.exec();
}
