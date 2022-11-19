#include "QtWidgets/QColorDialog"
#include <QtCore/QString>
#include "TraceFilterSettingDialog.h"
#include "Logger.h"

namespace Dialogs
{

TraceFilterSettingDialog::TraceFilterSettingDialog():
m_parent(nullptr),
m_dialog(nullptr),
m_form(nullptr),
m_buttonBox(nullptr),
m_regexEdit(nullptr),
m_backgroundButton(nullptr),
m_fontButton(nullptr)
{
}
TraceFilterSettingDialog::~TraceFilterSettingDialog()
{
}
std::optional<bool> TraceFilterSettingDialog::showDialog(QWidget* parent, const Settings& current_settings, Settings& out_settings, bool allow_edit)
{
   std::optional<bool> result;
   m_dialog = new QDialog(parent);
   m_dialog->setPalette(parent->palette());
   m_parent = m_dialog;
   std::string window_title = "FILTER" + std::to_string(current_settings.id) + " Settings";
   m_dialog->setWindowTitle(QString(window_title.c_str()));


   m_dialog->setLayout(createLayout(m_dialog, current_settings, allow_edit));
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
QLayout* TraceFilterSettingDialog::createLayout(QWidget* parent, const Settings& current_settings, bool allow_edit)
{
   m_parent = parent;
   m_form = new QFormLayout();
   m_settings = current_settings;

   m_regexEdit = new QLineEdit(m_parent);
   QPalette palette = m_regexEdit->palette();
   palette.setColor(QPalette::Base, QColor(current_settings.background));
   palette.setColor(QPalette::Text, QColor(current_settings.font));
   m_regexEdit->setPalette(palette);
   m_regexEdit->update();
   m_regexEdit->setText(QString(current_settings.regex.c_str()));
   m_regexEdit->setDisabled(!allow_edit);
   m_form->addRow("Regular expression", m_regexEdit);

   m_backgroundButton = new QPushButton(m_parent);
   QPalette background_palette;
   background_palette.setColor(QPalette::Button, QColor(current_settings.background));
   m_backgroundButton->setPalette(background_palette);
   m_backgroundButton->setDisabled(!allow_edit);
   m_form->addRow("Background color", m_backgroundButton);
   QObject::connect(m_backgroundButton, SIGNAL(clicked()), this, SLOT(onBackgroundColorButtonClicked()));

   m_fontButton = new QPushButton(m_parent);
   QPalette font_palette;
   font_palette.setColor(QPalette::Button, QColor(current_settings.font));
   m_fontButton->setPalette(font_palette);
   m_fontButton->setDisabled(!allow_edit);
   m_form->addRow("Font color", m_fontButton);
   QObject::connect(m_fontButton, SIGNAL(clicked()), this, SLOT(onFontColorButtonClicked()));

   return m_form;
}
void TraceFilterSettingDialog::destroyLayout()
{
   if (m_form)
   {
      delete m_form;
      m_form = nullptr;
   }
}
void TraceFilterSettingDialog::addDialogButtons()
{
   m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, m_dialog);
   m_form->addWidget(m_buttonBox);
   QObject::connect(m_buttonBox, SIGNAL(accepted()), m_dialog, SLOT(accept()));
   QObject::connect(m_buttonBox, SIGNAL(rejected()), m_dialog, SLOT(reject()));
}
bool TraceFilterSettingDialog::convertGuiValues(Settings& out_settings)
{
   out_settings.regex = m_regexEdit->text().toStdString();
   out_settings.background = m_backgroundButton->palette().color(QPalette::Button).rgb();
   out_settings.font = m_fontButton->palette().color(QPalette::Button).rgb();
   out_settings.id = m_settings.id;
   UT_Log(GUI_DIALOG, HIGH, "got colors background %.6x, font %.6x, regex %s", out_settings.background, out_settings.font, out_settings.regex.c_str());
   return true;
}
void TraceFilterSettingDialog::onBackgroundColorButtonClicked()
{
   QColor background_color = QColorDialog::getColor(m_backgroundButton->palette().color(QPalette::Button), m_parent, "Select background color");
   if (background_color.isValid())
   {
      QPalette background_palette;
      background_palette.setColor(QPalette::Button, background_color);
      m_backgroundButton->setPalette(background_palette);
   }
}
void TraceFilterSettingDialog::onFontColorButtonClicked()
{
   QColor font_color = QColorDialog::getColor(m_fontButton->palette().color(QPalette::Button), m_parent, "Select font color");
   if (font_color.isValid())
   {
      QPalette font_palette;
      font_palette.setColor(QPalette::Button, font_color);
      m_fontButton->setPalette(font_palette);
   }
}

}
