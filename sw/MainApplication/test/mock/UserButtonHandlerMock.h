#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "UserButtonHandler.h"

namespace GUI
{

UserButtonHandler::UserButtonHandler(GUIController& controller, const std::string& button_name, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer)
: m_gui_controller(controller),
 m_persistence(persistence),
 m_executor(writer, {})
{

}
UserButtonHandler::~UserButtonHandler()
{

}
void UserButtonHandler::onPersistenceRead(const std::vector<uint8_t>&)
{

}
void UserButtonHandler::onPersistenceWrite(std::vector<uint8_t>&)
{

}
bool UserButtonHandler::startThread()
{
   return true;
}
void UserButtonHandler::onButtonEvent(uint32_t, ButtonEvent){};
}
