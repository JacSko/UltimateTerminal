#include "TraceFilterHandler.h"
#include "Logger.h"
#include "Serialize.hpp"

static uint8_t TRACE_FILTER_FIELD_ID = 0;

TraceFilterHandler::TraceFilterHandler(QWidget* parent, QLineEdit* line_edit, QPushButton* button, Persistence::PersistenceHandler& persistence):
m_parent(parent),
m_line_edit(line_edit),
m_button(button),
m_persistence(persistence),
m_user_defined(false)
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
   setButtonState(false);

   m_settings.background = m_parent->palette().color(QPalette::Base).rgb();
   m_settings.font = m_parent->palette().color(QPalette::Text).rgb();
   m_settings.id = TRACE_FILTER_FIELD_ID;
   setLineEditColor(m_settings.background, m_settings.font);

   UT_Log(TRACE_FILTER, LOW, "Created trace filter with id %u", TRACE_FILTER_FIELD_ID);
}
TraceFilterHandler::~TraceFilterHandler()
{
   m_persistence.removeListener(*this);
}
std::optional<Dialogs::TraceFilterSettingDialog::Settings> TraceFilterHandler::tryMatch(const std::string& text)
{
   if (filteringActive())
   {
      if (std::regex_search(text, m_regex))
      {
         UT_Log(TRACE_FILTER, HIGH, "Found match for filer %s", getName().c_str());
         return m_settings;
      }
      return {};
   }
   else
   {
      return {};
   }
}
void TraceFilterHandler::refreshUi()
{
   setButtonState(m_button->palette().color(QPalette::Button) == Qt::green);
   if (!m_user_defined)
   {
      m_settings.background = m_parent->palette().color(QPalette::Base).rgb();
      m_settings.font = m_parent->palette().color(QPalette::Text).rgb();
   }
   setLineEditColor(m_settings.background, m_settings.font);
}
bool TraceFilterHandler::setSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   bool result = false;
   if (!filteringActive())
   {
      handleNewSettings(settings);
      result = true;
   }
   return result;
}
Dialogs::TraceFilterSettingDialog::Settings TraceFilterHandler::getSettings()
{
   return m_settings;
}
bool TraceFilterHandler::isActive()
{
   return filteringActive();
}
void TraceFilterHandler::onPersistenceRead(const std::vector<uint8_t>& data)
{
   UT_Log(TRACE_FILTER, LOW, "Restoring persistence for %s", getName().c_str());

   uint32_t offset = 0;

   uint32_t background_color;
   uint32_t font_color;
   uint8_t id;
   std::string text;
   bool is_active;
   Dialogs::TraceFilterSettingDialog::Settings settings;

   ::deserialize(data, offset, is_active);
   ::deserialize(data, offset, text);
   ::deserialize(data, offset, background_color);
   ::deserialize(data, offset, font_color);
   ::deserialize(data, offset, m_user_defined);
   ::deserialize(data, offset, id);

   settings.background = background_color;
   settings.font = font_color;
   settings.regex = text;
   settings.id = id;
   handleNewSettings(settings);
   setButtonState(!is_active);
   m_button->setChecked(!is_active);
   m_line_edit->setDisabled(!is_active);

   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : background color %.6x, font color %.6x active %u, text %s", getName().c_str(), m_settings.background, m_settings.font, is_active, text.c_str());
}
void TraceFilterHandler::onPersistenceWrite(std::vector<uint8_t>& data)
{
   UT_Log(TRACE_FILTER, LOW, "Saving persistence for %s", getName().c_str());
   std::string text = m_line_edit->text().toStdString();
   bool is_active = m_line_edit->isEnabled();

   ::serialize(data, is_active);
   ::serialize(data, text);
   ::serialize(data, m_settings.background);
   ::serialize(data, m_settings.font);
   ::serialize(data, m_user_defined);
   ::serialize(data, m_settings.id);
   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : id %u, background color %.6x, font color %.6x, active %u, text %s", getName().c_str(), m_settings.id, m_settings.background, m_settings.font, is_active, text.c_str());

}
void TraceFilterHandler::onButtonClicked()
{
   if (!filteringActive())
   {
      UT_Log(TRACE_FILTER, INFO, "enabling filter %s", getName().c_str());
      m_regex = std::regex(m_line_edit->text().toStdString());
      setButtonState(true);
      m_button->setChecked(true);
      m_line_edit->setDisabled(true);
   }
   else
   {
      UT_Log(TRACE_FILTER, INFO, "disabling filter %s", getName().c_str());
      setButtonState(false);
      m_button->setChecked(false);
      m_line_edit->setDisabled(false);
   }
}
void TraceFilterHandler::onContextMenuRequested()
{
   if(!filteringActive())
   {
      UT_Log(TRACE_FILTER, LOW, "showing color dialog for %s", getName().c_str());
      Dialogs::TraceFilterSettingDialog::Settings new_settings;
      Dialogs::TraceFilterSettingDialog dialog;
      std::optional<bool> result = dialog.showDialog(m_parent, m_settings, new_settings, true);
      if (result.has_value() && result.value())
      {
         m_user_defined = true;
         handleNewSettings(new_settings);
      }
   }
   else
   {
      UT_Log(TRACE_FILTER, LOW, "Cannot show color dialog - filter is active!");
   }
}
void TraceFilterHandler::handleNewSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   m_settings = settings;

   setLineEditColor(m_settings.background, m_settings.font);
   m_line_edit->setText(QString(m_settings.regex.c_str()));
   m_regex = std::regex(settings.regex);

}
void TraceFilterHandler::setButtonState(bool active)
{
   if (active)
   {
      QPalette palette = m_parent->palette();
      palette.setColor(QPalette::Button, Qt::green);
      palette.setColor(QPalette::ButtonText, Qt::black);
      m_button->setPalette(palette);
      m_button->update();
   }
   else
   {
      m_button->setPalette(m_parent->palette());
      m_button->update();
   }
}
void TraceFilterHandler::setLineEditColor(uint32_t background_color, uint32_t font_color)
{
   UT_Log(TRACE_FILTER, LOW, "setting %s bg %.6x text %.6x", getName().c_str(), background_color, font_color);
   QPalette palette = m_line_edit->palette();
   palette.setColor(QPalette::Base, QColor(background_color));
   palette.setColor(QPalette::Text, QColor(font_color));
   m_line_edit->setPalette(palette);
   m_line_edit->update();
}
bool TraceFilterHandler::filteringActive()
{
   return !m_line_edit->isEnabled();
}
