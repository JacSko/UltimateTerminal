#pragma once

#include "QtWidgets/QPushButton"
#include "QtWidgets/QLineEdit"
#include <optional>
#include <regex>
#include "PersistenceHandler.h"

class TraceFilterHandler : public QObject,
                           public Persistence::PersistenceListener
{
   Q_OBJECT

public:
   TraceFilterHandler(QWidget* parent, QLineEdit* line_edit, QPushButton* button, Persistence::PersistenceHandler& persistence);
   ~TraceFilterHandler();
   std::optional<uint32_t> tryMatch(const std::string&);

private:
   void onPersistenceRead(const std::vector<uint8_t>& data);
   void onPersistenceWrite(std::vector<uint8_t>& data);

   QWidget* m_parent;
   QLineEdit* m_line_edit;
   QPushButton* m_button;
   Persistence::PersistenceHandler& m_persistence;
   std::regex m_regex;
   QColor m_color;

   void setButtonColor(QColor);
   void setLineEditColor(QColor color);
   bool filteringActive();
public slots:
   void onButtonClicked();
   void onContextMenuRequested();

};
