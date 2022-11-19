#pragma once
#include <gmock/gmock.h>

#include <QtCore/QNamespace>
#include <QtCore/QString>
#include <QtCore/QtCoreMock.h>

#define Q_OBJECT

typedef unsigned int QRgb;
class QMenuBar;
class QStatusBar;
class QLayout;

constexpr uint32_t DEFAULT_APP_COLOR = 0xDEAD;

class QColor
{
public:
   QColor() : color(DEFAULT_APP_COLOR), valid(false){};
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
   enum ColorRole { Window, WindowText, Button, ButtonText, Base, Text};
   QPalette(ColorRole cr, QColor color)
   {
      m_color_map[cr] = color;
   }
   QPalette()
   {

   }
   void setColor(ColorRole acr, const QColor& acolor)
   {
      m_color_map[acr] = acolor;
   }
   QColor color(ColorRole cr)
   {
      if (m_color_map.find(cr) != m_color_map.end())
      {
         return m_color_map[cr];
      }
      else
      {
         return QColor(DEFAULT_APP_COLOR);
      }
   }
   bool operator==(const QPalette& lhs) const
   {
      return (lhs.m_color_map == m_color_map);
   }
private:
   std::map<ColorRole, QColor> m_color_map;
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
   QSizePolicy(Policy, Policy, ControlType = DefaultType){}
   void setHorizontalStretch(int){};
   void setVerticalStretch(int){};
   void setHeightForWidth(bool){};
   bool hasHeightForWidth(){return true;}
   void setVerticalPolicy(int){};
};

class QRect
{
public:
   QRect(int, int, int, int){}
};

class QWidget : public QObject
{
public:
   QWidget(){};
   QWidget(QObject*){};
   void setEnabled(bool);
   void setDisabled(bool);
   bool isEnabled();
   QPalette palette(){return m_palette;}
   void setPalette(const QPalette & palette) {m_palette = palette;}
   void setLayout(QLayout*){};
   void update(){};
   void setSizePolicy(QSizePolicy){};
   QSizePolicy sizePolicy(){return {};};
   void setGeometry(const QRect &){};

private:
   QPalette m_palette;
};

class QTabWidget : public QWidget
{
public:
   QTabWidget(){};
   void* operator new(size_t );
   void operator delete(void*){};
   void addTab(QWidget*, const QString& label);
};

class QShortcut : public QWidget
{
public:
   QShortcut(){}
   QShortcut(int, QWidget*){}
   void* operator new(size_t );
   void operator delete(void*){};
};

class QMainWindow : public QWidget
{
public:
   QMainWindow(){}
   QMainWindow(QWidget*){}
   void resize(int , int){};
   void setCentralWidget(QWidget *){};
   void setMenuBar(QMenuBar *){};
   void setStatusBar(QStatusBar *){};
   void setWindowTitle(const QString &){};
};

class QDialog : public QWidget
{
public:
   enum DialogCode { Rejected, Accepted };
   QDialog(){};
   QDialog(QWidget*&){};
   virtual ~QDialog(){};
   void* operator new(size_t );
   void operator delete(void*){};
   void setWindowModality(Qt::WindowModality);
   void setWindowTitle(const QString&);
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
   QDialogButtonBox(int, Qt::Orientation, QWidget * = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
};

class QLineEdit : public QWidget
{
public:
   QLineEdit(){};
   QLineEdit(QDialog*){};
   QLineEdit(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   void setMaxLength(int);
   void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
   QString text();
   void clear(){}
};

class QTextEdit : public QWidget
{
public:
   QTextEdit(QWidget * = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &);
   QString toPlainText();
};

class QPushButton : public QWidget
{
public:
   QPushButton(QWidget * = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString &text);
   void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
   void setCheckable(bool);
   void setChecked(bool);
   void repaint();

};

class QAbstractViewItem
{
public:
   void setPalette(const QPalette &){};
};

class QComboBox : public QWidget
{
public:
   enum InsertPolicy {
       NoInsert,
       InsertAtTop,
       InsertAtCurrent,
       InsertAtBottom,
       InsertAfterCurrent,
       InsertBeforeCurrent,
       InsertAlphabetically
   };

   QComboBox(QWidget * = nullptr){}

   void* operator new(size_t);
   void operator delete(void*){};
   void addItem(const QString&);
   void setCurrentText(const QString&);
   QString currentText();
   int findText(const QString &text);
   QString itemText(int index);
   void removeItem(int index);
   void setEditable(bool){}
   void setInsertPolicy(InsertPolicy){}
   QLineEdit* lineEdit(){return &edit;}
   QAbstractViewItem* view(){return &item;};
   void insertItem(int index, const QString&text);
   int count();
   void clear(){}
   int currentIndex();
   void setCurrentIndex(int idx);
private:
   QLineEdit edit;
   QAbstractViewItem item;
};

class QColorDialog : public QWidget
{
public:
   QColorDialog(QWidget * = nullptr){}
   void* operator new(size_t);
   void operator delete(void*){};
   static QColor getColor(const QColor &initial = Qt::white,
                              QWidget *parent = nullptr,
                              const QString &title = QString());
};

class QMessageBox : public QWidget
{
public:
   enum Type
   {
      Critical
   };
   QMessageBox(){}
   void* operator new(size_t);
   void operator delete(void*){};
   void setText(const QString&){};
   void setWindowTitle(const QString&){};
   void setIcon(int){};
   void exec();
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
   void addWidget(QWidget *, int, int, int, int, Qt::AlignmentFlag = Qt::AlignmentFlag::AlignCenter){};
   void addWidget(QWidget *, int = 0, Qt::AlignmentFlag = Qt::AlignmentFlag::AlignCenter){};
   void addLayout(QLayout *, int = 0){};
   void addLayout(QLayout *, int, int, int, int, Qt::AlignmentFlag = Qt::AlignmentFlag::AlignCenter){};
   void setContentsMargins(int, int, int, int){};
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

class QFormLayout : public QLayout
{
public:
   QFormLayout(){};
   QFormLayout(QDialog*&){}
   void* operator new(size_t);
   void operator delete(void*){};
   void addRow(QWidget *);
   void addRow(const QString&, QWidget *);
   void addWidget(QWidget *);
   void insertRow(int, const QString &, QWidget *);
   void removeRow(QWidget *);
};

class QSplitter : public QWidget
{
public:
   QSplitter(){};
   QSplitter(QWidget*){}
   void setOrientation(Qt::Orientation){};
   void addWidget(QWidget *){};

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
   void setForeground(const QColor &color);
};
class QListWidget : public QWidget
{
public:
   QListWidget(){};
   QListWidget(QWidget*){};
   void* operator new(size_t);
   void operator delete(void*){};
   void addItem(QListWidgetItem *);
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
   void showMessage(const QString&, uint32_t timeout);
};


struct QtWidgetsMock
{
   MOCK_METHOD0(QDialog_new, void*());
   MOCK_METHOD1(QDialog_exec, int(QDialog*));
   MOCK_METHOD2(QDialog_setWindowModality, void(QDialog*, Qt::WindowModality));
   MOCK_METHOD2(QDialog_setWindowTitle, void(QDialog*,const QString&));

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
   MOCK_METHOD2(QComboBox_itemText, QString(QComboBox*, int));
   MOCK_METHOD3(QComboBox_insertItem, void(QComboBox*, int, const QString&));
   MOCK_METHOD2(QComboBox_removeItem, void(QComboBox*, int));
   MOCK_METHOD1(QComboBox_count, int(QComboBox*));
   MOCK_METHOD1(QComboBox_currentIndex, int(QComboBox*));
   MOCK_METHOD2(QComboBox_setCurrentIndex, void(QComboBox*, int));

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

   MOCK_METHOD0(QColorDialog_new, void*());
   MOCK_METHOD3(QColorDialog_getColor, QColor(const QColor&, QWidget*, const QString&));

   MOCK_METHOD0(QMessageBox_new, void*());
   MOCK_METHOD5(QMessageBox_critical, int(QMessageBox*, QWidget*, const QString&, const QString&, const QString&));
   MOCK_METHOD1(QMessageBox_exec, void(QMessageBox*));

   MOCK_METHOD0(QLabel_new, void*());
   MOCK_METHOD2(QLabel_setAutoFillBackground, void(QLabel*, bool));
   MOCK_METHOD2(QLabel_setAlignment, void(QLabel*, Qt::AlignmentFlag));
   MOCK_METHOD2(QLabel_setText, void(QLabel*, const QString&));
   MOCK_METHOD2(QLabel_setStyleSheet, void(QLabel*, const QString&));

   MOCK_METHOD0(QListWidgetItem_new, void*());
   MOCK_METHOD2(QListWidgetItem_setText, void(QListWidgetItem*, const QString&));
   MOCK_METHOD2(QListWidgetItem_setBackground, void(QListWidgetItem*, const QColor&));
   MOCK_METHOD2(QListWidgetItem_setForeground, void(QListWidgetItem*, const QColor&));

   MOCK_METHOD0(QListWidget_new, void*());
   MOCK_METHOD2(QListWidget_addItem, void(QListWidget*, QListWidgetItem*));
   MOCK_METHOD1(QListWidget_scrollToBottom, void(QListWidget*));
   MOCK_METHOD1(QListWidget_count, int(QListWidget*));
   MOCK_METHOD1(QListWidget_clear, void(QListWidget*));

   MOCK_METHOD0(QStatusBar_new, void*());
   MOCK_METHOD3(QStatusBar_showMessage, void(QStatusBar*, const QString&, uint32_t timeout));

   MOCK_METHOD0(QTabWidget_new, void*());
   MOCK_METHOD3(QTabWidget_addTab, void(QTabWidget*, QWidget*, const QString&));

   MOCK_METHOD0(QCheckBox_new, void*());
   MOCK_METHOD0(QMenuBar_new, void*());
   MOCK_METHOD0(QShortcut_new, void*());
};

void QtWidgetsMock_init();
void QtWidgetsMock_deinit();
QtWidgetsMock* QtWidgetsMock_get();


