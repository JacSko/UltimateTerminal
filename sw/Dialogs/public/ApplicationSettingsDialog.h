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

#include "PortHandler.h"
#include "TraceFilterHandler.h"
#include "IFileLogger.h"


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
   ApplicationSettingsDialog(std::vector<std::unique_ptr<GUI::PortHandler>>& ports, std::vector<std::unique_ptr<TraceFilterHandler>>& filters, std::unique_ptr<IFileLogger>& logger);
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
   std::vector<std::unique_ptr<GUI::PortHandler>>& m_port_handlers;
   std::vector<std::unique_ptr<TraceFilterHandler>>& m_trace_filters;
   std::unique_ptr<IFileLogger>& m_file_logger;
};

}
