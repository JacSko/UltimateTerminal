#include "QtWidgets/QColorDialog"

#include "TraceFilterHandler.h"
#include "Logger.h"
#include "Serialize.hpp"

static uint8_t TRACE_FILTER_FIELD_ID = 0;

TraceFilterHandler::TraceFilterHandler(QWidget* parent, QLineEdit* line_edit, QPushButton* button, Persistence::PersistenceHandler& persistence):
m_parent(parent),
m_line_edit(line_edit),
m_button(button),
m_persistence(persistence)
{
   UT_Assert(line_edit && "Line edit cannot be NULL");
   UT_Assert(button && "Button cannot be NULL");
   UT_Assert(parent && "Parent cannot be NULL");

   TRACE_FILTER_FIELD_ID++;

   Persistence::PersistenceListener::setName(std::string("TRACE_FILTER") + std::to_string(TRACE_FILTER_FIELD_ID));
   m_persistence.addListener(*this);

   connect(m_button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
   m_button->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   m_line_edit->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
   connect(m_button, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenuRequested()));
   connect(m_line_edit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onContextMenuRequested()));
   connect(m_line_edit, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   m_button->setCheckable(true);
   m_line_edit->setDisabled(false);
   setButtonColor(Qt::red);
   m_color = m_line_edit->palette().color(QPalette::Base);

   UT_Log(TRACE_FILTER, LOW, "Created trace filter with id %u", TRACE_FILTER_FIELD_ID);
}
TraceFilterHandler::~TraceFilterHandler()
{
   m_persistence.removeListener(*this);
}
std::optional<uint32_t> TraceFilterHandler::tryMatch(const std::string& text)
{
   if (filteringActive())
   {
      if (std::regex_search(text, m_regex))
      {
         UT_Log(TRACE_FILTER, HIGH, "Found match for filer %s", getName().c_str());
         return m_color.rgb();
      }
      return {};
   }
   else
   {
      return {};
   }
}
void TraceFilterHandler::onPersistenceRead(const std::vector<uint8_t>& data)
{
   UT_Log(TRACE_FILTER, LOW, "Restoring persistence for %s", getName().c_str());

   uint32_t offset = 0;

   uint32_t color;
   std::string text;
   bool is_active;

   ::deserialize(data, offset, is_active);
   ::deserialize(data, offset, text);
   ::deserialize(data, offset, color);

   m_color = color;
   setLineEditColor(color);
   setButtonColor(is_active? Qt::red : Qt::green);
   m_button->setChecked(!is_active);
   m_line_edit->setDisabled(!is_active);
   m_line_edit->setText(QString(text.c_str()));
   m_regex = std::regex(text);

   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : color %.6x, active %u, text %s", getName().c_str(), color, is_active, text.c_str());
}
void TraceFilterHandler::onPersistenceWrite(std::vector<uint8_t>& data)
{
   UT_Log(TRACE_FILTER, LOW, "Saving persistence for %s", getName().c_str());
   uint32_t color = m_color.rgb();
   std::string text = m_line_edit->text().toStdString();
   bool is_active = m_line_edit->isEnabled();

   ::serialize(data, is_active);
   ::serialize(data, text);
   ::serialize(data, color);

   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : color %.6x, active %u, text %s", getName().c_str(), color, is_active, text.c_str());

}
void TraceFilterHandler::onButtonClicked()
{
   if (!filteringActive())
   {
      UT_Log(TRACE_FILTER, INFO, "enabling filter %s", getName().c_str());
      m_regex = std::regex(m_line_edit->text().toStdString());
      setButtonColor(Qt::green);
      m_button->setChecked(true);
      m_line_edit->setDisabled(true);
   }
   else
   {
      UT_Log(TRACE_FILTER, INFO, "disabling filter %s", getName().c_str());
      setButtonColor(Qt::red);
      m_button->setChecked(false);
      m_line_edit->setDisabled(false);
   }
}
void TraceFilterHandler::onContextMenuRequested()
{
   if(!filteringActive())
   {
      UT_Log(TRACE_FILTER, LOW, "showing color dialog for %s", getName().c_str());
      QColor color = QColorDialog::getColor(m_color, m_parent, "Select color");
      if (color.isValid())
      {
         m_color = color;
         setLineEditColor(color);
      }
   }
   else
   {
      UT_Log(TRACE_FILTER, LOW, "Cannot show color dialog - filter is active!");
   }
}
void TraceFilterHandler::setButtonColor(QColor color)
{
   QPalette palette = m_button->palette();
   palette.setColor(QPalette::Button, color);
   m_button->setPalette(palette);
   m_button->update();
}
void TraceFilterHandler::setLineEditColor(QColor color)
{
   UT_Log(TRACE_FILTER, LOW, "setting color for %s to %.6x", getName().c_str(), color.rgb());
   QPalette palette = m_line_edit->palette();
   palette.setColor(QPalette::Base, color);
   m_line_edit->setPalette(palette);
   m_line_edit->update();
}
bool TraceFilterHandler::filteringActive()
{
   return !m_line_edit->isEnabled();
}
