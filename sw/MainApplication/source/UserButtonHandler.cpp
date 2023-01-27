#include <QtWidgets/QPushButton>
#include <optional>
#include <sstream>

#include "Logger.h"
#include "UserButtonHandler.h"
#include "Serialize.hpp"

namespace GUI
{

const uint32_t THREAD_START_TIMEOUT = 1000;

UserButtonHandler::UserButtonHandler(GUIController& controller, uint8_t id, const std::string& button_name, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer):
m_gui_controller(controller),
m_persistence(persistence),
m_executor(writer, std::bind(&UserButtonHandler::onCommandExecutionEvent, this, std::placeholders::_1))
{
   UT_Log(USER_BTN_HANDLER, INFO, "Creating user button handler for button %u", id);

   Persistence::PersistenceListener::setName(std::string("BUTTON") + std::to_string(id));
   m_persistence.addListener(*this);
   m_settings.id = id;
   m_button_id = m_gui_controller.getButtonID(button_name);
   UT_Assert(m_button_id != UINT32_MAX);
   m_gui_controller.subscribeForButtonEvent(m_button_id, ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.setButtonCheckable(m_button_id, true);
}
UserButtonHandler::~UserButtonHandler()
{
   m_gui_controller.unsubscribeFromButtonEvent(m_button_id, ButtonEvent::CLICKED, this);
   m_gui_controller.unsubscribeFromButtonEvent(m_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, this);

   m_persistence.removeListener(*this);
}
void UserButtonHandler::onButtonEvent(uint32_t button_id, ButtonEvent event)
{
   if (button_id == m_button_id)
   {
      if (event == ButtonEvent::CLICKED)
      {
         onUserButtonClicked();
      }
      else if (event == ButtonEvent::CONTEXT_MENU_REQUESTED)
      {
         onUserButtonContextMenuRequested();
      }
   }
}

bool UserButtonHandler::startThread()
{
   return m_executor.start(THREAD_START_TIMEOUT);
}
void UserButtonHandler::handleNewSettings(const Dialogs::UserButtonDialog::Settings& settings)
{
   m_settings = settings;
   int count = m_executor.parseCommands(m_settings.raw_commands);
   UT_Log(USER_BTN_HANDLER, HIGH, "processed %d commands", count);
   setButtonName(m_settings.button_name);
}
void UserButtonHandler::onUserButtonContextMenuRequested()
{
   Dialogs::UserButtonDialog dialog;
   Dialogs::UserButtonDialog::Settings new_settings = {};

   std::optional<bool> result = dialog.showDialog(m_gui_controller.getParent(), m_settings, new_settings, true);
   if (result)
   {
      if (result.value())
      {
         handleNewSettings(new_settings);
      }
   }
}
void UserButtonHandler::onUserButtonClicked()
{
   UT_Log(USER_BTN_HANDLER, HIGH, "Sending commands for button %s [%s]", getName().c_str(), m_settings.button_name.c_str());
   m_gui_controller.setButtonChecked(m_button_id, true);
   m_gui_controller.setButtonEnabled(m_button_id, false);
   m_executor.execute();
}
void UserButtonHandler::setButtonName(const std::string name)
{
   m_gui_controller.setButtonText(m_button_id, name);
}
void UserButtonHandler::onPersistenceRead(const std::vector<uint8_t>& data)
{
   uint32_t offset = 0;
   Dialogs::UserButtonDialog::Settings new_settings = {};

   ::deserialize(data, offset, new_settings.button_name);
   ::deserialize(data, offset, new_settings.raw_commands);

   handleNewSettings(new_settings);
}
void UserButtonHandler::onPersistenceWrite(std::vector<uint8_t>& data)
{
   ::serialize(data, m_settings.button_name);
   ::serialize(data, m_settings.raw_commands);

}
void UserButtonHandler::onCommandExecutionEvent(bool result)
{
   m_gui_controller.setButtonChecked(m_button_id, false);
   m_gui_controller.setButtonEnabled(m_button_id, true);
   UT_Log(USER_BTN_HANDLER, LOW, "Commands executed %scorrectly", result? "" : "not ");
}

}
