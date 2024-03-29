#include <QtCore/QString>
#include "MessageDialog.h"
#include "UserButtonDialog.h"
#include "Logger.h"
#include <sstream>

namespace Dialogs
{

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
   m_dialog->setPalette(parent->palette());
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
   UT_Log(GUI_DIALOG, LOW, "%s [%u:%s], edit possible %u", __func__, current_settings.id, current_settings.button_name.c_str(), allow_edit);
   if (m_dialog->exec() == QDialog::Accepted)
   {
      result = convertGuiValues(out_settings);
      UT_Log(GUI_DIALOG, LOW, "%s [%u:%s], new content [%s]", __func__, current_settings.id, out_settings.button_name.c_str(), out_settings.raw_commands.c_str());
   }

   delete m_form;
   delete m_dialog;
   UT_Log(GUI_DIALOG, LOW, "%s result %s", __func__, result.has_value()? (result.value()? "OK" : "NOK") : "NO_VALUE");

   return result;
}
void UserButtonDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_buttonBox->setEnabled(m_editable);
   m_helpButton = new QPushButton();
   m_helpButton->setText(QString("HELP"));
   m_form->addWidget(m_helpButton);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
   QObject::connect(m_helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
}
bool UserButtonDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.button_name = m_buttonNameEdit->text().toStdString();
   out_settings.raw_commands = m_commandEdit->toPlainText().toStdString();
   UT_Log(GUI_DIALOG, HIGH, "got name %s and raw commands %s", out_settings.button_name.c_str(), out_settings.raw_commands.c_str());
   return true;
}
void UserButtonDialog::showHelp()
{
   std::string helpText = std::string("Commands can be written in a sequence, line by line.\n") +
                                      "Example:\n"
                                      "\t command 1\n"
                                      "\t command 2\n\n"
                                      "There are also some special control commands for different purposes.\n\n"
                                      "__wait(<time_in_ms>)\n"
                                      "This commands suspends the execution for defined time\n"
                                      "Time shall be in milliseconds\n\n"
                                      "\t __repeat_start(<count>)\n"
                                      "\t __repeat_end()\n"
                                      "This set of commands allow to repeats execution of defined commands in the loop.\n\n"
                                      "Commands can be send also to specific port by using the '@x>' prefix where 'x' is the port ID\n"
                                      "Acceptable are port ID's in range 0-4\n\n"
                                      "\t @0>command1\n"
                                      "This command will be send to port with ID 0\n";
   UT_Log(GUI_DIALOG, LOW, "%s", __func__);
   MessageDialog::show(MessageDialog::Icon::Information, "Button help", helpText, m_dialog->palette());

}
}
