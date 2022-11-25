#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>

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
m_info_label(nullptr),
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
   m_current_path = current_path;

   m_dialog = new QDialog(parent);
   m_dialog->setPalette(parent->palette());

   m_dialog->setLayout(createLayout(current_path, allow_edit));
   addDialogButtons();
   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(MAIN_GUI, INFO, "logging dialog show");
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "dialog accepted, gathering new settings");
      result = convertGuiValues();
   }

   destroyLayout();
   delete m_dialog;
   m_dialog = nullptr;

   return result;
}
QLayout* LoggingSettingDialog::createLayout(const std::string& current_path, bool allow_edit)
{
   m_editable = allow_edit;
   m_form = new QFormLayout();

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

   m_info_label = new QLabel();
   if (!allow_edit)
   {
      m_info_label->setText("Change locked!\nDisable file logging to change this setting");
   }
   m_form->insertRow(3, "", m_info_label);

   return m_form;
}
void LoggingSettingDialog::destroyLayout()
{
   if (m_form)
   {
      delete m_form;
      m_form = nullptr;
   }
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
   QString dir = QFileDialog::getExistingDirectory(m_dialog, "Select directory",
                                                QString(m_current_path.c_str()),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
   if (!dir.isEmpty())
   {
      m_filePathEdit->setText(dir);
   }
}
std::string LoggingSettingDialog::convertGuiValues()
{
   UT_Assert(m_filePathEdit && "Create m_filePathEdit first!");
   return m_filePathEdit->text().toStdString();
}

}
