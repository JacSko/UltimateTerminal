#pragma once
#include <gmock/gmock.h>
#include <string>

#define SIGNAL(arg) #arg
#define SLOT(arg) #arg
#define slots


class QWidget;
class QObject;

struct QtCoreMock
{
   MOCK_METHOD4(QObject_connect, void(QObject*, std::string, QObject*, std::string));
};

void QtCoreMock_init();
void QtCoreMock_deinit();
QtCoreMock* QtCoreMock_get();

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

enum GlobalColor {
    color0,
    color1,
    black,
    white,
    darkGray,
    gray,
    lightGray,
    red,
    green,
    blue,
    cyan,
    magenta,
    yellow,
    darkRed,
    darkGreen,
    darkBlue,
    darkCyan,
    darkMagenta,
    darkYellow,
    transparent
};

}

class QObject
{
public:
   void connect(QObject* source, const char* signal_name, QObject* dest, const char* slot_name);
};

class QString : public std::string
{
public:
   QString():
   std::string()
   {
   };
   QString(const char* str):
   std::string(str)
   {
   };

   static QString number(int value)
   {
      return QString(std::to_string(value).c_str());
   }
   std::string toStdString() const
   {
      return *this;
   }
   unsigned int toUInt()
   {
      return std::stoi(*this);
}

};
