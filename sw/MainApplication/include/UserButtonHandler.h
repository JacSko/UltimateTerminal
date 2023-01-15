#pragma once

#include <QtCore/QObject>

#include "GenericListener.hpp"
#include "UserButtonDialog.h"
#include "PersistenceHandler.h"
#include "ButtonCommandsExecutor.hpp"
#include "IGUIController.h"

namespace GUI
{

class UserButtonHandler : public QObject,
                          public Persistence::PersistenceListener,
                          public IGUIController::ButtonEventListener
{
   Q_OBJECT
public:
   UserButtonHandler(IGUIController& controller, const std::string& button_name, QWidget* parent, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer);
   ~UserButtonHandler();
   bool startThread();
private:

   IGUIController& m_gui_controller;
   uint32_t m_button_id;
   QWidget* m_parent;
   Dialogs::UserButtonDialog::Settings m_settings;
   Persistence::PersistenceHandler& m_persistence;
   ButtonCommandsExecutor m_executor;

   /* ButtonEventListener */
   void onButtonEvent(uint32_t button_id, ButtonEvent event);

   void handleNewSettings(const Dialogs::UserButtonDialog::Settings&);
   void setButtonName(const std::string name);
   void onPersistenceRead(const std::vector<uint8_t>& data) override;
   void onPersistenceWrite(std::vector<uint8_t>& data) override;
   void onCommandExecutionEvent(bool result);
public slots:
   void onUserButtonContextMenuRequested();
   void onUserButtonClicked();
};



}
