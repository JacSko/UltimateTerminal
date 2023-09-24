#pragma once

#include <QtCore/QObject>

#include "UserButtonDialog.h"
#include "Persistence.h"
#include "ButtonCommandsExecutor.hpp"
#include "GUIController.h"

namespace MainApplication
{

class UserButton : public QObject,
                          public Utilities::Persistence::PersistenceListener,
                          public GUIController::ButtonEventListener
{
   Q_OBJECT
public:
   UserButton(GUIController::GUIController& controller,
                     uint8_t id,
                     const std::string& button_name,
                     Utilities::Persistence::Persistence& persistence,
                     std::function<bool(const std::string&)> writer);
   ~UserButton();
private:

   GUIController::GUIController& m_gui_controller;
   uint32_t m_button_id;
   Dialogs::UserButtonDialog::Settings m_settings;
   Utilities::Persistence::Persistence& m_persistence;
   ButtonCommandsExecutor m_executor;

   /* ButtonEventListener */
   void onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event) override;

   void handleNewSettings(const Dialogs::UserButtonDialog::Settings&);
   void setButtonName(const std::string name);
   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void onCommandExecutionEvent(bool result);
   void onUserButtonContextMenuRequested();
   void onUserButtonClicked();
};



}
