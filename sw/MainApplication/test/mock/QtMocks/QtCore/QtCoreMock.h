#pragma once
#include <gmock/gmock.h>
#include <string>

#define SIGNAL(arg) #arg
#define SLOT(arg) #arg
#define slots
#define signals public
#define emit

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

enum ContextMenuPolicy {
    NoContextMenu,
    DefaultContextMenu,
    ActionsContextMenu,
    CustomContextMenu,
    PreventContextMenu
};

enum AlignmentFlag {
    AlignLeft = 0x0001,
    AlignLeading = AlignLeft,
    AlignRight = 0x0002,
    AlignTrailing = AlignRight,
    AlignHCenter = 0x0004,
    AlignJustify = 0x0008,
    AlignAbsolute = 0x0010,
    AlignHorizontal_Mask = AlignLeft | AlignRight | AlignHCenter | AlignJustify | AlignAbsolute,

    AlignTop = 0x0020,
    AlignBottom = 0x0040,
    AlignVCenter = 0x0080,
    AlignBaseline = 0x0100,
    AlignVertical_Mask = AlignTop | AlignBottom | AlignVCenter | AlignBaseline,

    AlignCenter = AlignVCenter | AlignHCenter
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
