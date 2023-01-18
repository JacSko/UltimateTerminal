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

template<typename T>
using QVector = std::vector<T>;
using qint32 = uint32_t;
using qint8 = uint8_t;

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
   unsigned int toUInt(bool* conv)
   {
      *conv = true;
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
      if ((size_t)count >= size())
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

enum Modifier {
    CTRL,
    Key_F1 = 0x01000030,
    Key_F2 = 0x01000031,
    Key_F3 = 0x01000032,
    Key_F4 = 0x01000033,
    Key_F5 = 0x01000034,
    Key_F6 = 0x01000035,
    Key_F7 = 0x01000036,
    Key_F8 = 0x01000037,
    Key_F9 = 0x01000038,
    Key_F10 = 0x01000039,
    Key_F11 = 0x0100003a,
    Key_F12 = 0x0100003b,
    Key_0 = 0x30,
    Key_1 = 0x31,
    Key_2 = 0x32,
    Key_3 = 0x33,
    Key_4 = 0x34,
    Key_5 = 0x35,
    Key_6 = 0x36,
    Key_7 = 0x37,
    Key_8 = 0x38,
    Key_9 = 0x39,
    Key_M,
};

}
class QApplication
{
public:
   static QString translate(const char * key,
                            const char *,
                            const char * = nullptr,
                            int = -1){return QString(key);};
};
class QCoreApplication : public QApplication
{
};


class QMetaObject
{
public:
   static void connectSlotsByName(QObject *){};
};

class QObject
{
public:
   QObject(){}
   QObject(QWidget*){}
   void connect(QObject* source, const char* signal_name, QObject* dest, const char* slot_name);
   QString objectName(){return QString(name.c_str());}
   void setObjectName(const QString& new_name) {name = new_name.toStdString();};
private:
   std::string name;
};
