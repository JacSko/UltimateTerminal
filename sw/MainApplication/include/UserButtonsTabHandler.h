#pragma once

#include "UserButtonHandler.h"
#include "PersistenceHandler.h"
#include "GUIController.h"

namespace GUI
{

class UserButtonsTabHandler : public Persistence::PersistenceListener,
                              public TabNameChangeRequestListener
{
public:
   UserButtonsTabHandler(GUIController& controller, uint8_t tab_id, uint8_t buttons_count, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer);
   ~UserButtonsTabHandler();
   bool run();
private:

   GUIController& m_gui_controller;
   const uint32_t m_tab_id;
   Persistence::PersistenceHandler& m_persistence;
   std::string m_tab_name;
   std::vector<std::unique_ptr<UserButtonHandler>> m_buttons;
   void onPersistenceRead(const PersistenceItems&) override;
   void onPersistenceWrite(PersistenceItems&) override;
   void onTabNameChangeRequest();
   void setTabName(const std::string& name);
};



}
