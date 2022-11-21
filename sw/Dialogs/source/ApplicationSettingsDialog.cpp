#include <array>
#include <string>
#include <QtWidgets/QScrollArea>

#include "ApplicationSettingsDialog.h"
#include "PortSettingDialog.h"
#include "TraceFilterSettingDialog.h"
#include "Logger.h"
#include "Settings.h"
#include <QtWidgets/qabstractitemview.h>

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                                                     std::unique_ptr<IFileLogger>& logger,
                                                     std::string& logging_path,
                                                     IThemeController& theme_controller):
m_handlers(ports),
m_filters(filters),
m_file_logging(logging_path, logger),
m_theme_controller(theme_controller),
m_theme_combobox(nullptr),
m_max_traces_edit(nullptr)
{
}
ApplicationSettingsDialog::~ApplicationSettingsDialog()
{

}
std::optional<bool> ApplicationSettingsDialog::showDialog(QWidget* parent)
{
   std::optional<bool> result;

   QDialog* main_dialog = new QDialog(parent);
   QGridLayout* main_layout = new QGridLayout();
   main_dialog->setPalette(parent->palette());
   QTabWidget* main_tab_view = new QTabWidget();

   createGeneralTab(main_tab_view, parent);
   createPortHandlersTab(main_tab_view, parent);
   createTraceFiltersTab(main_tab_view, parent);
   createFileLoggerTab(main_tab_view);
   createDebugTab(main_tab_view, parent);
   createAboutTab(main_tab_view, parent);

   QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, main_dialog);

   QObject::connect(button_box, SIGNAL(accepted()), main_dialog, SLOT(accept()));
   QObject::connect(button_box, SIGNAL(rejected()), main_dialog, SLOT(reject()));

   UT_Log(GUI_DIALOG, HIGH, "%s", __func__);

   main_layout->addWidget(main_tab_view);
   main_layout->addWidget(button_box);
   main_dialog->setLayout(main_layout);
   if (main_dialog->exec() == QDialog::Accepted)
   {
      notifyPortHandlersChanges();
      notifyTraceFiltersChanges();
      notifyFileLoggingChanges();
      saveLoggerGroups();
      saveSystemSettingsGroup();
      notifyGeneralChanges();
      result = true;
   }

   onDialogClosed();
   delete main_dialog;
   return result;
}
void ApplicationSettingsDialog::createGeneralTab(QTabWidget* main_tab, QWidget* parent)
{
   QWidget* tab_widget = new QWidget();
   QFormLayout* tab_layout = new QFormLayout();
   tab_widget->setLayout(tab_layout);
   /* theme control */
   m_theme_combobox = new QComboBox();
   m_theme_combobox->setPalette(parent->palette());
   m_theme_combobox->view()->setPalette(parent->palette());

   for (uint8_t i = 0; i < (uint8_t)IThemeController::Theme::APPLICATION_THEMES_MAX; i++)
   {
      m_theme_combobox->addItem(QString(m_theme_controller.themeToName((IThemeController::Theme)i).c_str()));
   }
   m_theme_combobox->setCurrentText(QString(m_theme_controller.themeToName(m_theme_controller.currentTheme()).c_str()));
   tab_layout->addRow("Theme", m_theme_combobox);

   /* trace count settings */
   m_max_traces_edit = new QLineEdit();
   m_max_traces_edit->setText(QString::number(SETTING_GET_U32(MainApplication_maxTerminalTraces)));
   tab_layout->addRow("Maximum traces", m_max_traces_edit);

   QLabel* persistence_path = new QLabel();
   persistence_path->setText(QString(PERSISTENCE_PATH));
   tab_layout->addRow("Data persistence", persistence_path);

   QLabel* settings_persistence_path = new QLabel();
   settings_persistence_path->setText(QString(SETTINGS_PERSISTENCE_PATH));
   tab_layout->addRow("Settings persistence", settings_persistence_path);

   QLabel* settings_path = new QLabel();
   settings_path->setText(QString(Settings::SettingsHandler::get()->getFilePath().c_str()));
   tab_layout->addRow("Settings file", settings_path);

   main_tab->addTab(tab_widget, "GENERAL");
}
void ApplicationSettingsDialog::createPortHandlersTab(QTabWidget* main_tab, QWidget* parent)
{
   QTabWidget* ports_tab = new QTabWidget();
   for (uint8_t i = 0; i < m_handlers.port_handlers.size(); i++)
   {
      QWidget* port_widget = new QWidget();
      std::string name = "PORT" + std::to_string(m_handlers.port_handlers[i]->getSettings().port_id);
      port_widget->setLayout(m_handlers.dialogs[i].createLayout(parent, m_handlers.port_handlers[i]->getSettings(), !m_handlers.port_handlers[i]->isOpened()));
      ports_tab->addTab(port_widget, QString(name.c_str()));
   }
   main_tab->addTab(ports_tab, "PORTS");
}
void ApplicationSettingsDialog::createTraceFiltersTab(QTabWidget* main_tab,QWidget* parent)
{
   QTabWidget* filters_tab = new QTabWidget();

   for (uint8_t i = 0; i < m_filters.filters.size(); i++)
   {
      QWidget* trace_widget = new QWidget();
      std::string name = "FILTER" + std::to_string(i+1);
      trace_widget->setLayout(m_filters.dialogs[i].createLayout(parent, m_filters.filters[i]->getSettings(), !m_filters.filters[i]->isActive()));
      filters_tab->addTab(trace_widget, QString(name.c_str()));
   }
   main_tab->addTab(filters_tab, "FILTERS");
}
void ApplicationSettingsDialog::createFileLoggerTab(QTabWidget* main_tab)
{
   QWidget* logging_widget = new QWidget();
   logging_widget->setLayout(m_file_logging.dialog.createLayout(m_file_logging.path, !m_file_logging.logger->isActive()));
   main_tab->addTab(logging_widget, "FILE LOGGING");
}
void ApplicationSettingsDialog::createDebugTab(QTabWidget* main_tab, QWidget* parent)
{
   QTabWidget* debug_tab = new QTabWidget();

   createLoggerTab(debug_tab, parent);
   createSystemSettingsTab(debug_tab, parent);
   main_tab->addTab(debug_tab, "DEBUG");
}
void ApplicationSettingsDialog::createAboutTab(QTabWidget* main_tab, QWidget*)
{
   QLabel* about_label = new QLabel();
   std::string about_text = "UltimateTerminal\nv." + std::string(APPLICATION_VERSION) + '\n' +
                            "Author: Jacek Skowronek\n" + "email:jacekskowronekk@gmail.com";
   about_label->setText(about_text.c_str());
   about_label->setAlignment(Qt::AlignCenter);
   main_tab->addTab(about_label, "ABOUT");
}
void ApplicationSettingsDialog::createLoggerTab(QTabWidget* debug_tab, QWidget* parent)
{
   QWidget* logger_widget = new QWidget();
   QFormLayout* logger_widget_layout = new QFormLayout();
   logger_widget->setLayout(logger_widget_layout);

   for (uint8_t i = 0; i < LOGGER_GROUP_MAX; i++)
   {
      QComboBox* levelbox = new QComboBox(debug_tab);
      levelbox->setPalette(parent->palette());
      levelbox->view()->setPalette(parent->palette());
      for (uint8_t j = 0; j < LOGGER_LEVEL_MAX; j++)
      {
         levelbox->addItem(QString(LoggerEngine::get()->getLevelName((LoggerLevelID)j).c_str()));
      }
      levelbox->setCurrentText(QString(LoggerEngine::get()->getLevelName(LoggerEngine::get()->getLevel((LoggerGroupID)i)).c_str()));
      m_logger_comboboxes.push_back(levelbox);
      logger_widget_layout->addRow(QString(LoggerEngine::get()->getGroupName((LoggerGroupID)i).c_str()), levelbox);
   }
   debug_tab->addTab(logger_widget, "LOGGING");
}
void ApplicationSettingsDialog::createSystemSettingsTab(QTabWidget* debug_tab, QWidget* parent)
{
   QScrollArea* scroll = new QScrollArea();

   QWidget* setting_widget = new QWidget();
   QFormLayout* setting_widget_layout = new QFormLayout();
   setting_widget->setLayout(setting_widget_layout);

   setting_widget_layout->setSizeConstraint(QLayout::SetMaximumSize);

   for (uint8_t i = 0; i < SETTING_GROUP_MAX; i++)
   {
      QLineEdit* setting_value = new QLineEdit(debug_tab);
      setting_value->setPalette(parent->palette());
      std::string name = SETTING_GET_NAME((KeyID)i);
      writeSettingValue(i, setting_value);
      setting_widget_layout->addRow(QString(name.c_str()), setting_value);
      m_setting_lineedits.push_back(setting_value);
   }
   scroll->setWidget(setting_widget);
   debug_tab->addTab(scroll, "SETTINGS");
}
void ApplicationSettingsDialog::writeSettingValue(int id, QLineEdit* edit)
{
   switch (SETTING_GET_TYPE((KeyID)id))
   {
   case Settings::SettingType::BOOL:
   {
      edit->setText(QString::number(SETTING_GET_BOOL((KeyID)id)));
      break;
   }
   case Settings::SettingType::STRING:
   {
      edit->setText(QString(SETTING_GET_STRING((KeyID)id).c_str()));
      break;
   }
   case Settings::SettingType::U32:
   {
      edit->setText(QString::number(SETTING_GET_U32((KeyID)id)));
      break;
   }
   default:
      break;
   }
}
void ApplicationSettingsDialog::notifyPortHandlersChanges()
{
   for (uint8_t i = 0; i < m_handlers.port_handlers.size(); i++)
   {
      Dialogs::PortSettingDialog::Settings new_settings;
      Dialogs::PortSettingDialog::Settings current_settings = m_handlers.port_handlers[i]->getSettings();
      if (m_handlers.dialogs[i].convertGuiValues(new_settings) && (new_settings != current_settings))
      {
         UT_Log(GUI_DIALOG, LOW, "Detected settings change for port %u", current_settings.port_id);
         (void) m_handlers.port_handlers[i]->setSettings(new_settings);
      }
   }
}
void ApplicationSettingsDialog::notifyTraceFiltersChanges()
{
   for (uint8_t i = 0; i < m_filters.filters.size(); i++)
   {
      Dialogs::TraceFilterSettingDialog::Settings new_settings;
      Dialogs::TraceFilterSettingDialog::Settings current_settings = m_filters.filters[i]->getSettings();
      if (m_filters.dialogs[i].convertGuiValues(new_settings) && (new_settings != current_settings))
      {
         UT_Log(GUI_DIALOG, LOW, "Detected trace filter change for filter %u", current_settings.id);
         (void) m_filters.filters[i]->setSettings(new_settings);
      }
   }
}
void ApplicationSettingsDialog::notifyFileLoggingChanges()
{
   std::string new_logging_path = m_file_logging.dialog.convertGuiValues();
   if (new_logging_path != m_file_logging.path)
   {
      UT_Log(GUI_DIALOG, LOW, "Detected logging path change %s -> %s", m_file_logging.path.c_str(), new_logging_path.c_str());
      m_file_logging.path = new_logging_path;
   }
}

void ApplicationSettingsDialog::notifyGeneralChanges()
{
   saveThemeChange();
   bool conversion_ok;
   uint32_t max_trace_count = m_max_traces_edit->text().toUInt(&conversion_ok);
   if (conversion_ok && max_trace_count)
   {
      UT_Log(GUI_DIALOG, LOW, "New maximum number of traces: %u", max_trace_count);
      SETTING_SET_U32(MainApplication_maxTerminalTraces, max_trace_count);
   }
}

void ApplicationSettingsDialog::saveLoggerGroups()
{
   for (uint8_t i = 0; i < m_logger_comboboxes.size(); i++)
   {
      std::string level_name = m_logger_comboboxes[i]->currentText().toStdString();
      LoggerGroupID logger_id = (LoggerGroupID)i;
      LoggerLevelID logger_level = LoggerEngine::get()->levelFromString(level_name);
      LoggerEngine::get()->setLevel(logger_id, logger_level);
      UT_Log(GUI_DIALOG, LOW, "Logger group %s set to %s", LoggerEngine::get()->getGroupName(logger_id).c_str(), level_name.c_str());
   }
}
void ApplicationSettingsDialog::saveThemeChange()
{
   std::string new_theme_text = m_theme_combobox->currentText().toStdString();
   IThemeController::Theme new_theme = m_theme_controller.nameToTheme(new_theme_text);
   IThemeController::Theme current_theme = m_theme_controller.currentTheme();
   if (new_theme != current_theme)
   {
      UT_Log(GUI_DIALOG, LOW, "Theme change detected - %s -> %s", m_theme_controller.themeToName(current_theme).c_str(), new_theme_text.c_str());
      m_theme_controller.reloadTheme(new_theme);
   }
}
void ApplicationSettingsDialog::saveSystemSettingsGroup()
{
   for (uint8_t i = 0; i < m_setting_lineedits.size(); i++)
   {
      switch(SETTING_GET_TYPE((KeyID)i))
      {
      case Settings::SettingType::BOOL:
         saveSystemBoolSetting(i, m_setting_lineedits[i]);
         break;
      case Settings::SettingType::STRING:
         saveSystemStringSetting(i, m_setting_lineedits[i]);
         break;
      case Settings::SettingType::U32:
         saveSystemU32Setting(i, m_setting_lineedits[i]);
         break;
      default:
         break;
      }
   }
}
void ApplicationSettingsDialog::saveSystemU32Setting(int id, QLineEdit* edit)
{
   KeyID setting_id = (KeyID)id;
   bool conversion_ok = false;
   QString text = edit->text();
   uint32_t value = text.toUInt(&conversion_ok);
   if (conversion_ok)
   {
      SETTING_SET_U32(setting_id, value);
      UT_Log(GUI_DIALOG, LOW, "Setting %s to %u", SETTING_GET_NAME(setting_id).c_str(), value);
   }
   else
   {
      UT_Log(GUI_DIALOG, ERROR, "Cannot convert value %s to U32 for setting %s", text.toStdString().c_str(), SETTING_GET_NAME(setting_id).c_str());
   }
}
void ApplicationSettingsDialog::saveSystemBoolSetting(int id, QLineEdit* edit)
{
   KeyID setting_id = (KeyID)id;
   bool conversion_ok = false;
   QString text = edit->text();
   bool value = (bool)text.toUInt(&conversion_ok);
   if (conversion_ok)
   {
      SETTING_SET_BOOL(setting_id, value);
      UT_Log(GUI_DIALOG, ERROR, "Setting %s to %u", SETTING_GET_NAME(setting_id).c_str(), value);
   }
   else
   {
      UT_Log(GUI_DIALOG, ERROR, "Cannot convert value %s to BOOL for setting %s", text.toStdString().c_str(), SETTING_GET_NAME(setting_id).c_str());
   }
}
void ApplicationSettingsDialog::saveSystemStringSetting(int id, QLineEdit* edit)
{
   KeyID setting_id = (KeyID)id;
   std::string value = edit->text().toStdString();
   SETTING_SET_STRING(setting_id, value);
   UT_Log(GUI_DIALOG, ERROR, "Setting %s to %s", SETTING_GET_NAME(setting_id).c_str(), value.c_str());
}

void ApplicationSettingsDialog::onDialogClosed()
{
   m_logger_comboboxes.clear();
   m_setting_lineedits.clear();
   delete m_theme_combobox;
   delete m_max_traces_edit;
}

}
