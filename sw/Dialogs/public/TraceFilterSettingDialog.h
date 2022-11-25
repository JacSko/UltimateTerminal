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
#include <QtWidgets/QLabel>

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
      uint8_t id;            /**< ID of the filter   */
      std::string regex;     /**< Regular expression */
      uint32_t background;   /**< Background color.  */
      uint32_t font;         /**< Font color         */
      bool operator == (const Settings& lhs)
      {
         return (regex == lhs.regex)           &&
                (background == lhs.background) &&
                (font == lhs.font);
      }
      bool operator != (const Settings& lhs)
      {
         return (!(*this == lhs));
      }
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
   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
   /**
    * @brief Creates a layout related to dialog window that can be placed in another QWidget (i.e application settings).
    * @param[in] current_settings - settings to be presented to user
    * @param[in] allow_edit - set window editability
    *
    * @return Pointer to created Layout.
    */
   QLayout* createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit);
   /**
    * @brief Destroys previously created layout.
    * @return None.
    */
   void destroyLayout();
   /**
    * @brief Converts the values from currently created layout. Method createLayout() has to be called prior to conversion.
    * @return True if gathered settings are correct, othwerwise false.
    * @return If settings are correct, out_settings are filled with the new data.
    */
   bool convertGuiValues(Settings& out_settings);
private:
   QWidget* m_parent;
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_regexEdit;
   QPushButton* m_backgroundButton;
   QPushButton* m_fontButton;
   QLabel* m_info_label;
   Settings m_settings;

   void addDialogButtons();
public slots:
   void onBackgroundColorButtonClicked();
   void onFontColorButtonClicked();

};

}
