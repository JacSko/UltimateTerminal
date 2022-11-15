#include "QtWidgets/QColorDialog"
#include <QtCore/QString>
#include "TraceFilterSettingDialog.h"
#include "Logger.h"

namespace Dialogs
{

TraceFilterSettingDialog::TraceFilterSettingDialog():
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_backgroundButton(nullptr),
m_fontButton(nullptr)
{
}
TraceFilterSettingDialog::~TraceFilterSettingDialog()
{
}
std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const ColorSet& current_settings, ColorSet& out_settings)
{
   std::optional<bool> result;
   m_dialog = new QDialog(parent);
   m_dialog->setPalette(parent->palette());
   m_form = new QFormLayout(m_dialog);

   m_backgroundButton = new QPushButton(m_dialog);
   QPalette background_palette;
   background_palette.setColor(QPalette::Button, QColor(current_settings.background));
   m_backgroundButton->setPalette(background_palette);
   m_form->addRow("Background color", m_backgroundButton);
   QObject::connect(m_backgroundButton, SIGNAL(clicked()), this, SLOT(onBackgroundColorButtonClicked()));

   m_fontButton = new QPushButton(m_dialog);
   QPalette font_palette;
   font_palette.setColor(QPalette::Button, QColor(current_settings.font));
   m_fontButton->setPalette(font_palette);
   m_form->addRow("Font color", m_fontButton);
   QObject::connect(m_fontButton, SIGNAL(clicked()), this, SLOT(onFontColorButtonClicked()));

   addDialogButtons();

   m_dialog->setWindowModality(Qt::ApplicationModal);
   UT_Log(GUI_DIALOG, INFO, "Trace filter dialog opened");
   if (m_dialog->exec() == QDialog::Accepted)
   {
      UT_Log(GUI_DIALOG, LOW, "dialog accepted, gathering new settings");
      result = convertGuiValues(out_settings);
   }

   delete m_form;
   delete m_dialog;
   UT_Log(GUI_DIALOG, INFO, "%s result %s", __func__, result.has_value()? (result.value()? "OK" : "NOK") : "NO_VALUE");

   return result;
}
void TraceFilterSettingDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
}
bool TraceFilterSettingDialog::convertGuiValues(ColorSet& out_settings)
{
   out_settings.background = m_backgroundButton->palette().color(QPalette::Button).rgb();
   out_settings.font = m_fontButton->palette().color(QPalette::Button).rgb();
   UT_Log(GUI_DIALOG, HIGH, "got colors background %.6x, font %.6x", out_settings.background, out_settings.font);
   return true;
}
void TraceFilterSettingDialog::onBackgroundColorButtonClicked()
{
   QColor background_color = QColorDialog::getColor(m_backgroundButton->palette().color(QPalette::Button), m_dialog, "Select background color");
   if (background_color.isValid())
   {
      QPalette background_palette;
      background_palette.setColor(QPalette::Button, background_color);
      m_backgroundButton->setPalette(background_palette);
   }
}
void TraceFilterSettingDialog::onFontColorButtonClicked()
{
   QColor font_color = QColorDialog::getColor(m_fontButton->palette().color(QPalette::Button), m_dialog, "Select font color");
   if (font_color.isValid())
   {
      QPalette font_palette;
      font_palette.setColor(QPalette::Button, font_color);
      m_fontButton->setPalette(font_palette);
   }
}

}
