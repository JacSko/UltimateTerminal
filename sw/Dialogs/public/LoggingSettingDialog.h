#ifndef LOGGING_SETTING_DIALOG_H_
#define LOGGING_SETTING_DIALOG_H_

#include <stdint.h>
#include <optional>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>

namespace Dialogs
{

class LoggingSettingDialog : public QObject
{
   Q_OBJECT

public:
   LoggingSettingDialog();
   ~LoggingSettingDialog();

   struct Settings
   {
      std::string file_path;
      std::string file_name;
      bool use_default_name;

      std::string getPath()
      {
         return (file_path + "/" + file_name);
      }
   };

   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_fileNameEdit;
   QLineEdit* m_filePathEdit;
   QPushButton* m_pathSelectButton;
   QCheckBox* m_autoNameBox;
   Settings m_current_settings;
   bool m_editable;

   void addDialogButtons();
   bool convertGuiValues(Settings& out_settings);
public slots:
   void onPathSelectButtonClicked();
   void onAutoNameStateChanged(int);

};

}

#endif
