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

/**
 * @file LoggingSettingDialog.cpp
 *
 * @brief
 *    Dialog window implementation that allows user to enter the file logging details.
 *
 * @details
 *    Dialog contains places to enter path and file name of the new log file.
 *    Current settings are presented to the user, their editability is configurable using allow_edit argument during showDialog() call.
 *    Logfile path is not editable by hand, but can be chosen using dedicated QFileDialog.
 *    There is possibility to enter own logfile name, or choose the automatic naming convention (log_ddmmrr_hhmmss).
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

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

      /**
       * @brief Returns the current path, which is obtained by adding path and name together.
       *
       * @return Path to the logfile.
       */
      std::string getPath()
      {
         return (file_path + "/" + file_name);
      }
   };

   /**
    * @brief Opens a logfile settings dialog.
    * @param[in] parent - parent Widget
    * @param[in] current_settings - settings to be presented to user
    * @param[in] out_settings - new settings gathered from user
    * @param[in] allow_edit - set window editability
    *
    * @return Optional has value if user accepted the dialog, has no value if user rejected the dialog.
    * @return If value of optional is true, new settings are correct. Otherwise false.
    */
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
