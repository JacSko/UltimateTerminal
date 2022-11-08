#pragma once
#include <gmock/gmock.h>
#include <string>
#include <cstdarg>

#define SIGNAL(arg) #arg
#define SLOT(arg) #arg
#define slots
#define signals public
#define emit
#define QT_BEGIN_NAMESPACE
#define QT_END_NAMESPACE

class QWidget;
class QObject;


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
   bool isEmpty() const
   {
      return std::string::empty();
   }
   static QString fromUtf8(const char* data)
   {
      return QString(data);
   }
   QString asprintf(const char* fmt, ...)
   {
      int idx = 0;
      char buffer [1024];
      va_list va;
      va_start(va, fmt);
      idx += vsprintf(buffer + idx, fmt, va);
      va_end(va);
      buffer[idx] = 0x00;
      return QString(buffer);
   }
   void chop(int count)
   {
      if (count >= size())
      {
         std::string("");
      }
      else
      {
         erase(size() - count, count);
      }
   }

};

struct QtCoreMock
{
   MOCK_METHOD4(QObject_connect, void(QObject*, std::string, QObject*, std::string));
   MOCK_METHOD1(QObject_objectName, QString(QObject*));
   MOCK_METHOD2(QObject_setObjectName, void(QObject*, const QString&));
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
class QApplication
{
public:
   static QString translate(const char * context,
                            const char * key,
                            const char * disambiguation = nullptr,
                            int n = -1){return QString(key);};
};
class QCoreApplication : public QApplication
{
};


class QMetaObject
{
public:
   static void connectSlotsByName(QObject *o){};
};

class QObject
{
public:
   QObject(){}
   QObject(QWidget*){}
   void connect(QObject* source, const char* signal_name, QObject* dest, const char* slot_name);
   QString objectName();
   void setObjectName(const QString&);
};
