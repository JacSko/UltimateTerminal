#include <QtWidgets/QFileDialog>

#include "LoggingSettingDialog.h"
#include "Logger.h"

namespace Dialogs
{

LoggingSettingDialog::LoggingSettingDialog():
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_fileNameEdit(nullptr),
m_filePathEdit(nullptr),
m_pathSelectButton(nullptr),
m_autoNameBox(nullptr),
m_editable(true)
{
}
LoggingSettingDialog::~LoggingSettingDialog()
{

}
std::optional<std::string> LoggingSettingDialog::showDialog(QWidget* parent, const std::string& current_path, bool allow_edit)
{
   std::optional<std::string> result;
   m_editable = allow_edit;
   m_current_path = current_path;

   m_dialog = new QDialog(parent);
   m_dialog->setPalette(parent->palette());
   m_form = new QFormLayout(m_dialog);

   // create file path edit
   QString file_path_label = QString("File path:");
   m_filePathEdit = new QLineEdit(m_dialog);
   m_filePathEdit->setText(QString(current_path.c_str()));
   m_filePathEdit->setDisabled(true);
   m_filePathEdit->setMinimumSize(500, 0);
   m_form->insertRow(1, file_path_label, m_filePathEdit);

   // create file path button
   QString path_select_label = QString("");
   m_pathSelectButton = new QPushButton(m_dialog);
   m_pathSelectButton->setText("BROWSE");
   m_pathSelectButton->setDisabled(!m_editable);
   m_form->insertRow(2, path_select_label, m_pathSelectButton);
   QObject::connect(m_pathSelectButton, SIGNAL(clicked()), this, SLOT(onPathSelectButtonClicked()));

   addDialogButtons();

   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(MAIN_GUI, INFO, "logging dialog show");
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "dialog accepted, gathering new settings");
      result = convertGuiValues();
   }

   delete m_form;
   delete m_dialog;

   return result;
}

void LoggingSettingDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_buttonBox->setDisabled(!m_editable);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
}
void LoggingSettingDialog::onPathSelectButtonClicked()
{
   QString dir = QFileDialog::getExistingDirectory(m_dialog, tr("Select directory"),
                                                QString(m_current_path.c_str()),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
   m_filePathEdit->setText(dir);
}
std::string LoggingSettingDialog::convertGuiValues()
{
   return m_filePathEdit->text().toStdString();
}

}
