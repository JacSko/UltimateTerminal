#pragma once

#include "QtWidgets/QPushButton"
#include "QtWidgets/QLineEdit"
#include <optional>
#include <regex>
#include "Persistence.h"
#include "TraceFilterSettingDialog.h"
#include "GUIController.h"

/**
 * @file TraceFilter.h
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
namespace MainApplication
{

class TraceFilter : public QObject,
                           public Utilities::Persistence::PersistenceListener,
                           public GUIController::ButtonEventListener
{
   Q_OBJECT

public:
   /**
    * @brief Creates object.
    * @param[in] id - id of the filter in system.
    * @param[in] controller - GUI controller.
    * @param[in] button_name - name of the related button.
    * @param[in] persistence - persistence instance.
    *
    * @return None.
    */
   TraceFilter(uint8_t id, GUIController::GUIController& controller, const std::string& button_name, Utilities::Persistence::Persistence& persistence);
   ~TraceFilter();
   /**
    * @brief Checks matching of provided text with regular expression defined by user.
    * @param[in] text - trace to test
    *
    * @return Empty optional if text does not match the regular expression or filter is disabled.
    * @return Non-empty optional with background and font color if filter is enabled and text does match the regular expression.
    */
   std::optional<Dialogs::TraceFilterSettingDialog::Settings> tryMatch(const std::string& text);
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
   bool setSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings);
   /**
    * @brief Return the current module settings.
    * @return Current setting.
    */
   Dialogs::TraceFilterSettingDialog::Settings getSettings();
   /**
    * @brief Checks if filter is active.
    * @return True if filter is active.
    */
   bool isActive();

private:
   /* ButtonEventListener */
   void onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event);

   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void handleNewSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings);

   GUIController::GUIController& m_gui_controller;
   uint32_t m_button_id;
   Utilities::Persistence::Persistence& m_persistence;
   std::regex m_regex;
   Dialogs::TraceFilterSettingDialog::Settings m_settings;
   bool m_user_defined;
   bool m_is_active;

   void setButtonState(bool state);
   void setLineEditColor(uint32_t background_color, uint32_t font_color);
   bool filteringActive();
public slots:
   void onButtonClicked();
   void onContextMenuRequested();

};

}
