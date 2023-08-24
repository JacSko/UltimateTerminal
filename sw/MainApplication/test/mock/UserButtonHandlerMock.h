#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "UserButtonHandler.h"

namespace GUI
{

UserButtonHandler::UserButtonHandler(GUIController& controller, uint8_t id, const std::string&, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer)
: m_gui_controller(controller),
 m_persistence(persistence),
 m_executor(writer, {})
{

}
UserButtonHandler::~UserButtonHandler()
{

}
void UserButtonHandler::onPersistenceRead(const PersistenceItems&)
{

}
void UserButtonHandler::onPersistenceWrite(PersistenceItems&)
{

}
bool UserButtonHandler::startThread()
{
   return true;
}
void UserButtonHandler::onButtonEvent(uint32_t, ButtonEvent){};
}
