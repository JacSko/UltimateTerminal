#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "UserButton.h"

namespace MainApplication
{

UserButton::UserButton(GUIController::GUIController& controller,
                                     uint8_t id,
                                     const std::string&,
                                     Utilities::Persistence::Persistence& persistence,
                                     std::function<bool(const std::string&)> writer):
m_gui_controller(controller),
m_button_id(id),
m_persistence(persistence),
m_executor(writer, {})
{

}
UserButton::~UserButton()
{

}
void UserButton::onPersistenceRead(const PersistenceItems&)
{

}
void UserButton::onPersistenceWrite(PersistenceItems&)
{

}
void UserButton::onButtonEvent(uint32_t, GUIController::ButtonEvent){};
}
