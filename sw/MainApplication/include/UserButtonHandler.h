#pragma once

#include <QtCore/QObject>

#include "GenericListener.hpp"
#include "UserButtonDialog.h"
#include "PersistenceHandler.hpp"
#include "ButtonCommandsExecutor.hpp"
namespace GUI
{

class UserButtonHandler : public QObject,
                          public Persistence::PersistenceListener
{
   Q_OBJECT
public:
   UserButtonHandler(QPushButton* object, QWidget* parent, Persistence::PersistenceHandler& persistence, std::function<bool(const std::string&)> writer);
   ~UserButtonHandler();

private:

   QPushButton* m_object;
   QWidget* m_parent;
   UserButtonDialog::Settings m_settings;
   Persistence::PersistenceHandler& m_persistence;
   ButtonCommandsExecutor m_executor;

   void handleNewSettings(const UserButtonDialog::Settings&);
   void setButtonName(const std::string name);
   void onPersistenceRead(const std::vector<uint8_t>& data) override;
   void onPersistenceWrite(std::vector<uint8_t>& data) override;
   void onCommandExecutionEvent(bool result);
public slots:
   void onUserButtonContextMenuRequested();
   void onUserButtonClicked();
   void onCommandsFinished();
signals:
   void commandsFinished();

};



}
