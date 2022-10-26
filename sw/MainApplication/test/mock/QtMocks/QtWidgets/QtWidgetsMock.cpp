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
   return g_widgets_mock->QDialog_exec();
}

void QDialog::setWindowModality(Qt::WindowModality windowModality)
{
   g_widgets_mock->QDialog_setWindowModality(windowModality);
}

void* QFormLayout::operator new(size_t)
{
   return g_widgets_mock->QFormLayout_new();
}

void QFormLayout::addRow(QWidget* widget)
{
   g_widgets_mock->QFormLayout_addRow(widget);
}

void QFormLayout::addWidget(QWidget* widget)
{
   g_widgets_mock->QFormLayout_addWidget(widget);
}

void* QLineEdit::operator new(size_t)
{
   return g_widgets_mock->QLineEdit_new();
}

void QLineEdit::setText(const QString & text)
{
   g_widgets_mock->QLineEdit_setText(text.toStdString());
}

QString QLineEdit::text()
{
   return QString(g_widgets_mock->QLineEdit_text().c_str());
}

void* QTextEdit::operator new(size_t)
{
   return g_widgets_mock->QTextEdit_new();
}

void QTextEdit::setText(const QString & text)
{
   g_widgets_mock->QTextEdit_setText(text.toStdString());
}

QString QTextEdit::toPlainText()
{
   return QString(g_widgets_mock->QTextEdit_toPlainText().c_str());
}

void* QDialogButtonBox::operator new(size_t)
{
   return g_widgets_mock->QDialogButtonBox_new();
}

void QWidget::setEnabled(bool enabled)
{
   g_widgets_mock->QWidget_setEnabled(enabled);
}

void QWidget::setDisabled(bool disabled)
{
   g_widgets_mock->QWidget_setDisabled(disabled);
}
