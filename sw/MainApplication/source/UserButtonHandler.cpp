#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <optional>
#include <sstream>

#include "Logger.h"
#include "UserButtonHandler.h"
#include "Serialize.hpp"

namespace GUI
{

static uint8_t USER_BUTTON_ID = 0;

UserButtonHandler::UserButtonHandler(QPushButton* object, QWidget* parent, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer):
m_object(object),
m_parent(parent),
m_persistence(persistence),
m_executor(writer, std::bind(&UserButtonHandler::onCommandExecutionEvent, this, std::placeholders::_1))
{
   USER_BUTTON_ID++;
   UT_Log(MAIN, INFO, "Creating user button handler for button %u", USER_BUTTON_ID);

   UT_Assert(object && "invalid QObject pointer");

   Persistence::PersistenceListener::setName(std::string("BUTTON") + std::to_string(USER_BUTTON_ID));
   m_persistence.addListener(*this);

   object->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(object, SIGNAL(clicked()), this, SLOT(onUserButtonClicked()));
   connect(object, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onUserButtonContextMenuRequested()));
   connect(this, SIGNAL(commandsFinished()), this, SLOT(onCommandsFinished()));
   m_object->setCheckable(true);
}
UserButtonHandler::~UserButtonHandler()
{
   m_persistence.removeListener(*this);
}
void UserButtonHandler::handleNewSettings(const UserButtonDialog::Settings& settings)
{
   m_settings = settings;
   int count = m_executor.parseCommands(m_settings.raw_commands);
   UT_Log(MAIN, HIGH, "processed %d commands", count);
   setButtonName(m_settings.button_name);
}
void UserButtonHandler::onUserButtonContextMenuRequested()
{
   UserButtonDialog dialog;
   UserButtonDialog::Settings new_settings = {};
   /* TODO block editing when commands are ongoing */
   std::optional<bool> result = dialog.showDialog(m_parent, m_settings, new_settings, true);
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
   UT_Log(MAIN, HIGH, "Sending commands");
   m_object->setChecked(true);
   m_object->repaint();

   m_executor.execute();
}
void UserButtonHandler::setButtonName(const std::string name)
{
   m_object->setText(QString(name.c_str()));
}
void UserButtonHandler::onPersistenceRead(const std::vector<uint8_t>& data)
{
   uint32_t offset = 0;
   UserButtonDialog::Settings new_settings = {};

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
   emit commandsFinished();
   UT_Log(MAIN, LOW, "Commands executed %scorrectly", result? "" : "not ");
}

void UserButtonHandler::onCommandsFinished()
{
   m_object->setChecked(false);
   m_object->repaint();
}

}
