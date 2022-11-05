#include "QtCoreMock.h"
#include "Logger.h"

QtCoreMock* g_core_mock;

void QtCoreMock_init()
{
   if (!g_core_mock)
   {
      g_core_mock = new QtCoreMock();
   }
}

void QtCoreMock_deinit()
{
   if (g_core_mock)
   {
      delete g_core_mock;
      g_core_mock = nullptr;
   }
}

QtCoreMock* QtCoreMock_get()
{
   UT_Assert(g_core_mock && "Create core mock first!");
   return g_core_mock;
}


void QObject::connect(QObject* source, const char* signal_name, QObject* dest, const char* slot_name)
{
   g_core_mock->QObject_connect(source, std::string(signal_name), dest, std::string(slot_name));
}
QString QObject::objectName()
{
   return g_core_mock->QObject_objectName(this);
}
void QObject::setObjectName(const QString& name)
{
   g_core_mock->QObject_setObjectName(this, name);
}
