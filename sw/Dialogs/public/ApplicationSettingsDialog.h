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
#include "IThemeController.h"


/**
 * @file ApplicationSettingsDialog.cpp
 *
 * @brief
 *    Dialog window implementation that allows user to enter the application settings.
 *
 * @details TODO
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
   ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                             std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                             std::unique_ptr<IFileLogger>& logger,
                             std::string& logging_path,
                             IThemeController& theme_controller);
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

   void createGeneralTab(QTabWidget*);
   void createPortHandlersTab(QTabWidget*, QWidget*);
   void createTraceFiltersTab(QTabWidget*, QWidget*);
   void createFileLoggerTab(QTabWidget*);
   void createDebugTab(QTabWidget*, QWidget*);
   void createLoggerTab(QTabWidget*, QWidget*);
   void createSystemSettingsTab(QTabWidget*, QWidget*);
   void writeSettingValue(int id, QLineEdit*);

   void notifyPortHandlersChanges();
   void notifyTraceFiltersChanges();
   void notifyFileLoggingChanges();
   void saveLoggerGroups();
   void saveSystemSettingsGroup();
   void saveSystemU32Setting(int, QLineEdit*);
   void saveSystemBoolSetting(int, QLineEdit*);
   void saveSystemStringSetting(int, QLineEdit*);

   void onDialogClosed();
   PortHandlerItems m_handlers;
   TraceFilterItems m_filters;
   FileLoggingItems m_file_logging;
   IThemeController& m_theme_controller;
   std::vector<QComboBox*> m_logger_comboboxes;
   std::vector<QLineEdit*> m_setting_lineedits;
};

}
