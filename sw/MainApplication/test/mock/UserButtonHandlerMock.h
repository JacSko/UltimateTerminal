#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "UserButtonHandler.h"

namespace GUI
{

UserButtonHandler::UserButtonHandler(QPushButton*, QWidget*, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer)
:m_persistence(persistence),
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

}
