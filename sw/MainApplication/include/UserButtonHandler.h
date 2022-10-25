#pragma once

#include <QtCore/QObject>

#include "GenericListener.hpp"
#include "UserButtonDialog.h"
#include "PersistenceHandler.h"

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
   std::function<bool(const std::string&)> m_writer;

   void handleNewSettings(const UserButtonDialog::Settings&);
   void setButtonName(const std::string name);
   void onPersistenceRead(const std::vector<uint8_t>& data) override;
   void onPersistenceWrite(std::vector<uint8_t>& data) override;
public slots:
   void onUserButtonContextMenuRequested();
   void onUserButtonClicked();
};



}
