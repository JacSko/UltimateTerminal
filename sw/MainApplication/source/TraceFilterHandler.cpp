#include "TraceFilterHandler.h"
#include "Logger.h"
#include "Serialize.hpp"

TraceFilterHandler::TraceFilterHandler(uint8_t id, GUIController& controller, const std::string& button_name, Persistence::PersistenceHandler& persistence):
m_gui_controller(controller),
m_persistence(persistence),
m_user_defined(false),
m_is_active(false)
{
   Persistence::PersistenceListener::setName(std::string("TRACE_FILTER") + std::to_string(id));
   m_persistence.addListener(*this);

   m_settings.background = m_gui_controller.getApplicationPalette().color(QPalette::Base).rgb();
   m_settings.font = m_gui_controller.getApplicationPalette().color(QPalette::Text).rgb();
   m_settings.id = id;

   m_button_id = m_gui_controller.getButtonID(button_name);
   UT_Assert(m_button_id != UINT32_MAX);
   m_gui_controller.subscribeForButtonEvent(m_button_id, ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.setButtonCheckable(m_button_id, true);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, true);
   setButtonState(false);
   setLineEditColor(m_settings.background, m_settings.font);

   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u Created trace filter with id %u", id);
}
TraceFilterHandler::~TraceFilterHandler()
{
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u Destroying trace filter", m_settings.id);
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
         UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u Found match", m_settings.id);
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
void TraceFilterHandler::onPersistenceRead(const PersistenceItems& items)
{
   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u restoring persistence", m_settings.id);
   Dialogs::TraceFilterSettingDialog::Settings settings = m_settings;
   bool isActive = false;

   Persistence::readItem(items, "isActive", isActive);
   Persistence::readItem(items, "text", settings.regex);
   Persistence::readItem(items, "backgroundColor", settings.background);
   Persistence::readItem(items, "fontColor", settings.font);
   Persistence::readItem(items, "userDefined", m_user_defined);
   Persistence::readItem(items, "id", settings.id);

   handleNewSettings(settings);
   setButtonState(isActive);

   m_gui_controller.setButtonChecked(m_button_id, isActive);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, !isActive);
   m_is_active = isActive;

   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u Persistent data: background color %.6x, font color %.6x active %u, text %s", m_settings.id, m_settings.background, m_settings.font, isActive, m_settings.regex.c_str());
}
void TraceFilterHandler::onPersistenceWrite(PersistenceItems& buffer)
{
   std::string text = m_gui_controller.getTraceFilter(m_settings.id);

   Persistence::writeItem(buffer, "isActive", m_is_active);
   Persistence::writeItem(buffer, "text", text);
   Persistence::writeItem(buffer, "backgroundColor", m_settings.background);
   Persistence::writeItem(buffer, "fontColor", m_settings.font);
   Persistence::writeItem(buffer, "userDefined", m_user_defined);
   Persistence::writeItem(buffer, "id", m_settings.id);

   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u Persistent data: background color %.6x, font color %.6x, active %u, text %s", m_settings.id, m_settings.background, m_settings.font, m_is_active, text.c_str());
}

void TraceFilterHandler::onButtonClicked()
{
   if (!filteringActive())
   {
      UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u enabling filter", m_settings.id);
      m_settings.regex = m_gui_controller.getTraceFilter(m_settings.id);
      m_regex = std::regex(m_settings.regex);
      setButtonState(true);
      m_gui_controller.setTraceFilterEnabled(m_settings.id, false);
      m_is_active = true;
   }
   else
   {
      UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u disabling filter", m_settings.id);
      setButtonState(false);
      m_gui_controller.setTraceFilterEnabled(m_settings.id, true);
      m_is_active = false;
   }
}
void TraceFilterHandler::onContextMenuRequested()
{
   if(!filteringActive())
   {
      UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u showing context", m_settings.id);
      Dialogs::TraceFilterSettingDialog::Settings new_settings;
      Dialogs::TraceFilterSettingDialog dialog;
      m_settings.regex = m_gui_controller.getTraceFilter(m_settings.id);

      std::optional<bool> result = dialog.showDialog(m_gui_controller.getParent(), m_settings, new_settings, true);
      if (result.has_value() && result.value())
      {
         m_user_defined = true;
         handleNewSettings(new_settings);
      }
   }
   else
   {
      UT_Log(TRACE_FILTER, ERROR, "TRACE_FILTER%u Cannot show dialog - filter is active!", m_settings.id);
   }
}
void TraceFilterHandler::handleNewSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   m_settings = settings;

   setLineEditColor(m_settings.background, m_settings.font);
   m_gui_controller.setTraceFilter(m_settings.id, m_settings.regex.c_str());
   m_regex = std::regex(settings.regex);
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u %s bg %.6x text %.6x, regex [%s]", m_settings.id, __func__, settings.background, settings.font, settings.regex.c_str());
}
void TraceFilterHandler::setButtonState(bool active)
{
   constexpr uint32_t GREEN = 0x00FF00;
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
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u bg %.6x text %.6x", m_settings.id,  background_color, font_color);
   m_gui_controller.setTraceFilterBackgroundColor(m_settings.id, background_color);
   m_gui_controller.setTraceFilterFontColor(m_settings.id, font_color);
}
bool TraceFilterHandler::filteringActive()
{
   return m_is_active;
}
