#pragma once
#include <gmock/gmock.h>

#include <QtCore/QNamespace>
#include <QtCore/QString>
#include <QtCore/QtCoreMock.h>

#define Q_OBJECT

typedef unsigned int QRgb;
class QMenuBar;
class QStatusBar;


class QColor
{
public:
   QColor() : color(0), valid(false){};
   QColor(uint32_t color):color(color), valid(true){};
   bool isValid(){return valid;};
   QRgb rgb() const
   {
      return color;
   }
   bool operator==(const QColor& rhs) const
   {
      return (rhs.color == color);
   }
private:
   QRgb color;
   bool valid;
};
class QPalette
{
public:
   enum ColorGroup { Active, Disabled, Inactive, NColorGroups, Current, All, Normal = Active };
   enum ColorRole { Button, Base};
   QPalette(ColorRole cr, QColor color):
   current_role(cr),
   current_color(color)
   {

   }
   void setColor(ColorRole acr, const QColor& acolor)
   {
      current_role = acr;
      current_color = acolor;
   }
   QColor color(ColorRole cr)
   {
      if (cr == current_role)
      {
         return current_color;
      }
      else
      {
         return QColor(0);
      }
   }
   bool operator==(const QPalette& lhs) const
   {
      return (lhs.current_color == current_color) && (lhs.current_role == current_role);
   }
private:
   ColorRole current_role;
   QColor current_color;
};

class QSizePolicy
{
public:
   enum Policy {
       Fixed = 0,
       Minimum,
       Maximum,
       Preferred,
       MinimumExpanding,
       Expanding,
       Ignored
   };
   enum ControlType {
       DefaultType      = 0x00000001,
       ButtonBox        = 0x00000002,
       CheckBox         = 0x00000004,
       ComboBox         = 0x00000008,
       Frame            = 0x00000010,
       GroupBox         = 0x00000020,
       Label            = 0x00000040,
       Line             = 0x00000080,
       LineEdit         = 0x00000100,
       PushButton       = 0x00000200,
       RadioButton      = 0x00000400,
       Slider           = 0x00000800,
       SpinBox          = 0x00001000,
       TabWidget        = 0x00002000,
       ToolButton       = 0x00004000
   };
   QSizePolicy(){}
   QSizePolicy(Policy horizontal, Policy vertical, ControlType type = DefaultType){}
   void setHorizontalStretch(int stretchFactor){};
   void setVerticalStretch(int stretchFactor){};
   void setHeightForWidth(bool b){};
   bool hasHeightForWidth(){return true;}
};

class QRect
{
public:
   QRect(int a, int b, int c, int d){}
};

class QWidget : public QObject
{
public:
   QWidget(){};
   QWidget(QObject*){};
   void setEnabled(bool);
   void setDisabled(bool);
   bool isEnabled();
   QPalette palette(){return {QPalette::Button, Qt::red};}
   void setPalette(const QPalette &);
   void update(){};
   void setSizePolicy(QSizePolicy){};
   QSizePolicy sizePolicy(){return {};};
   void setGeometry(const QRect &){};

};

class QMainWindow : public QWidget
{
public:
   QMainWindow(){}
   QMainWindow(QWidget*){}
   void resize(int w, int h){};
   void setCentralWidget(QWidget *widget){};
   void setMenuBar(QMenuBar *menubar){};
   void setStatusBar(QStatusBar *statusbar){};
   void setWindowTitle(const QString & title){};
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
   QLineEdit(QWidget*&){};
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   void setMaxLength(int);
   void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
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
   void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
   void setCheckable(bool);
   void setChecked(bool);
   void repaint();

};

class QComboBox : public QWidget
{
public:
   QComboBox(QWidget *parent = nullptr){}

   void* operator new(size_t);
   void operator delete(void*){};
   void addItem(const QString&);
   void setCurrentText(const QString&);
   QString currentText();
   int findText(const QString &text);
   void removeItem(int index);
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

class QMessageBox : public QWidget
{
public:
   QMessageBox(){}
   void* operator new(size_t);
   void operator delete(void*){};
   int critical(QWidget *parent, const QString &title,
                           const QString& text,
                           const QString& button0Text);
};

class QLabel : public QWidget
{
public:
   QLabel(){}
   QLabel(QWidget*){}
   void* operator new(size_t);
   void operator delete(void*){};
   void setAutoFillBackground(bool enabled);
   void setAlignment(Qt::AlignmentFlag);
   void setText(const QString &);
   void setStyleSheet(const QString& styleSheet);
};

class QCheckBox : public QWidget
{
public:
   QCheckBox(){}
   void* operator new(size_t);
   void operator delete(void*){};
};

class QLayout : public QObject
{
public:
   enum SizeConstraint {
       SetDefaultConstraint,
       SetNoConstraint,
       SetMinimumSize,
       SetFixedSize,
       SetMaximumSize,
       SetMinAndMaxSize
   };

   QLayout(){}
   void setSizeConstraint(SizeConstraint){};
   void addWidget(QWidget *, int row, int column, int rowSpan, int columnSpan, Qt::AlignmentFlag = Qt::AlignmentFlag::AlignCenter){};
   void addWidget(QWidget *, int stretch = 0, Qt::AlignmentFlag alignment = Qt::AlignmentFlag::AlignCenter){};
   void addLayout(QLayout *layout, int stretch = 0){};
   void addLayout(QLayout *, int row, int column, int rowSpan, int columnSpan, Qt::AlignmentFlag = Qt::AlignmentFlag::AlignCenter){};
   void setContentsMargins(int left, int top, int right, int bottom){};
};

class QGridLayout : public QLayout
{
public:
   QGridLayout(){};
   QGridLayout(QWidget*){};
};

class QVBoxLayout : public QLayout
{
public:
   QVBoxLayout(){};
   QVBoxLayout(QWidget*){};
};

class QSplitter : public QWidget
{
public:
   QSplitter(){};
   QSplitter(QWidget*){}
   void setOrientation(Qt::Orientation){};
   void addWidget(QWidget *widget){};

};

class QListWidgetItem : public QWidget
{
public:
   QListWidgetItem(){};
   QListWidgetItem(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &atext);
   void setBackground(const QColor &color);
};
class QListWidget : public QWidget
{
public:
   QListWidget(){};
   QListWidget(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
   void addItem(QListWidgetItem *aitem);
   void scrollToBottom();
   int count();
   void clear();

};

class QMenuBar : public QWidget
{
public:
   QMenuBar(){};
   QMenuBar(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
};

class QStatusBar : public QWidget
{
public:
   QStatusBar(){};
   QStatusBar(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
};


struct QtWidgetsMock
{
   MOCK_METHOD0(QDialog_new, void*());
   MOCK_METHOD1(QDialog_exec, int(QDialog*));
   MOCK_METHOD2(QDialog_setWindowModality, void(QDialog*, Qt::WindowModality));

   MOCK_METHOD0(QFormLayout_new, void*());
   MOCK_METHOD2(QFormLayout_addRow, void(QFormLayout*, QWidget*));
   MOCK_METHOD2(QFormLayout_addWidget, void(QFormLayout*, QWidget*));
   MOCK_METHOD3(QFormLayout_addRow, void(QFormLayout*, const QString&, QWidget *widget));
   MOCK_METHOD4(QFormLayout_insertRow, void(QFormLayout*, int row, const QString &labelText, QWidget *field));
   MOCK_METHOD2(QFormLayout_removeRow, void(QFormLayout*, QWidget *field));

   MOCK_METHOD0(QLineEdit_new, void*());
   MOCK_METHOD2(QLineEdit_setText, void(QLineEdit*, const std::string&));
   MOCK_METHOD1(QLineEdit_text, QString(QLineEdit*));
   MOCK_METHOD2(QLineEdit_setMaxLength, void(QLineEdit*, int));
   MOCK_METHOD2(QLineEdit_setContextMenuPolicy, void(QLineEdit*, Qt::ContextMenuPolicy));

   MOCK_METHOD0(QTextEdit_new, void*());
   MOCK_METHOD2(QTextEdit_setText, void(QTextEdit*, const std::string&));
   MOCK_METHOD1(QTextEdit_toPlainText, QString(QTextEdit*));

   MOCK_METHOD0(QComboBox_new, void*());
   MOCK_METHOD2(QComboBox_addItem, void(QComboBox*, const QString&));
   MOCK_METHOD2(QComboBox_setCurrentText, void(QComboBox*, const QString&));
   MOCK_METHOD1(QComboBox_currentText, QString(QComboBox*));
   MOCK_METHOD2(QComboBox_findText, int(QComboBox*, const QString&));
   MOCK_METHOD2(QComboBox_removeItem, void(QComboBox*, int));

   MOCK_METHOD0(QPushButton_new, void*());
   MOCK_METHOD2(QPushButton_setText, void(QPushButton*, const QString&));
   MOCK_METHOD2(QPushButton_setContextMenuPolicy, void(QPushButton*, Qt::ContextMenuPolicy));
   MOCK_METHOD2(QPushButton_setCheckable, void(QPushButton*, bool));
   MOCK_METHOD2(QPushButton_setChecked, void(QPushButton*, bool));
   MOCK_METHOD1(QPushButton_repaint, void(QPushButton*));

   MOCK_METHOD0(QDialogButtonBox_new, void*());

   MOCK_METHOD2(QWidget_setEnabled, void(QWidget*, bool));
   MOCK_METHOD2(QWidget_setDisabled, void(QWidget*, bool));
   MOCK_METHOD1(QWidget_isEnabled, bool(QWidget*));
   MOCK_METHOD2(QWidget_setPalette, void(QWidget*, const QPalette&));

   MOCK_METHOD0(QColorDialog_new, void*());
   MOCK_METHOD3(QColorDialog_getColor, QColor(const QColor&, QWidget*, const QString&));

   MOCK_METHOD0(QMessageBox_new, void*());
   MOCK_METHOD5(QMessageBox_critical, int(QMessageBox*, QWidget*, const QString&, const QString&, const QString&));

   MOCK_METHOD0(QLabel_new, void*());
   MOCK_METHOD2(QLabel_setAutoFillBackground, void(QLabel*, bool));
   MOCK_METHOD2(QLabel_setAlignment, void(QLabel*, Qt::AlignmentFlag));
   MOCK_METHOD2(QLabel_setText, void(QLabel*, const QString&));
   MOCK_METHOD2(QLabel_setStyleSheet, void(QLabel*, const QString&));

   MOCK_METHOD0(QListWidgetItem_new, void*());
   MOCK_METHOD2(QListWidgetItem_setText, void(QListWidgetItem*, const QString&));
   MOCK_METHOD2(QListWidgetItem_setBackground, void(QListWidgetItem*, const QColor&));

   MOCK_METHOD0(QListWidget_new, void*());
   MOCK_METHOD2(QListWidget_addItem, void(QListWidget*, QListWidgetItem*));
   MOCK_METHOD1(QListWidget_scrollToBottom, void(QListWidget*));
   MOCK_METHOD1(QListWidget_count, int(QListWidget*));
   MOCK_METHOD1(QListWidget_clear, void(QListWidget*));

   MOCK_METHOD0(QCheckBox_new, void*());
   MOCK_METHOD0(QMenuBar_new, void*());
   MOCK_METHOD0(QStatusBar_new, void*());
};

void QtWidgetsMock_init();
void QtWidgetsMock_deinit();
QtWidgetsMock* QtWidgetsMock_get();


