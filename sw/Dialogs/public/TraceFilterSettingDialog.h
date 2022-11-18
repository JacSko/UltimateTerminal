#pragma once

/**
 * @file TraceFilterSettingDialog.h
 *
 * @brief
 *    Dialog class to allow user to configure trace filer entity.
 *
 * @details
 *    Dialog contains two fields - background and font color. These settings allows to configure how trace item will be visible in trace terminal view.
 *    To show dialog to user, call showDialog() method (this is blocking until user reaction).
 *    There is no possibility to block editability.
 *
 * @author Jacek Skowronek
 * @date   15/11/2022
 *
 */

/* =============================
 *   Includes of standard headers
 * =============================*/
#include <stdint.h>
#include <optional>
#include <vector>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>

namespace Dialogs
{

class TraceFilterSettingDialog : public QObject
{
   Q_OBJECT

public:
   TraceFilterSettingDialog();
   virtual ~TraceFilterSettingDialog();

   /** @brief Stores data from dialog fields */
   struct Settings
   {
      std::string regex;     /**< Regular expression */
      uint32_t background;   /**< Background color. */
      uint32_t font;         /**< Font color */
   };

   /**
    * @brief Shows dialog window to user. This dialog allows to change colors.
    *
    * Function is blocking until user press any control button (OK, Cancel, Exit).
    * Presents current_settings on startup.
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
   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings);
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_regexEdit;
   QPushButton* m_backgroundButton;
   QPushButton* m_fontButton;

   void addDialogButtons();
   bool convertGuiValues(Settings& out_settings);
public slots:
   void onBackgroundColorButtonClicked();
   void onFontColorButtonClicked();

};

}
