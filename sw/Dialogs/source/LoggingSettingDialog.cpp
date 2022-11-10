#include <QtWidgets/QFileDialog>

#include "LoggingSettingDialog.h"
#include "Logger.h"

namespace Dialogs
{

LoggingSettingDialog::LoggingSettingDialog():
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_editable(true),
m_fileNameEdit(nullptr),
m_filePathEdit(nullptr),
m_pathSelectButton(nullptr),
m_autoNameBox(nullptr)
{
}
LoggingSettingDialog::~LoggingSettingDialog()
{

}
std::optional<bool> LoggingSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   std::optional<bool> result;
   m_dialog = new QDialog(parent);
   m_form = new QFormLayout(m_dialog);
   m_editable = allow_edit;
   m_current_settings = current_settings;

   // create file path edit
   QString file_path_label = QString("File path:");
   m_filePathEdit = new QLineEdit(m_dialog);
   m_filePathEdit->setText(QString(current_settings.file_path.c_str()));
   m_filePathEdit->setDisabled(true);
   m_form->insertRow(1, file_path_label, m_filePathEdit);

   // create file path button
   QString path_select_label = QString("");
   m_pathSelectButton = new QPushButton(m_dialog);
   m_pathSelectButton->setText("BROWSE");
   m_pathSelectButton->setDisabled(!m_editable);
   m_form->insertRow(2, path_select_label, m_pathSelectButton);
   QObject::connect(m_pathSelectButton, SIGNAL(clicked()), this, SLOT(onPathSelectButtonClicked()));

   // create file name edit
   QString file_name_label = QString("File name:");
   m_fileNameEdit = new QLineEdit(m_dialog);
   m_fileNameEdit->setText(QString(current_settings.file_name.c_str()));
   m_fileNameEdit->setDisabled(!m_editable || current_settings.use_default_name);
   m_form->insertRow(3, file_name_label, m_fileNameEdit);

   // create default filename checkbox
   QString default_filename_label = QString("Use default filename:");
   m_autoNameBox = new QCheckBox(m_dialog);
   m_autoNameBox->setDisabled(!m_editable);
   m_autoNameBox->setChecked(current_settings.use_default_name);
   m_form->insertRow(4, default_filename_label, m_autoNameBox);
   QObject::connect(m_autoNameBox, SIGNAL(stateChanged(int)), this, SLOT(onAutoNameStateChanged(int)));

   addDialogButtons();

   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(MAIN_GUI, INFO, "logging dialog show");
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(MAIN_GUI, HIGH, "dialog accepted, gathering new settings");
      result = convertGuiValues(out_settings);
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
                                                QString(m_current_settings.file_path.c_str()),
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
   m_filePathEdit->setText(dir);
}
void LoggingSettingDialog::onAutoNameStateChanged(int state)
{
   m_fileNameEdit->setDisabled(state);
}
bool LoggingSettingDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.file_path = m_filePathEdit->text().toStdString();
   out_settings.file_name = m_fileNameEdit->text().toStdString();
   out_settings.use_default_name = m_autoNameBox->isChecked();

   if (out_settings.use_default_name)
   {
      auto currentTime = std::chrono::system_clock::now();
      std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
      auto ts = localtime (&tt);
      out_settings.file_name = QString().asprintf("log_%u%.2u%u_%.2u%.2u%.2u",
                                      ts->tm_mday, ts->tm_mon, ts->tm_year + 1900,
                                      ts->tm_hour, ts->tm_min, ts->tm_sec).toStdString();
   }
   return true;
}

}
