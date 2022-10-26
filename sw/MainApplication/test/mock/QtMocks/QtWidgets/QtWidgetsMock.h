#pragma once
#include <gmock/gmock.h>

#include <QtCore/QNamespace>
#include <QtCore/QString>

#define Q_OBJECT

struct QtWidgetsMock
{
   MOCK_METHOD0(QDialog_new, void*());
   MOCK_METHOD0(QDialog_exec, int());
   MOCK_METHOD1(QDialog_setWindowModality, void(Qt::WindowModality));

   MOCK_METHOD0(QFormLayout_new, void*());
   MOCK_METHOD1(QFormLayout_addRow, void(QWidget*));
   MOCK_METHOD1(QFormLayout_addWidget, void(QWidget*));

   MOCK_METHOD0(QLineEdit_new, void*());
   MOCK_METHOD1(QLineEdit_setText, void(const std::string&));
   MOCK_METHOD0(QLineEdit_text, QString());

   MOCK_METHOD0(QTextEdit_new, void*());
   MOCK_METHOD1(QTextEdit_setText, void(const std::string&));
   MOCK_METHOD0(QTextEdit_toPlainText, QString());

   MOCK_METHOD0(QDialogButtonBox_new, void*());

   MOCK_METHOD1(QWidget_setEnabled, void(bool));
   MOCK_METHOD1(QWidget_setDisabled, void(bool));
};

void QtWidgetsMock_init();
void QtWidgetsMock_deinit();
QtWidgetsMock* QtWidgetsMock_get();


class QWidget
{
public:
   void setEnabled(bool);
   void setDisabled(bool);
};

class QDialog : public QWidget
{
public:
   enum DialogCode { Rejected, Accepted };

   QDialog(){};
   QDialog(QWidget*&){};
   virtual ~QDialog(){};

   void* operator new(size_t size);
   void operator delete(void*){};
   void setWindowModality(Qt::WindowModality windowModality);
   virtual int exec();
};

class QDialogButtonBox : public QWidget
{
public:
   enum StandardButton
   {
      Ok                 = 0x00000400,
      Cancel             = 0x00400000,
   };
   QDialogButtonBox(){};
   QDialogButtonBox(int buttons, Qt::Orientation orientation, QWidget *parent = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
};

class QFormLayout : public QWidget
{
public:
   QFormLayout(){};
   QFormLayout(QDialog*&){}

   void* operator new(size_t);
   void operator delete(void*){};
   void addRow(QWidget *widget);
   void addWidget(QWidget *w);
};


class QLineEdit : public QWidget
{
public:
   QLineEdit(){};
   QLineEdit(QDialog*&){};

   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   QString text();
};


class QTextEdit : public QWidget
{
public:
   QTextEdit(QWidget *parent = nullptr){}

   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   QString toPlainText();
};

