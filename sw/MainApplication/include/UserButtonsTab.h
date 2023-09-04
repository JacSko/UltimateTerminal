#pragma once

#include "UserButton.h"
#include "Persistence.h"
#include "GUIController.h"

namespace MainApplication
{

class UserButtonsTab : public Utilities::Persistence::PersistenceListener,
                              public GUIController::TabNameChangeRequestListener
{
public:
   UserButtonsTab(GUIController::GUIController& controller, uint8_t tab_id, uint8_t buttons_count, Utilities::Persistence::Persistence& persistence, std::function<bool(const std::string&)> writer);
   ~UserButtonsTab();
   bool run();
private:

   GUIController::GUIController& m_gui_controller;
   const uint32_t m_tab_id;
   Utilities::Persistence::Persistence& m_persistence;
   std::string m_tab_name;
   std::vector<std::unique_ptr<UserButton>> m_buttons;
   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void onTabNameChangeRequest();
   void setTabName(const std::string& name);
};



}
