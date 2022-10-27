#pragma once
#include <gmock/gmock.h>
#include <string>

#define SIGNAL(arg) #arg
#define SLOT(arg) #arg

class QWidget;


struct QtCoreMock
{
   MOCK_METHOD4(QObject_connect, void(QWidget*, std::string, QWidget*, std::string));
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

}

class QObject
{
public:
   void connect(QWidget*, const char*, QWidget*, const char*);
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
