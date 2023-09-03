#pragma once
#include <gmock/gmock.h>
#include "UserButtonsTabHandler.h"

namespace GUI
{

UserButtonsTabHandler::UserButtonsTabHandler(GUIController& controller,
                      uint8_t tab_id,
                      uint8_t,
                      Persistence::PersistenceHandler& persistence,
                      std::function<bool(const std::string&)>):
m_gui_controller(controller),
m_tab_id(tab_id),
m_persistence(persistence)
{

}
UserButtonsTabHandler::~UserButtonsTabHandler()
{

}
void UserButtonsTabHandler::onPersistenceRead(const PersistenceItems&)
{

}
void UserButtonsTabHandler::onPersistenceWrite(PersistenceItems&)
{

}
bool UserButtonsTabHandler::run()
{
   return true;
}
void UserButtonsTabHandler::onTabNameChangeRequest()
{
}
void UserButtonsTabHandler::setTabName(const std::string&)
{
}

}
