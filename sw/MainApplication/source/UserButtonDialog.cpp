#include <QtCore/QString>
#include "UserButtonDialog.h"
#include "Logger.h"
#include <sstream>

UserButtonDialog::UserButtonDialog():
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_buttonNameEdit(nullptr),
m_commandEdit(nullptr),
m_editable(true)
{
}
UserButtonDialog::~UserButtonDialog()
{
}
std::optional<bool> UserButtonDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   std::optional<bool> result;
   m_dialog = new QDialog(parent);
   m_form = new QFormLayout(m_dialog);
   m_editable = allow_edit;

   m_buttonNameEdit = new QLineEdit(m_dialog);
   m_buttonNameEdit->setText(QString(current_settings.button_name.c_str()));
   m_buttonNameEdit->setEnabled(allow_edit);
   m_form->addRow(m_buttonNameEdit);

   m_commandEdit = new QTextEdit(m_dialog);
   m_commandEdit->setText(QString(current_settings.raw_commands.c_str()));
   m_commandEdit->setEnabled(allow_edit);
   m_form->addRow(m_commandEdit);

   addDialogButtons();

   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(MAIN_GUI, INFO, "Command button dialog opened");
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "dialog accepted, gathering new settings");
      result = convertGuiValues(out_settings);
   }

   delete m_form;
   delete m_dialog;

   return result;
}

void UserButtonDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_buttonBox->setEnabled(m_editable);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
}
bool UserButtonDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.button_name = m_buttonNameEdit->text().toStdString();
   out_settings.raw_commands = m_commandEdit->toPlainText().toStdString();
   return true;
}
