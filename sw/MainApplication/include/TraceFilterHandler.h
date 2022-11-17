#pragma once

#include "QtWidgets/QPushButton"
#include "QtWidgets/QLineEdit"
#include <optional>
#include <regex>
#include "PersistenceHandler.h"
#include "TraceFilterSettingDialog.h"

class TraceFilterHandler : public QObject,
                           public Persistence::PersistenceListener
{
   Q_OBJECT

public:
   TraceFilterHandler(QWidget* parent, QLineEdit* line_edit, QPushButton* button, Persistence::PersistenceHandler& persistence);
   ~TraceFilterHandler();
   std::optional<Dialogs::TraceFilterSettingDialog::ColorSet> tryMatch(const std::string&);
   void refreshUi();

private:
   void onPersistenceRead(const std::vector<uint8_t>& data);
   void onPersistenceWrite(std::vector<uint8_t>& data);

   QWidget* m_parent;
   QLineEdit* m_line_edit;
   QPushButton* m_button;
   Persistence::PersistenceHandler& m_persistence;
   std::regex m_regex;
   uint32_t m_background_color;
   uint32_t m_font_color;
   bool m_user_defined;

   void setButtonState(bool state);
   void setLineEditColor(uint32_t background_color, uint32_t font_color);
   bool filteringActive();
public slots:
   void onButtonClicked();
   void onContextMenuRequested();

};
