#include "Logger.h"
#include "UserButtonsTabHandler.h"
#include "TabNameDialog.h"

namespace GUI
{

UserButtonsTabHandler::UserButtonsTabHandler(GUIController& controller,
                                             uint8_t tab_id,
                                             uint8_t buttons_count,
                                             Persistence::PersistenceHandler& persistence,
                                             std::function<bool(const std::string&)> writer):
m_gui_controller(controller),
m_tab_id(tab_id),
m_persistence(persistence),
m_tab_name("TAB" + std::to_string(tab_id))
{
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTONTAB%u[%s] Creating user button tab handler", m_tab_id, m_tab_name.c_str());

   Persistence::PersistenceListener::setName(std::string("BUTTONTAB") + std::to_string(m_tab_id));
   m_persistence.addListener(*this);
   m_gui_controller.subscribeForTabNameChangeRequest(m_tab_id, this);

   for (uint8_t i = 0; i < buttons_count; i++)
   {
      std::string buttonName = "BUTTON" + std::to_string((tab_id * buttons_count) + i);
      m_buttons.emplace_back(std::unique_ptr<GUI::UserButtonHandler>(new GUI::UserButtonHandler(
                                                                         m_gui_controller,
                                                                         i,
                                                                         buttonName,
                                                                         m_persistence,
                                                                         writer)));
      UT_Assert(m_buttons.back()->startThread());
      UT_Log(USER_BTN_HANDLER, LOW, "BUTTONTAB%u[%s] Creating button with name [%s]", m_tab_id,
                                                                                      m_tab_name.c_str(),
                                                                                      buttonName.c_str());
   }
   setTabName(m_tab_name);
}
UserButtonsTabHandler::~UserButtonsTabHandler()
{
   UT_Log(USER_BTN_HANDLER, LOW, "BUTTONTAB%u[%s] Destroying user button tab handler", m_tab_id, m_tab_name.c_str());
   m_persistence.removeListener(*this);
   m_gui_controller.unsubscribeFromTabNameChangeRequest(m_tab_id, this);
}
void UserButtonsTabHandler::onPersistenceRead(const PersistenceItems& items)
{
   std::string tab_name = "";
   Persistence::readItem(items, "tabName", tab_name);
   setTabName(tab_name);
   UT_Log(USER_BTN_HANDLER, HIGH, "BUTTONTAB%u[%s] Persistence read OK", m_tab_id, m_tab_name.c_str());
}
void UserButtonsTabHandler::onPersistenceWrite(PersistenceItems& buffer)
{
   UT_Log(USER_BTN_HANDLER, HIGH, "BUTTONTAB%u[%s] Persistence write", m_tab_id, m_tab_name.c_str());
   Persistence::writeItem(buffer, "tabName", m_tab_name);
}
void UserButtonsTabHandler::onTabNameChangeRequest()
{
   Dialogs::TabNameDialog dialog;
   std::optional<std::string> result = dialog.showDialog(m_gui_controller.getParent(), m_tab_name);
   if (result.has_value())
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "BUTTONTAB%u[%s] new tab name - [%s]", m_tab_id, m_tab_name.c_str(),
                                                                            result.value().c_str());
      setTabName(result.value());
   }
}
void UserButtonsTabHandler::setTabName(const std::string& name)
{
   m_tab_name = name;
   m_gui_controller.setTabName(m_tab_id, m_tab_name);
}

}
