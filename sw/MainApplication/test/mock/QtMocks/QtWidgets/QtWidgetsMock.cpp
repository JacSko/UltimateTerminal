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
QString QLineEdit::text()
{
   return QString(g_widgets_mock->QLineEdit_text(this).c_str());
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
void* QDialogButtonBox::operator new(size_t)
{
   return g_widgets_mock->QDialogButtonBox_new();
}
void QWidget::setEnabled(bool enabled)
{
   g_widgets_mock->QWidget_setEnabled(this, enabled);
}
void QWidget::setDisabled(bool disabled)
{
   g_widgets_mock->QWidget_setDisabled(this, disabled);
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
void* QPushButton::operator new(size_t)
{
   return g_widgets_mock->QPushButton_new();
}
void QPushButton::setText(const QString &text)
{
   g_widgets_mock->QPushButton_setText(this, text);
}
QPalette QPushButton::palette()
{
   return g_widgets_mock->QPushButton_palette(this);
}
void QPushButton::setPalette(const QPalette & palette)
{
   g_widgets_mock->QPushButton_setPalette(this, palette);
}
void QPushButton::update()
{
   g_widgets_mock->QPushButton_update(this);
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
bool QColor::isValid()
{
   return g_widgets_mock->QColor_isValid(this);
}