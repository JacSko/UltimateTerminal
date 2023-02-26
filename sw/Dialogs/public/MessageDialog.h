#pragma once

#include <string>

#include <QtGui/QPalette>

/**
 * @file MessageDialog.h
 *
 * @brief
 *    This class allows to display short dialog with OK button to user.
 *
 * @details
 *    The dialog can have different meaning basing on Icon type. Window title and message are fully customizable, as well as dialog colors.
 *
 * @author Jacek Skowronek
 * @date   11/01/2023
 *
 */

namespace Dialogs
{

class MessageDialog
{
public:
   /**< Displayed icon type */
   enum class Icon
   {
      Information,    /**< Dialogs with Information icon presented */
      Warning,        /**< Dialogs with Warning icon presented */
      Critical,       /**< Dialogs with Critical icon presented */
   };

   /**
    * @brief Static method that presents the dialog to the user..
    * @param[in] icon - type of the icon to be placed on dialog
    * @param[in] window_title - title of the window
    * @param[in] text - message to be presented to user
    * @param[in] palette - colors to be set.
    *
    * @return None.
    */
   static void show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette = {});
};

}
