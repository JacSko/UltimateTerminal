#include <QtCore/QString>
#include "TabNameDialog.h"
#include "Logger.h"
#include <sstream>

namespace Dialogs
{

TabNameDialog::TabNameDialog():
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_tabNameEdit(nullptr)
{
}
TabNameDialog::~TabNameDialog()
{
}
std::optional<std::string> TabNameDialog::showDialog(QWidget* parent, const std::string& current_name)
{
   std::optional<std::string> result;
   m_dialog = new QDialog(parent);
   m_dialog->setPalette(parent->palette());
   m_form = new QFormLayout(m_dialog);

   m_tabNameEdit = new QLineEdit(m_dialog);
   m_tabNameEdit->setText(QString(current_name.c_str()));
   m_form->addRow("Tab name:", m_tabNameEdit);

   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));

   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(GUI_DIALOG, LOW, "%s current name %s", __func__, current_name.c_str());
   if (m_dialog->exec() == QDialog::Accepted)
   {
      result = m_tabNameEdit->text().toStdString();
      UT_Log(GUI_DIALOG, LOW, "%s new name [%s]", __func__, result.value().c_str());
   }
   delete m_form;
   delete m_dialog;
   UT_Log(GUI_DIALOG, LOW, "%s result %s", __func__, result.has_value()? "OK" : "NO_VALUE");

   return result;
}

}
