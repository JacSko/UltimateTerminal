#include <QtWidgets/QPushButton>
#include <optional>
#include <sstream>

#include "Logger.h"
#include "UserButton.h"
#include "Serialize.hpp"

namespace MainApplication
{


UserButton::UserButton(GUIController::GUIController& controller,
                       uint8_t id,
                       const std::string& button_name,
                       Utilities::Persistence::Persistence& persistence,
                       std::function<bool(int8_t portId, const std::string&)> writer):
m_gui_controller(controller),
m_persistence(persistence),
m_executor(writer, std::bind(&UserButton::onCommandExecutionEvent, this, std::placeholders::_1))
{
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTON%u[%s] Creating user button handler", id, m_settings.button_name.c_str());

   Utilities::Persistence::PersistenceListener::setName(std::string("BUTTON") + std::to_string(id));
   m_persistence.addListener(*this);
   m_settings.id = id;
   m_button_id = m_gui_controller.getButtonID(button_name);
   UT_Assert(m_button_id != UINT32_MAX);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.setButtonCheckable(m_button_id, true);
}
UserButton::~UserButton()
{
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTON%u[%s] Destroying user button handler", m_settings.id, m_settings.button_name.c_str());
   m_gui_controller.unsubscribeFromButtonEvent(m_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);

   m_persistence.removeListener(*this);
}
void UserButton::onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event)
{
   if (button_id == m_button_id)
   {
      if (event == GUIController::ButtonEvent::CLICKED)
      {
         onUserButtonClicked();
      }
      else if (event == GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED)
      {
         onUserButtonContextMenuRequested();
      }
   }
}
void UserButton::handleNewSettings(const Dialogs::UserButtonDialog::Settings& settings)
{
   m_settings = settings;
   int count = m_executor.parseCommands(m_settings.raw_commands);
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTON%u[%s] %s - %u commands, text [%s]", m_settings.id, m_settings.button_name.c_str(),  __func__, count, m_settings.raw_commands.c_str());
   setButtonName(m_settings.button_name);
}
void UserButton::onUserButtonContextMenuRequested()
{
   Dialogs::UserButtonDialog dialog;
   Dialogs::UserButtonDialog::Settings new_settings = {};

   UT_Log(USER_BTN_HANDLER, LOW, "BUTTON%u[%s] %s", m_settings.id, m_settings.button_name.c_str(), __func__);
   std::optional<bool> result = dialog.showDialog(m_gui_controller.getParent(), m_settings, new_settings, true);
   if (result)
   {
      if (result.value())
      {
         handleNewSettings(new_settings);
      }
   }
}
void UserButton::onUserButtonClicked()
{
   UT_Log(USER_BTN_HANDLER, HIGH, "BUTTON%u[%s] Sending commands", m_settings.id, m_settings.button_name.c_str());
   m_gui_controller.setButtonChecked(m_button_id, true);
   m_gui_controller.setButtonEnabled(m_button_id, false);
   m_executor.execute();
}
void UserButton::setButtonName(const std::string name)
{
   m_gui_controller.setButtonText(m_button_id, name);
}
void UserButton::onPersistenceRead(const PersistenceItems& items)
{
   Dialogs::UserButtonDialog::Settings new_settings = m_settings;

   Utilities::Persistence::readItem(items, "ButtonName", new_settings.button_name);
   Utilities::Persistence::readItem(items, "ButtonCommands", new_settings.raw_commands);

   UT_Log(USER_BTN_HANDLER, HIGH, "BUTTON%u[%s] Persistence read - text [%s]", m_settings.id, m_settings.button_name.c_str(), new_settings.raw_commands.c_str());

   handleNewSettings(new_settings);
}
void UserButton::onPersistenceWrite(PersistenceItems& buffer)
{
   UT_Log(USER_BTN_HANDLER, HIGH, "BUTTON%u[%s] Persistence write - text [%s]", m_settings.id, m_settings.button_name.c_str(), m_settings.raw_commands.c_str());

   Utilities::Persistence::writeItem(buffer, "ButtonName", m_settings.button_name);
   Utilities::Persistence::writeItem(buffer, "ButtonCommands", m_settings.raw_commands);

}
void UserButton::onCommandExecutionEvent(bool result)
{
   m_gui_controller.setButtonChecked(m_button_id, false);
   m_gui_controller.setButtonEnabled(m_button_id, true);
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTON%u[%s] Commands executed %scorrectly", m_settings.id, m_settings.button_name.c_str(), result? "" : "not ");
}

}
