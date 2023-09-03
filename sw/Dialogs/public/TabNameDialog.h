#pragma once
#include <optional>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>

namespace Dialogs
{

class TabNameDialog : public QObject
{
   Q_OBJECT

public:
   TabNameDialog();
   virtual ~TabNameDialog();

   /**
    * @brief Shows dialog windows to user. This dialog allows to change tab name.
    *
    * Function is blocking until user press any control button (OK, Cancel, Exit).
    * Presents current_settings on startup.
    * If dialog was accepted by the user, the new settings are written to out_settings.
    *
    * @param[in] parent - parent QWidget for this dialog.
    * @param[in] current_name - settings to be presented on startup
    *
    * @return If user accepted the dialog, the resulting value is not empty and contains result of the data validation.
    * @return If user declined the dialog, the empty value is returned.
    *
    */
   std::optional<std::string> showDialog(QWidget* parent, const std::string& current_name);
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_tabNameEdit;
};

}
