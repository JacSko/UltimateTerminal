#pragma once

#include <stdint.h>
#include <optional>

#include <QtCore/QObject>
#include "PortHandler.h"
#include "TraceFilterHandler.h"
#include "LoggingSettingDialog.h"
#include "IFileLogger.h"
#include "GUIController.h"

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

class ApplicationSettingsDialog
{
public:
   ApplicationSettingsDialog(GUIController& gui_controller,
                             std::vector<std::unique_ptr<GUI::PortHandler>>& ports,
                             std::vector<std::unique_ptr<TraceFilterHandler>>& filters,
                             std::unique_ptr<IFileLogger>& logger,
                             std::string& logging_path);
   ~ApplicationSettingsDialog();

   /**
    * @brief Opens a application settings dialog.
    * @param[in] parent - parent Widget
    *
    * @return Optional has value if user accepted the dialog, has no value if user rejected the dialog.
    * @return Optional value is true, when at least one setting items has changed.
    */
   std::optional<bool> showDialog(QWidget* parent);
};

}
