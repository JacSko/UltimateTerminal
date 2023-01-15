#include "TraceFilterHandler.h"
#include "Logger.h"
#include "Serialize.hpp"

static uint8_t TRACE_FILTER_FIELD_ID = 0;

TraceFilterHandler::TraceFilterHandler(IGUIController& controller, const std::string& button_name, const std::string& lineedit_name, QWidget* parent, Persistence::PersistenceHandler& persistence):
m_gui_controller(controller),
m_parent(parent),
m_persistence(persistence),
m_user_defined(false),
m_is_active(false)
{
   UT_Assert(parent && "Parent cannot be NULL");

   TRACE_FILTER_FIELD_ID++;

   Persistence::PersistenceListener::setName(std::string("TRACE_FILTER") + std::to_string(TRACE_FILTER_FIELD_ID));
   m_persistence.addListener(*this);

   m_settings.background = m_gui_controller.getApplicationPalette().color(QPalette::Base).rgb();
   m_settings.font = m_gui_controller.getApplicationPalette().color(QPalette::Text).rgb();
   m_settings.id = TRACE_FILTER_FIELD_ID;

   m_button_id = m_gui_controller.getElementID(button_name);
   m_gui_controller.setButtonCheckable(m_button_id, true);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, true);
   setButtonState(false);
   setLineEditColor(m_settings.background, m_settings.font);

   UT_Log(TRACE_FILTER, LOW, "Created trace filter with id %u", TRACE_FILTER_FIELD_ID);
}
TraceFilterHandler::~TraceFilterHandler()
{
   m_persistence.removeListener(*this);
}
void TraceFilterHandler::onButtonEvent(uint32_t button_id, ButtonEvent event)
{
   if (button_id == m_button_id)
   {
      if (event == ButtonEvent::CLICKED)
      {
         onButtonClicked();
      }
      else if (event == ButtonEvent::CONTEXT_MENU_REQUESTED)
      {
         onContextMenuRequested();
      }
   }
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
   setButtonState(filteringActive());
   if (!m_user_defined)
   {
      m_settings.background = m_gui_controller.getApplicationPalette().color(QPalette::Base).rgb();
      m_settings.font = m_gui_controller.getApplicationPalette().color(QPalette::Text).rgb();
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

   m_gui_controller.setButtonChecked(m_button_id, !is_active);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, is_active);
   m_is_active = is_active;

   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : background color %.6x, font color %.6x active %u, text %s", getName().c_str(), m_settings.background, m_settings.font, is_active, text.c_str());
}
void TraceFilterHandler::onPersistenceWrite(std::vector<uint8_t>& data)
{
   UT_Log(TRACE_FILTER, LOW, "Saving persistence for %s", getName().c_str());
   std::string text = m_gui_controller.getTraceFilter(m_settings.id);

   ::serialize(data, m_is_active);
   ::serialize(data, text);
   ::serialize(data, m_settings.background);
   ::serialize(data, m_settings.font);
   ::serialize(data, m_user_defined);
   ::serialize(data, m_settings.id);
   UT_Log(TRACE_FILTER, HIGH, "Persistent data for %s : id %u, background color %.6x, font color %.6x, active %u, text %s", getName().c_str(), m_settings.id, m_settings.background, m_settings.font, m_is_active, text.c_str());

}
void TraceFilterHandler::onButtonClicked()
{
   if (!filteringActive())
   {
      UT_Log(TRACE_FILTER, INFO, "enabling filter %s", getName().c_str());
      m_regex = std::regex(m_gui_controller.getTraceFilter(m_settings.id));
      setButtonState(true);
      m_gui_controller.setButtonEnabled(m_button_id, false);
      m_gui_controller.setTraceFilterEnabled(m_settings.id, false);
      m_is_active = true;
   }
   else
   {
      UT_Log(TRACE_FILTER, INFO, "disabling filter %s", getName().c_str());
      setButtonState(false);
      m_gui_controller.setButtonEnabled(m_button_id, false);
      m_gui_controller.setTraceFilterEnabled(m_settings.id, false);
      m_is_active = false;
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
   m_gui_controller.setTraceFilter(m_settings.id, m_settings.regex.c_str());
   m_regex = std::regex(settings.regex);

}
void TraceFilterHandler::setButtonState(bool active)
{
   constexpr uint32_t GREEN = 0x00FF00;
   constexpr uint32_t BLUE = 0x0000FF;
   constexpr uint32_t BLACK = 0x000000;
   const uint32_t DEFAULT_BACKGROUND = m_gui_controller.getBackgroundColor();
   const uint32_t DEFAULT_FONT = m_gui_controller.getTextColor();

   if (active)
   {
      m_gui_controller.setButtonBackgroundColor(m_button_id, GREEN);
      m_gui_controller.setButtonFontColor(m_button_id, BLACK);
   }
   else
   {
      m_gui_controller.setButtonBackgroundColor(m_button_id, DEFAULT_BACKGROUND);
      m_gui_controller.setButtonFontColor(m_button_id, DEFAULT_FONT);
   }
}
void TraceFilterHandler::setLineEditColor(uint32_t background_color, uint32_t font_color)
{
   UT_Log(TRACE_FILTER, LOW, "setting %s bg %.6x text %.6x", getName().c_str(), background_color, font_color);
   m_gui_controller.setTraceFilterBackgroundColor(m_settings.id, background_color);
   m_gui_controller.setTraceFilterFontColor(m_settings.id, font_color);
}
bool TraceFilterHandler::filteringActive()
{
   return m_is_active;
}
