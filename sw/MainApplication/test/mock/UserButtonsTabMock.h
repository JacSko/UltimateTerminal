#pragma once
#include <gmock/gmock.h>
#include "UserButtonsTab.h"

namespace MainApplication
{

UserButtonsTab::UserButtonsTab(GUIController::GUIController& controller,
                      uint8_t tab_id,
                      uint8_t,
                      Utilities::Persistence::Persistence& persistence,
                      std::function<bool(int8_t portId, const std::string&)>):
m_gui_controller(controller),
m_tab_id(tab_id),
m_persistence(persistence)
{

}
UserButtonsTab::~UserButtonsTab()
{

}
void UserButtonsTab::onPersistenceRead(const PersistenceItems&)
{

}
void UserButtonsTab::onPersistenceWrite(PersistenceItems&)
{

}
bool UserButtonsTab::run()
{
   return true;
}
void UserButtonsTab::onTabNameChangeRequest()
{
}
void UserButtonsTab::setTabName(const std::string&)
{
}

}
