#pragma once

#include <stdint.h>
#include <optional>
#include <vector>

#include <QtCore/QObject>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTextEdit>


class UserButtonDialog : public QObject
{
   Q_OBJECT

public:
   UserButtonDialog();
   ~UserButtonDialog();

   struct Settings
   {
      std::string button_name;
      std::string raw_commands;
   };

   std::optional<bool> showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit);
private:
   QDialog* m_dialog;
   QFormLayout* m_form;
   QDialogButtonBox* m_buttonBox;
   QLineEdit* m_buttonNameEdit;
   QTextEdit* m_commandEdit;
   bool m_editable;

   void addDialogButtons();
   bool convertGuiValues(Settings& out_settings);
};
