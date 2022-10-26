#pragma once

#include <string>

#define SIGNAL(arg) 0
#define SLOT(arg) 0

class QWidget;

namespace Qt
{

enum WindowModality {
    NonModal,
    WindowModal,
    ApplicationModal
};

enum Orientation {
    Horizontal = 0x1,
    Vertical = 0x2
};

}

class QObject
{
public:
   void connect(QWidget*, int, QWidget*, int){};
};

class QString : public std::string
{
public:
   QString(const char* str):
   std::string(str)
   {
   };
   std::string toStdString() const
   {
      return *this;
   }
};
