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
 *    Dialog contains places to enter path where new logfiles should be stored.
 *    Current path is presented to the user, editability is configurable using allow_edit argument during showDialog() call.
 *    Logfile path is not editable by hand, but can be chosen using dedicated QFileDialog.
 *    Module user shall take care of generating the logfile name.
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

   /**
    * @brief Opens a logfile settings dialog.
    * @param[in] parent - parent Widget
    * @param[in] current_path - settings to be presented to user
    * @param[in] allow_edit - set window editability
    *
    * @return Optional has value if user accepted the dialog, has no value if user rejected the dialog.
    * @return If value of optional is true, new logging path is returned.
    */
   std::optional<std::string> showDialog(QWidget* parent, const std::string& current_path, bool allow_edit);

   /**
    * @brief Creates a layout related to dialog window that can be placed in another QWidget (i.e application settings).
    * @param[in] current_path - settings to be presented to user
    * @param[in] allow_edit - set window editability
    *
    * @return Pointer to new Layout. User is responsible for calling destroyLayout afterwards.
    */
   QLayout* createLayout(const std::string& current_path, bool allow_edit);
   /**
    * @brief Destroys previously created layout.
    * @return None.
    */
   void destroyLayout();
   /**
    * @brief Converts the values from currently created layout. Method createLayout() has to be called prior to conversion.
    * @return User selected logging path.
    */
   std::string convertGuiValues();
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_fileNameEdit;
   QLineEdit* m_filePathEdit;
   QPushButton* m_pathSelectButton;
   QLabel* m_info_label;
   QCheckBox* m_autoNameBox;
   bool m_editable;
   std::string m_current_path;

   void addDialogButtons();
public slots:
   void onPathSelectButtonClicked();

};

}

#endif
