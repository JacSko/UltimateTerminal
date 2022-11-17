#pragma once

#include "QtWidgets/QPushButton"
#include "QtWidgets/QLineEdit"
#include <optional>
#include <regex>
#include "PersistenceHandler.h"
#include "TraceFilterSettingDialog.h"

/**
 * @file TraceFilterHandler.h
 *
 * @brief
 *    Class is responsible for filtering out traces based on regular expression defined by user.
 *
 * @details
 *    TraceFilterHandler handles the GUI elements directly related to port instance (QPushButton and QLineEdit) - it controls the button color, lineedit background and text color.
 *    Subscribes for clicked() signal of QPushButton and customContextMenuRequested() signal of QLineEdit.
 *    When context menu is requested, it shows the settings dialog that allows to configure the trace filter.
 *    Context menu is presented only if filter is not active.
 *    On push button clicked() signal it enables the filter.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

class TraceFilterHandler : public QObject,
                           public Persistence::PersistenceListener
{
   Q_OBJECT

public:
   /**< Describes settings used in module */
   struct TraceFilterSettings
   {
      Dialogs::TraceFilterSettingDialog::ColorSet colors; /**< Set of two colors used for trace presentation */
      std::string regular_expression;                     /**< User-defined regular expression */
   };

   /**
    * @brief Creates object.
    * @param[in] parent - parent widget.
    * @param[in] line_edit - pointer to related line edit.
    * @param[in] button - pointer to related push button
    * @param[in] persistence - persistence instance.
    *
    * @return None.
    */
   TraceFilterHandler(QWidget* parent, QLineEdit* line_edit, QPushButton* button, Persistence::PersistenceHandler& persistence);
   ~TraceFilterHandler();
   /**
    * @brief Checks matching of provided text with regular expression defined by user.
    * @param[in] text - trace to test
    *
    * @return Empty optional if text does not match the regular expression or filter is disabled.
    * @return Non-empty optional with background and font color if filter is enabled and text does match the regular expression.
    */
   std::optional<Dialogs::TraceFilterSettingDialog::ColorSet> tryMatch(const std::string& text);
   /**
    * @brief Refresh the related UI elements.
    * @return None.
    */
   void refreshUi();
   /**
    * @brief Sets new settings to module.
    * @details Settings can be set only if filter is not active.
    * @return True if settings accepted.
    */
   bool setSettings(const TraceFilterSettings& settings);
   /**
    * @brief Return the current module settings.
    * @return Current setting.
    */
   TraceFilterSettings getSettings();
   /**
    * @brief Checks if filter is active.
    * @return True if filter is active.
    */
   bool isActive();

private:
   void onPersistenceRead(const std::vector<uint8_t>& data);
   void onPersistenceWrite(std::vector<uint8_t>& data);

   QWidget* m_parent;
   QLineEdit* m_line_edit;
   QPushButton* m_button;
   Persistence::PersistenceHandler& m_persistence;
   std::regex m_regex;
   uint32_t m_background_color;
   uint32_t m_font_color;
   bool m_user_defined;

   void setButtonState(bool state);
   void setLineEditColor(uint32_t background_color, uint32_t font_color);
   bool filteringActive();
public slots:
   void onButtonClicked();
   void onContextMenuRequested();

};
