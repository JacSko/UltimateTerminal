#pragma once

#include <stdint.h>
#include <optional>
#include <vector>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
/**
 * @file UserButtonDialog.h
 *
 * @brief
 *    Dialog class that allows user to edit button details.
 *
 * @details
 *    Dialog contains place to enter button name and related commands.
 *    Current settings are presented to the user, their editability is configurable using allow_edit argument during showDialog() call.
 *    All settings are validated when user accepts dialog box. When user rejects, no settings change takes place.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */
namespace Dialogs
{

class UserButtonDialog : public QObject
{
   Q_OBJECT

public:
   UserButtonDialog();
   virtual ~UserButtonDialog();

   /** @brief Stores data from dialog fields */
   struct Settings
   {
      uint8_t id;                /**< ID of the button */
      std::string button_name;   /**< Name of the button. */
      std::string raw_commands;  /**< Commands in raw format, e.g. "CMD1\nCMD2"*/
      bool operator == (const Settings& lhs)
      {
         return (id == lhs.id) &&
                (button_name == lhs.button_name) &&
                (raw_commands == lhs.raw_commands);
      }
   };

   /**
    * @brief Shows dialog windows to user. This dialog allows to change button name and respective commands.
    *
    * Function is blocking until user press any control button (OK, Cancel, Exit).
    * Presents current_settings on startup, fields editability is controlled by allow_edit boolean.
    * If dialog was accepted by the user, the new settings are written to out_settings.
    *
    * @param[in] parent - parent QWidget for this dialog.
    * @param[in] current_settings - settings to be presented on startup
    * @param[out] out_settings - new settings read from dialog, filled only if user accepted the window.
    * @param[in] allow_edit - if set to false, then all fields are disabled and cannot be edited.
    *
    * @return If user accepted the dialog, the resulting value is not empty and contains result of the data validation.
    * @return If user declined the dialog, the empty value is returned.
    *
    */
   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QPushButton* m_helpButton;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_buttonNameEdit;
   QTextEdit* m_commandEdit;
   bool m_editable;

   void addDialogButtons();
   bool convertGuiValues(Settings& out_settings);

public slots:
   void showHelp();
};

}
