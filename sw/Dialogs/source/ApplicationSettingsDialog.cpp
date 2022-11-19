#include <array>

#include "ApplicationSettingsDialog.h"
#include "PortSettingDialog.h"
#include "TraceFilterSettingDialog.h"
#include "Logger.h"

namespace Dialogs
{

ApplicationSettingsDialog::ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                                                     std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                                                     std::unique_ptr<IFileLogger>& logger,
                                                     std::string& logging_path):
m_handlers(ports),
m_filters(filters),
m_file_logging(logging_path, logger)
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

   createGeneralTab(main_tab_view);
   createPortHandlersTab(main_tab_view, parent);
   createTraceFiltersTab(main_tab_view, parent);
   createFileLoggerTab(main_tab_view);
   createDebugTab(main_tab_view);

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
      result = true;
   }
   delete main_dialog;
   return result;
}
void ApplicationSettingsDialog::createGeneralTab(QTabWidget* main_tab)
{
   //TODO
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
void ApplicationSettingsDialog::createDebugTab(QTabWidget* main_tab)
{
   //TODO but support in logger engine missing
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


}
