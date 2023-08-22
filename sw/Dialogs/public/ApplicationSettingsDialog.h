#pragma once

#include <stdint.h>
#include <optional>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>

#include "PortHandler.h"
#include "TraceFilterHandler.h"
#include "LoggingSettingDialog.h"
#include "IFileLogger.h"
#include "GUIController.h"

/**
 * @file ApplicationSettingsDialog.h
 *
 * @brief
 *    Dialog window implementation that allows user to enter the application settings.
 *
 * @details This class allows to show the dialog windows with application settings. Settings are gathered from objects passed via constructor.
 *          When dialog is rejected by user, no settings change takes place.
 *          When dialog is accepted, respective modules are notified with new settings only if these settings has changed.
 *
 * @author Jacek Skowronek
 * @date   18/11/2022
 *
 */

namespace Dialogs
{

class ApplicationSettingsDialog : public QObject
{
   Q_OBJECT

public:
   ApplicationSettingsDialog(GUIController& gui_controller,
                             std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                             std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                             std::unique_ptr<IFileLogger>& logger,
                             std::string& logging_path,
                             const std::string& persistence_path,
                             const std::string& settings_persistence_path);
   ~ApplicationSettingsDialog();

   /**
    * @brief Opens a application settings dialog.
    * @param[in] parent - parent Widget
    *
    * @return Optional has value if user accepted the dialog, has no value if user rejected the dialog.
    * @return Optional value is true, when at least one setting items has changed.
    */
   std::optional<bool> showDialog(QWidget* parent);
private:
   struct PortHandlerItems
   {
      PortHandlerItems(std::vector<std::unique_ptr<GUI::PortHandler>>& handlers):
      port_handlers(handlers),
      dialogs {handlers.size()}
      {
      }
      std::vector<std::unique_ptr<GUI::PortHandler>>& port_handlers;
      std::vector<Dialogs::PortSettingDialog> dialogs;
   };

   struct TraceFilterItems
   {
      TraceFilterItems(std::vector<std::unique_ptr<TraceFilterHandler>>& handlers):
      filters(handlers),
      dialogs {handlers.size()}
      {
      }
      std::vector<std::unique_ptr<TraceFilterHandler>>& filters;
      std::vector<Dialogs::TraceFilterSettingDialog> dialogs;
   };

   struct FileLoggingItems
   {
      FileLoggingItems(std::string& path, std::unique_ptr<IFileLogger>& logger):
      path(path),
      dialog (),
      logger(logger)
      {
      }
      std::string& path;
      Dialogs::LoggingSettingDialog dialog;
      std::unique_ptr<IFileLogger>& logger;
   };

   void createGeneralTab(QTabWidget*, QWidget* parent);
   void createPortHandlersTab(QTabWidget*, QWidget*);
   void createTraceFiltersTab(QTabWidget*, QWidget*);
   void createFileLoggerTab(QTabWidget*);
   void createDebugTab(QTabWidget*, QWidget*);
   void createAboutTab(QTabWidget*, QWidget*);
   void createLoggerTab(QTabWidget*, QWidget*);
   void createSystemSettingsTab(QTabWidget*, QWidget*);
   void writeSettingValue(int id, QLineEdit*);

   void notifyPortHandlersChanges();
   void notifyTraceFiltersChanges();
   void notifyFileLoggingChanges();
   void notifyGeneralChanges();
   void saveLoggerGroups();
   void saveThemeChange();
   void saveSystemSettingsGroup();
   void saveSystemU32Setting(int, QLineEdit*);
   void saveSystemBoolSetting(int, QLineEdit*);
   void saveSystemStringSetting(int, QLineEdit*);

   void onDialogClosed();
   PortHandlerItems m_handlers;
   TraceFilterItems m_filters;
   FileLoggingItems m_file_logging;
   GUIController& m_gui_controller;
   std::vector<QComboBox*> m_logger_comboboxes;
   std::vector<QLineEdit*> m_setting_lineedits;
   QComboBox* m_theme_combobox;
   QLineEdit* m_max_traces_edit;
   std::string m_persistence_path;
   std::string m_settings_persistence_path;

};

}
