#include "QtWidgetsMock.h"
#include "Logger.h"

QtWidgetsMock* g_widgets_mock;

void QtWidgetsMock_init()
{
   if (!g_widgets_mock)
   {
      g_widgets_mock = new QtWidgetsMock();
   }
}

void QtWidgetsMock_deinit()
{
   if (g_widgets_mock)
   {
      delete g_widgets_mock;
      g_widgets_mock = nullptr;
   }
}

QtWidgetsMock* QtWidgetsMock_get()
{
   UT_Assert(g_widgets_mock && "Create widgets mock first!");
   return g_widgets_mock;
}

QObject* sender()
{
   return g_widgets_mock->QSender();
}

void* QDialog::operator new(size_t)
{
   return g_widgets_mock->QDialog_new();
}
int QDialog::exec()
{
   return g_widgets_mock->QDialog_exec(this);
}
void QDialog::setWindowModality(Qt::WindowModality windowModality)
{
   g_widgets_mock->QDialog_setWindowModality(this, windowModality);
}
void QDialog::setWindowTitle(const QString& text)
{
   g_widgets_mock->QDialog_setWindowTitle(this, text);
}
void* QFormLayout::operator new(size_t)
{
   return g_widgets_mock->QFormLayout_new();
}
void QFormLayout::addRow(QWidget* widget)
{
   g_widgets_mock->QFormLayout_addRow(this, widget);
}
void QFormLayout::addWidget(QWidget* widget)
{
   g_widgets_mock->QFormLayout_addWidget(this, widget);
}
void QFormLayout::addRow(const QString& text, QWidget *widget)
{
   g_widgets_mock->QFormLayout_addRow(this, text, widget);
}
void QFormLayout::insertRow(int row, const QString &labelText, QWidget *field)
{
   g_widgets_mock->QFormLayout_insertRow(this, row, labelText, field);
}
void QFormLayout::removeRow(QWidget *widget)
{
   g_widgets_mock->QFormLayout_removeRow(this, widget);
}
void* QLineEdit::operator new(size_t)
{
   return g_widgets_mock->QLineEdit_new();
}
void QLineEdit::setText(const QString & text)
{
   g_widgets_mock->QLineEdit_setText(this, text.toStdString());
}
void QLineEdit::setMaxLength(int length)
{
   g_widgets_mock->QLineEdit_setMaxLength(this, length);
}
void QLineEdit::setContextMenuPolicy(Qt::ContextMenuPolicy policy)
{
   g_widgets_mock->QLineEdit_setContextMenuPolicy(this, policy);
}
QString QLineEdit::text()
{
   return QString(g_widgets_mock->QLineEdit_text(this).c_str());
}
void QLineEdit::setStyleSheet(const QString& styleSheet)
{
   stylesheet = styleSheet;
   g_widgets_mock->QLineEdit_setStyleSheet(this, styleSheet);
}

void* QTextEdit::operator new(size_t)
{
   return g_widgets_mock->QTextEdit_new();
}
void QTextEdit::setText(const QString & text)
{
   g_widgets_mock->QTextEdit_setText(this, text.toStdString());
}

QString QTextEdit::toPlainText()
{
   return QString(g_widgets_mock->QTextEdit_toPlainText(this).c_str());
}
void QTextEdit::setReadOnly(bool read_only)
{
   g_widgets_mock->QTextEdit_setReadOnly(this, read_only);
}
void QTextEdit::setWordWrapMode(QTextOption option)
{
   g_widgets_mock->QTextEdit_setWordWrapMode(this, option);
}
void QTextEdit::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
   g_widgets_mock->QTextEdit_setHorizontalScrollBarPolicy(this, policy);
}
void QTextEdit::append(const QString& text)
{
   vertical_scroll_bar.maximum_position++;
   g_widgets_mock->QTextEdit_append(this, text);
}
void QTextEdit::clear()
{
   g_widgets_mock->QTextEdit_clear(this);
}
void* QDialogButtonBox::operator new(size_t)
{
   return g_widgets_mock->QDialogButtonBox_new();
}
QPushButton* QDialogButtonBox::button(QDialogButtonBox::StandardButton bn)
{
   return g_widgets_mock->QDialogButtonBox_button(this, bn);
}
void QWidget::setEnabled(bool enabled)
{
   g_widgets_mock->QWidget_setEnabled(this, enabled);
}
void QWidget::setDisabled(bool disabled)
{
   g_widgets_mock->QWidget_setDisabled(this, disabled);
}
bool QWidget::isEnabled()
{
   return g_widgets_mock->QWidget_isEnabled(this);
}
void* QComboBox::operator new(size_t)
{
   return g_widgets_mock->QComboBox_new();
}
void QComboBox::addItem(const QString& text)
{
   g_widgets_mock->QComboBox_addItem(this, text);
}
void QComboBox::setCurrentText(const QString& text)
{
   g_widgets_mock->QComboBox_setCurrentText(this, text);
}
QString QComboBox::currentText()
{
   return g_widgets_mock->QComboBox_currentText(this);
}
int QComboBox::count()
{
   return g_widgets_mock->QComboBox_count(this);
}
int QComboBox::findText(const QString &text)
{
   return g_widgets_mock->QComboBox_findText(this, text);
}
QString QComboBox::itemText(int idx)
{
   return g_widgets_mock->QComboBox_itemText(this, idx);
}
void QComboBox::insertItem(int idx, const QString& text)
{
   g_widgets_mock->QComboBox_insertItem(this, idx, text);
}
void QComboBox::removeItem(int index)
{
   g_widgets_mock->QComboBox_removeItem(this, index);
}
int QComboBox::currentIndex()
{
   return g_widgets_mock->QComboBox_currentIndex(this);
}
void QComboBox::setCurrentIndex(int idx)
{
   g_widgets_mock->QComboBox_setCurrentIndex(this, idx);
}
void* QPushButton::operator new(size_t)
{
   return g_widgets_mock->QPushButton_new();
}
void QPushButton::setText(const QString &text)
{
   g_widgets_mock->QPushButton_setText(this, text);
}
void QPushButton::setContextMenuPolicy(Qt::ContextMenuPolicy policy)
{
   g_widgets_mock->QPushButton_setContextMenuPolicy(this, policy);
}
void QPushButton::setCheckable(bool value)
{
   g_widgets_mock->QPushButton_setCheckable(this, value);
}
void QPushButton::setChecked(bool value)
{
   g_widgets_mock->QPushButton_setChecked(this, value);
}
void QPushButton::repaint()
{
   g_widgets_mock->QPushButton_repaint(this);
}
void* QColorDialog::operator new(size_t)
{
   return g_widgets_mock->QColorDialog_new();
}
QColor QColorDialog::getColor(const QColor &initial, QWidget *parent, const QString &title)
{
   return g_widgets_mock->QColorDialog_getColor(initial, parent, title);
}
void* QMessageBox::operator new(size_t)
{
   return g_widgets_mock->QMessageBox_new();
}
int QMessageBox::critical(QWidget *parent, const QString &title,
                        const QString& text,
                        const QString& button0Text)
{
   return g_widgets_mock->QMessageBox_critical(this, parent, title, text, button0Text);
}
void QMessageBox::exec()
{
   g_widgets_mock->QMessageBox_exec(this);
}
void* QLabel::operator new(size_t)
{
   return g_widgets_mock->QLabel_new();
}
void QLabel::setAutoFillBackground(bool enabled)
{
   g_widgets_mock->QLabel_setAutoFillBackground(this, enabled);
}
void QLabel::setAlignment(Qt::AlignmentFlag alignment)
{
   g_widgets_mock->QLabel_setAlignment(this, alignment);
}
void QLabel::setText(const QString & text)
{
   g_widgets_mock->QLabel_setText(this, text);
}
void QLabel::setStyleSheet(const QString& styleSheet)
{
   g_widgets_mock->QLabel_setStyleSheet(this, styleSheet);
}
void* QListWidgetItem::operator new(size_t)
{
   return g_widgets_mock->QListWidgetItem_new();
}
void QListWidgetItem::setText(const QString &atext)
{
   g_widgets_mock->QListWidgetItem_setText(this, atext);
}
void QListWidgetItem::setBackground(const QColor &color)
{
   g_widgets_mock->QListWidgetItem_setBackground(this, color);
}
void QListWidgetItem::setForeground(const QColor &color)
{
   g_widgets_mock->QListWidgetItem_setForeground(this, color);
}
void QListWidget::addItem(QListWidgetItem *aitem)
{
   g_widgets_mock->QListWidget_addItem(this, aitem);
}
void QListWidget::scrollToBottom()
{
   g_widgets_mock->QListWidget_scrollToBottom(this);
}
int QListWidget::count()
{
   return g_widgets_mock->QListWidget_count(this);
}
void QListWidget::clear()
{
   g_widgets_mock->QListWidget_clear(this);
}
void QListWidget::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
   g_widgets_mock->QListWidget_setHorizontalScrollBarPolicy(policy);
}
void QListWidget::setWordWrap(bool wrap)
{
   g_widgets_mock->QListWidget_setWordWrap(wrap);
}
void* QCheckBox::operator new(size_t)
{
   return g_widgets_mock->QCheckBox_new();
}
void* QListWidget::operator new(size_t)
{
   return g_widgets_mock->QListWidget_new();
}
void* QMenuBar::operator new(size_t)
{
   return g_widgets_mock->QMenuBar_new();
}
void* QShortcut::operator new(size_t)
{
   return g_widgets_mock->QShortcut_new();
}
void* QStatusBar::operator new(size_t)
{
   return g_widgets_mock->QStatusBar_new();
}
void QStatusBar::showMessage(const QString& text, uint32_t timeout)
{
   g_widgets_mock->QStatusBar_showMessage(this, text, timeout);
}
void* QTabWidget::operator new(size_t)
{
   return g_widgets_mock->QTabWidget_new();
}
void QTabWidget::addTab(QWidget* widget, const QString& text)
{
   g_widgets_mock->QTabWidget_addTab(this, widget, text);
}
QString QTabWidget::tabText(int index)
{
   return g_widgets_mock->QTabWidget_tabText(this, index);
}
void QTabWidget::setTabText(int index, const QString& text)
{
   g_widgets_mock->QTabWidget_setTabText(this, index, text);
}
int QTabWidget::count()
{
   return g_widgets_mock->QTabWidget_count(this);
}
void QScrollArea::setWidget(QWidget* widget)
{
   g_widgets_mock->QScrollArea_setWidget(this, widget);
}
void* QScrollArea::operator new(size_t )
{
   return g_widgets_mock->QScrollArea_new();
}
