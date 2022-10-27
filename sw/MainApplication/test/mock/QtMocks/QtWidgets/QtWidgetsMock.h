#pragma once
#include <gmock/gmock.h>

#include <QtCore/QNamespace>
#include <QtCore/QString>
#include <QtCore/QtCoreMock.h>

#define Q_OBJECT

typedef unsigned int QRgb;

class QColor
{
public:
   QColor(uint32_t color):color(color){};
   bool isValid();
   QRgb rgb() const
   {
      return color;
   }
private:
   QRgb color;
};

class QPalette
{
public:
   enum ColorGroup { Active, Disabled, Inactive, NColorGroups, Current, All, Normal = Active };
   enum ColorRole { Button, };
   void setColor(ColorRole acr, const QColor &acolor);
   const QColor& color(ColorRole cr) const;
};

class QWidget : public QObject
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
   void addRow(const QString&, QWidget *widget);
   void addWidget(QWidget *w);
   void insertRow(int row, const QString &labelText, QWidget *field);
   void removeRow(QWidget *widget);
};

class QLineEdit : public QWidget
{
public:
   QLineEdit(){};
   QLineEdit(QDialog*&){};
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   void setMaxLength(int);
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

class QPushButton : public QWidget
{
public:
   QPushButton(QWidget *parent = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &text);
   const QPalette& palette();
   void setPalette(const QPalette &);
   void update();
};

class QComboBox : public QWidget
{
public:
   QComboBox(QWidget *parent = nullptr){}

   void* operator new(size_t);
   void operator delete(void*){};
   void addItem(const QString&);
   void setCurrentText(const QString&);
   QString currentText() const;
};

class QColorDialog : public QWidget
{
public:
   QColorDialog(QWidget *parent = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
   static QColor getColor(const QColor &initial = Qt::white,
                              QWidget *parent = nullptr,
                              const QString &title = QString());
};


struct QtWidgetsMock
{
   MOCK_METHOD0(QDialog_new, void*());
   MOCK_METHOD0(QDialog_exec, int());
   MOCK_METHOD1(QDialog_setWindowModality, void(Qt::WindowModality));

   MOCK_METHOD0(QFormLayout_new, void*());
   MOCK_METHOD1(QFormLayout_addRow, void(QWidget*));
   MOCK_METHOD1(QFormLayout_addWidget, void(QWidget*));
   MOCK_METHOD2(QFormLayout_addRow, void(const QString&, QWidget *widget));
   MOCK_METHOD3(QFormLayout_insertRow, void(int row, const QString &labelText, QWidget *field));
   MOCK_METHOD1(QFormLayout_removeRow, void(QWidget *field));

   MOCK_METHOD0(QLineEdit_new, void*());
   MOCK_METHOD1(QLineEdit_setText, void(const std::string&));
   MOCK_METHOD0(QLineEdit_text, QString());
   MOCK_METHOD1(QLineEdit_setMaxLength, void(int));

   MOCK_METHOD0(QTextEdit_new, void*());
   MOCK_METHOD1(QTextEdit_setText, void(const std::string&));
   MOCK_METHOD0(QTextEdit_toPlainText, QString());

   MOCK_METHOD0(QComboBox_new, void*());
   MOCK_METHOD1(QComboBox_addItem, void(const QString&));
   MOCK_METHOD1(QComboBox_setCurrentText, void(const QString&));
   MOCK_CONST_METHOD0(QComboBox_currentText, QString());

   MOCK_METHOD0(QPushButton_new, void*());
   MOCK_METHOD1(QPushButton_setText, void(const QString&));
   MOCK_METHOD0(QPushButton_palette, const QPalette& ());
   MOCK_METHOD1(QPushButton_setPalette, void(const QPalette&));
   MOCK_METHOD0(QPushButton_update, void());

   MOCK_METHOD0(QDialogButtonBox_new, void*());

   MOCK_METHOD1(QWidget_setEnabled, void(bool));
   MOCK_METHOD1(QWidget_setDisabled, void(bool));

   MOCK_METHOD2(QPalette_setColor, void(QPalette::ColorRole, const QColor&));
   MOCK_CONST_METHOD1(QPalette_color, const QColor&(QPalette::ColorRole));

   MOCK_METHOD0(QColorDialog_new, void*());
   MOCK_METHOD3(QColorDialog_getColor, QColor(const QColor&, QWidget*, const QString&));

   MOCK_METHOD0(QColor_isValid, bool());
};

void QtWidgetsMock_init();
void QtWidgetsMock_deinit();
QtWidgetsMock* QtWidgetsMock_get();


