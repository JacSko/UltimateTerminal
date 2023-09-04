#include "TraceFilter.h"
#include "Logger.h"
#include "Serialize.hpp"

namespace MainApplication
{

TraceFilter::TraceFilter(uint8_t id, GUIController::GUIController& controller, const std::string& button_name, Utilities::Persistence::Persistence& persistence):
m_gui_controller(controller),
m_persistence(persistence),
m_user_defined(false),
m_is_active(false)
{
   Utilities::Persistence::PersistenceListener::setName(std::string("TRACE_FILTER") + std::to_string(id));
   m_persistence.addListener(*this);

   m_settings.background = m_gui_controller.getApplicationPalette().color(QPalette::Base).rgb();
   m_settings.font = m_gui_controller.getApplicationPalette().color(QPalette::Text).rgb();
   m_settings.id = id;

   m_button_id = m_gui_controller.getButtonID(button_name);
   UT_Assert(m_button_id != UINT32_MAX);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CLICKED, this);
   m_gui_controller.subscribeForButtonEvent(m_button_id, GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED, this);
   m_gui_controller.setButtonCheckable(m_button_id, true);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, true);
   setButtonState(false);
   setLineEditColor(m_settings.background, m_settings.font);

   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u Created trace filter with id %u", id);
}
TraceFilter::~TraceFilter()
{
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u Destroying trace filter", m_settings.id);
   m_persistence.removeListener(*this);
}
void TraceFilter::onButtonEvent(uint32_t button_id, GUIController::ButtonEvent event)
{
   if (button_id == m_button_id)
   {
      if (event == GUIController::ButtonEvent::CLICKED)
      {
         onButtonClicked();
      }
      else if (event == GUIController::ButtonEvent::CONTEXT_MENU_REQUESTED)
      {
         onContextMenuRequested();
      }
   }
}
std::optional<Dialogs::TraceFilterSettingDialog::Settings> TraceFilter::tryMatch(const std::string& text)
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
void TraceFilter::refreshUi()
{
   setButtonState(filteringActive());
   if (!m_user_defined)
   {
      m_settings.background = m_gui_controller.getApplicationPalette().color(QPalette::Base).rgb();
      m_settings.font = m_gui_controller.getApplicationPalette().color(QPalette::Text).rgb();
   }
   setLineEditColor(m_settings.background, m_settings.font);
}
bool TraceFilter::setSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   bool result = false;
   if (!filteringActive())
   {
      handleNewSettings(settings);
      result = true;
   }
   return result;
}
Dialogs::TraceFilterSettingDialog::Settings TraceFilter::getSettings()
{
   return m_settings;
}
bool TraceFilter::isActive()
{
   return filteringActive();
}
void TraceFilter::onPersistenceRead(const PersistenceItems& items)
{
   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u restoring persistence", m_settings.id);
   Dialogs::TraceFilterSettingDialog::Settings settings = m_settings;
   bool isActive = false;

   Utilities::Persistence::readItem(items, "isActive", isActive);
   Utilities::Persistence::readItem(items, "text", settings.regex);
   Utilities::Persistence::readItem(items, "backgroundColor", settings.background);
   Utilities::Persistence::readItem(items, "fontColor", settings.font);
   Utilities::Persistence::readItem(items, "userDefined", m_user_defined);
   Utilities::Persistence::readItem(items, "id", settings.id);

   handleNewSettings(settings);
   setButtonState(isActive);

   m_gui_controller.setButtonChecked(m_button_id, isActive);
   m_gui_controller.setTraceFilterEnabled(m_settings.id, !isActive);
   m_is_active = isActive;

   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u Persistent data: background color %.6x, font color %.6x active %u, text %s", m_settings.id, m_settings.background, m_settings.font, isActive, m_settings.regex.c_str());
}
void TraceFilter::onPersistenceWrite(PersistenceItems& buffer)
{
   std::string text = m_gui_controller.getTraceFilter(m_settings.id);

   Utilities::Persistence::writeItem(buffer, "isActive", m_is_active);
   Utilities::Persistence::writeItem(buffer, "text", text);
   Utilities::Persistence::writeItem(buffer, "backgroundColor", m_settings.background);
   Utilities::Persistence::writeItem(buffer, "fontColor", m_settings.font);
   Utilities::Persistence::writeItem(buffer, "userDefined", m_user_defined);
   Utilities::Persistence::writeItem(buffer, "id", m_settings.id);

   UT_Log(TRACE_FILTER, HIGH, "TRACE_FILTER%u Persistent data: background color %.6x, font color %.6x, active %u, text %s", m_settings.id, m_settings.background, m_settings.font, m_is_active, text.c_str());
}

void TraceFilter::onButtonClicked()
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
void TraceFilter::onContextMenuRequested()
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
void TraceFilter::handleNewSettings(const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   m_settings = settings;

   setLineEditColor(m_settings.background, m_settings.font);
   m_gui_controller.setTraceFilter(m_settings.id, m_settings.regex.c_str());
   m_regex = std::regex(settings.regex);
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u %s bg %.6x text %.6x, regex [%s]", m_settings.id, __func__, settings.background, settings.font, settings.regex.c_str());
}
void TraceFilter::setButtonState(bool active)
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
void TraceFilter::setLineEditColor(uint32_t background_color, uint32_t font_color)
{
   UT_Log(TRACE_FILTER, LOW, "TRACE_FILTER%u bg %.6x text %.6x", m_settings.id,  background_color, font_color);
   m_gui_controller.setTraceFilterBackgroundColor(m_settings.id, background_color);
   m_gui_controller.setTraceFilterFontColor(m_settings.id, font_color);
}
bool TraceFilter::filteringActive()
{
   return m_is_active;
}

}
