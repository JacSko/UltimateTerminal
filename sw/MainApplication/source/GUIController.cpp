#include "GUIController.h"
#include <QtCore/QVector>

#undef APPLICATION_THEME
#define APPLICATION_THEME(name) #name,
std::array<std::string, (uint32_t)GUIController::Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME


std::unique_ptr<IGUIController> IGUIController::create(QWidget *parent)
{
   return std::unique_ptr<IGUIController>(new GUIController(parent));
}

GUIController::GUIController(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_current_theme(IGUIController::Theme::DEFAULT),
m_terminal_view_status{},
m_trace_view_status{},
m_current_command(nullptr)
{
   ui->setupUi(this);

   if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Dark_Theme_ID))
   {
      ui->loadTheme(GUIController::Theme::DARK);
      m_current_theme = GUIController::Theme::DARK;
   }
   else if (SETTING_GET_U32(GUI_Theme_ID) == SETTING_GET_U32(GUI_Default_Theme_ID))
   {
      ui->loadTheme(GUIController::Theme::DEFAULT);
      m_current_theme = GUIController::Theme::DEFAULT;
   }
   this->setWindowTitle("UltimateTerminal");
   ui->infoLabel->setText(QString().asprintf("UltimateTerminal v%s", std::string(APPLICATION_VERSION).c_str()));

   auto buttons = ui->getButtons();
   for (QPushButton* button : buttons)
   {
      button->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
      connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
      connect(button, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   }

   connect(ui->markerButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->loggingButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->clearButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentPortSelectionChanged(int)));
   connect(ui->switchSendPortShortcut, SIGNAL(activated()), this, SLOT(onPortSwitchRequest()));
   connect(ui->textEdit->lineEdit(), SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceClearButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port1ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port2ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port3ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port4ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port5ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   ui->portLabel_1->setAutoFillBackground(true);
   ui->portLabel_2->setAutoFillBackground(true);
   ui->portLabel_3->setAutoFillBackground(true);
   ui->portLabel_4->setAutoFillBackground(true);
   ui->portLabel_5->setAutoFillBackground(true);
   ui->portLabel_1->setAlignment(Qt::AlignCenter);
   ui->portLabel_2->setAlignment(Qt::AlignCenter);
   ui->portLabel_3->setAlignment(Qt::AlignCenter);
   ui->portLabel_4->setAlignment(Qt::AlignCenter);
   ui->portLabel_5->setAlignment(Qt::AlignCenter);

   connect(ui->traceFilter_1, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_2, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_3, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_4, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_5, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_6, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_7, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   connect(ui->traceFilter_1, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_2, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_3, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_4, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_5, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_6, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   connect(ui->traceFilter_7, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));

   connect(this, SIGNAL(setButtonBackgroundColorSignal(uint32_t, uint32_t)), this, SLOT(onButtonBackgroundColorSignal(uint32_t, uint32_t)));
   connect(this, SIGNAL(setButtonFontColorSignal(uint32_t, uint32_t)), this, SLOT(onButtonFontColorSignal(uint32_t, uint32_t)));
   connect(this, SIGNAL(setButtonCheckableSignal(uint32_t, bool)), this, SLOT(onButtonCheckableSignal(uint32_t, bool)));
   connect(this, SIGNAL(setButtonCheckedSignal(uint32_t, bool)), this, SLOT(onButtonCheckedSignal(uint32_t, bool)));
   connect(this, SIGNAL(setButtonEnabledSignal(uint32_t, bool)), this, SLOT(onButtonEnabledSignal(uint32_t, bool)));
   connect(this, SIGNAL(setButtonTextSignal(uint32_t, QString)), this, SLOT(onButtonTextSignal(uint32_t, QString)));
   connect(this, SIGNAL(clearTerminalViewSignal()), this, SLOT(onClearTerminalViewSignal()));
   connect(this, SIGNAL(clearTraceViewSignal()), this, SLOT(onClearTraceViewSignal()));
   connect(this, SIGNAL(addToTerminalViewSignal(QString, uint32_t, uint32_t)), this, SLOT(onAddToTerminalViewSignal(QString, uint32_t, uint32_t)));
   connect(this, SIGNAL(addToTraceViewSignal(QString, uint32_t, uint32_t)), this, SLOT(onAddToTraceViewSignal(QString, uint32_t, uint32_t)));
   connect(this, SIGNAL(setTerminalScrollingEnabledSignal(bool)), this, SLOT(onSetTerminalScrollingEnabledSignal(bool)));
   connect(this, SIGNAL(setTraceScrollingEnabledSignal(bool)), this, SLOT(onSetTraceScrollingEnabledSignal(bool)));
   connect(this, SIGNAL(registerPortOpenedSignal(QString)), this, SLOT(onRegisterPortOpenedSignal(QString)));
   connect(this, SIGNAL(registerPortClosedSignal(QString)), this, SLOT(onRegisterPortClosedSignal(QString)));
   connect(this, SIGNAL(setCommandHistorySignal(QVector<QString>)), this, SLOT(onsetCommandHistorySignal(QVector<QString>)));
   connect(this, SIGNAL(addCommandToHistorySignal(QString)), this, SLOT(onAddCommandToHistorySignal(QString)));
   connect(this, SIGNAL(addLineEndingSignal(QString)), this, SLOT(onAddLineEndingSignal(QString)));
   connect(this, SIGNAL(setCurrentLineEndingSignal(QString)), this, SLOT(onSetCurrentLineEndingSignal(QString)));
   connect(this, SIGNAL(setTraceFilterSignal(uint8_t, QString)), this, SLOT(onSetTraceFilterSignal(uint8_t, QString)));
   connect(this, SIGNAL(setTraceFilterEnabledSignal(uint8_t, bool)), this, SLOT(onSetTraceFilterEnabledSignal(uint8_t, bool)));
   connect(this, SIGNAL(setTraceFilterBackgroundColorSignal(uint32_t, uint32_t)), this, SLOT(onSetTraceFilterBackgroundColorSignal(uint32_t, uint32_t)));
   connect(this, SIGNAL(setTraceFilterFontColorSignal(uint32_t, uint32_t)), this, SLOT(onSetTraceFilterFontColorSignal(uint32_t, uint32_t)));
   connect(this, SIGNAL(setPortLabelTextSignal(uint8_t, QString)), this, SLOT(onSetPortLabelTextSignal(uint8_t, QString)));
   connect(this, SIGNAL(setPortLabelStylesheetSignal(uint8_t, QString)), this, SLOT(onSetPortLabelStylesheetSignal(uint8_t, QString)));
   connect(this, SIGNAL(reloadThemeSignal(uint8_t)), this, SLOT(onReloadThemeSignal(uint8_t)));


   connect(this, SIGNAL(guiRequestSignal()), this, SLOT(onGuiRequestSignal()));
}
GUIController::~GUIController()
{
   delete ui;
}
uint32_t GUIController::getButtonID(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   auto buttons = ui->getButtons();
   for (uint32_t i = 0; i < buttons.size(); i++)
   {
      UT_Assert(buttons[i]);
      if (buttons[i]->objectName() == name)
      {
         result = i;
         break;
      }
   }
   return result;
}
uint32_t GUIController::getLabelID(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   auto& labels = ui->getLabels();
   for (uint32_t i = 0; i < labels.size(); i++)
   {
      UT_Assert(labels[i]);
      if (labels[i]->objectName() == name)
      {
         result = i;
         break;
      }
   }
   return result;
}
void GUIController::subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) && (item.event == event);});
   if (it != m_button_listeners.end())
   {
      UT_Log(MAIN_GUI, ERROR, "event %u for buttonID %u already registered, replacing!");
      *it = ButtonEventItem{button_id, event, listener};
   }
   else
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u registered!", (uint32_t)event, button_id);
      m_button_listeners.push_back(ButtonEventItem{button_id, event, listener});
   }
}
void GUIController::unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) &&
                                                                                                                        (item.event == event) &&
                                                                                                                        (item.listener == listener);});
   UT_Log_If(it == m_button_listeners.end(), MAIN_GUI, ERROR, "event %u for buttonID %u not found!");
   if (it != m_button_listeners.end())
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u unregistered", (uint32_t)event, button_id);
      m_button_listeners.erase(it);
   }
}
void GUIController::setButtonBackgroundColor(uint32_t button_id, uint32_t color)
{
   emit setButtonBackgroundColorSignal(button_id, color);
}
void GUIController::setButtonFontColor(uint32_t button_id, uint32_t color)
{
   emit setButtonFontColorSignal(button_id, color);
}
void GUIController::setButtonCheckable(uint32_t button_id, bool checkable)
{
   emit setButtonCheckableSignal(button_id, checkable);
}
void GUIController::setButtonChecked(uint32_t button_id, bool checked)
{
   emit setButtonCheckedSignal(button_id, checked);
}
void GUIController::setButtonEnabled(uint32_t button_id, bool enabled)
{
   emit setButtonEnabledSignal(button_id, enabled);
}
void GUIController::setButtonText(uint32_t button_id, const std::string& text)
{
   emit setButtonTextSignal(button_id, QString(text.c_str()));
}
void GUIController::clearTerminalView()
{
   emit clearTerminalViewSignal();
}
void GUIController::clearTraceView()
{
   emit clearTraceViewSignal();
}
void GUIController::addToTerminalView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   emit addToTerminalViewSignal(QString(text.c_str()), background_color, font_color);
}
void GUIController::addToTraceView(const std::string& text, uint32_t background_color, uint32_t font_color)
{
   emit addToTraceViewSignal(QString(text.c_str()), background_color, font_color);
}
uint32_t GUIController::countTerminalItems()
{
   return m_terminal_view_status.item_count;
}
uint32_t GUIController::countTraceItems()
{
   return m_trace_view_status.item_count;
}
void GUIController::setTerminalScrollingEnabled(bool enabled)
{
   emit setTerminalScrollingEnabledSignal(enabled);
}
void GUIController::setTraceScrollingEnabled(bool enabled)
{
   emit setTraceScrollingEnabledSignal(enabled);
}
void GUIController::subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback)
{
   UT_Log(MAIN_GUI, HIGH, "%s", __func__);
   m_active_port_listener = std::move(callback);
}
void GUIController::registerPortOpened(const std::string& port_name)
{
   emit registerPortOpenedSignal(QString(port_name.c_str()));
}
void GUIController::registerPortClosed(const std::string& port_name)
{
   emit registerPortClosedSignal(QString(port_name.c_str()));
}
void GUIController::setCommandsHistory(const std::vector<std::string>& history)
{
   QVector<QString> commands;
   for (const std::string& command : history)
   {
      commands.push_back(QString(command.c_str()));
   }
   emit setCommandHistorySignal(commands);
}
void GUIController::addCommandToHistory(const std::string& history)
{
   emit addCommandToHistorySignal(QString(history.c_str()));
}
std::string GUIController::getCurrentCommand()
{
   GetCommandRequest request;
   executeGUIRequest(&request);
   return request.command;
}
std::string GUIController::getCurrentLineEnding()
{
   GetLineEndingRequest request;
   executeGUIRequest(&request);
   return request.line_ending;
}
void GUIController::addLineEnding(const std::string& ending)
{
   emit addLineEndingSignal(QString(ending.c_str()));
}
void GUIController::setCurrentLineEnding(const std::string& ending)
{
   emit setCurrentLineEndingSignal(QString(ending.c_str()));
}
uint32_t GUIController::getTraceFilterID(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   auto filters = ui->getTraceFilters();
   for (uint32_t i = 0; i < filters.size(); i++)
   {
      UT_Assert(filters[i].button);
      UT_Assert(filters[i].line_edit);
      if ((filters[i].button->objectName() == name) ||
          (filters[i].line_edit->objectName() == name))
      {
         result = i;
         break;
      }
   }
   return result;
}
void GUIController::setTraceFilter(uint8_t id, const std::string& filter)
{
   emit setTraceFilterSignal(id, QString(filter.c_str()));
}
std::string GUIController::getTraceFilter(uint8_t id)
{
   GetTraceFilterRequest request(id);
   executeGUIRequest(&request);
   return request.trace_filter;
}
void GUIController::setTraceFilterEnabled(uint8_t id, bool enabled)
{
   emit setTraceFilterEnabledSignal(id, enabled);
}
void GUIController::setTraceFilterBackgroundColor(uint32_t id, uint32_t color)
{
   emit setTraceFilterBackgroundColorSignal(id, color);
}
void GUIController::setTraceFilterFontColor(uint32_t id, uint32_t color)
{
   emit setTraceFilterFontColorSignal(id, color);
}
void GUIController::setPortLabelText(uint8_t id, const std::string& description)
{
   emit setPortLabelTextSignal(id, QString(description.c_str()));
}
void GUIController::setPortLabelStylesheet(uint8_t id, const std::string& stylesheet)
{
   emit setPortLabelStylesheetSignal(id, QString(stylesheet.c_str()));
}
void GUIController::reloadTheme(IGUIController::Theme theme)
{
   emit reloadThemeSignal((uint8_t)theme);
}
GUIController::Theme GUIController::currentTheme()
{
   return ui->theme;
}
std::string GUIController::themeToName(IGUIController::Theme theme)
{
   UT_Assert(theme < IGUIController::Theme::APPLICATION_THEMES_MAX);
   return m_themes_names[(uint32_t)theme];
}
IGUIController::Theme GUIController::nameToTheme(const std::string& name)
{
   IGUIController::Theme result = IGUIController::Theme::APPLICATION_THEMES_MAX;
   auto it = std::find(m_themes_names.begin(), m_themes_names.end(), name);
   if (it != m_themes_names.end())
   {
      result = (IGUIController::Theme)(std::distance(m_themes_names.begin(), it));
   }
   return result;
}
uint32_t GUIController::getBackgroundColor()
{
   return ui->mainWindow->palette().color(QPalette::Window).rgb();
}
uint32_t GUIController::getTerminalBackgroundColor()
{
   return ui->mainWindow->palette().color(QPalette::Base).rgb();
}
uint32_t GUIController::getTextColor()
{
   return ui->mainWindow->palette().color(QPalette::Text).rgb();
}
QPalette GUIController::getApplicationPalette()
{
   return ui->mainWindow->palette();
}
void GUIController::subscribeForThemeReloadEvent(ThemeListener* listener)
{
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   auto it = std::find_if(m_theme_listeners.begin(), m_theme_listeners.end(), [&](ThemeListener* theme_listener){return theme_listener == listener;});
   if (it == m_theme_listeners.end())
   {
      UT_Log(MAIN_GUI, ERROR, "new theme listener registered!");
      m_theme_listeners.push_back(listener);
   }
   else
   {
      UT_Log(MAIN_GUI, ERROR, "theme listener already registered!");
   }
}
void GUIController::unsubscribeFromThemeReloadEvent(ThemeListener* listener)
{
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   auto it = std::find_if(m_theme_listeners.begin(), m_theme_listeners.end(), [&](ThemeListener* theme_listener){return (theme_listener == listener);});
   UT_Log_If(it == m_theme_listeners.end(), MAIN_GUI, ERROR, "Theme listener not found!");
   if (it != m_theme_listeners.end())
   {
      UT_Log(MAIN_GUI, MEDIUM, "event %u for buttonID %u unregistered");
      m_theme_listeners.erase(it);
   }
}
bool GUIController::executeGUIRequest(CommandExecutor* request)
{
   std::lock_guard<std::mutex> lock(m_command_mutex);
   bool result = true;
   m_current_command = request;

   emit guiRequestSignal();
   if (!request->waitReady(500))
   {
      UT_Log(MAIN_GUI, ERROR, "%s timeout", __func__);
      result = false;
   }
   m_current_command = nullptr;

   return result;
}
void GUIController::onButtonClicked()
{
   QPushButton* object = reinterpret_cast<QPushButton*>(sender());
   auto& buttons = ui->getButtons();
   auto it = std::find(buttons.begin(), buttons.end(), object);
   if (it != buttons.end())
   {
      uint32_t id = std::distance(buttons.begin(), it);
      for (auto& listener : m_button_listeners)
      {
         if (listener.id == id && listener.event == ButtonEvent::CLICKED && listener.listener)
         {
            listener.listener->onButtonEvent(id, ButtonEvent::CLICKED);
         }
      }
   }
}
void GUIController::onButtonContextMenuRequested()
{
   QPushButton* object = reinterpret_cast<QPushButton*>(sender());
   auto& buttons = ui->getButtons();
   auto it = std::find(buttons.begin(), buttons.end(), object);
   if (it != buttons.end())
   {
      uint32_t id = std::distance(buttons.begin(), it);
      for (auto& listener : m_button_listeners)
      {
         if (listener.id == id && listener.event == ButtonEvent::CONTEXT_MENU_REQUESTED && listener.listener)
         {
            listener.listener->onButtonEvent(id, ButtonEvent::CONTEXT_MENU_REQUESTED);
         }
      }
   }
}
void GUIController::onCurrentPortSelectionChanged(int index)
{
   std::string port_name = ui->portComboBox->itemText(index).toStdString();
   UT_Log(MAIN_GUI, LOW, "Selected port changed, idx %u, name %s", (uint8_t) index, port_name.c_str());
   if (m_active_port_listener)
   {
      bool result = m_active_port_listener(port_name);
      UT_Log(MAIN_GUI, LOW, "%s - Event %s consumed", __func__, result? "" : "not");
   }
}
void GUIController::onPortSwitchRequest()
{
   int element_count = ui->portComboBox->count();
   if (element_count > 1)
   {
      int idx = ui->portComboBox->currentIndex();
      if (idx == (element_count - 1))
      {
         ui->portComboBox->setCurrentIndex(0);
      }
      else
      {
         ui->portComboBox->setCurrentIndex(idx+1);
      }
   }
}
void GUIController::onButtonBackgroundColorSignal(uint32_t id, uint32_t color)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u color %.6x", __func__, id, color);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   QPalette palette = buttons[id]->palette();
   palette.setColor(QPalette::Button, color);
   buttons[id]->setPalette(palette);
   buttons[id]->update();
}
void GUIController::onButtonFontColorSignal(uint32_t id, uint32_t color)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u color %.6x", __func__, id, color);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   QPalette palette = buttons[id]->palette();
   palette.setColor(QPalette::ButtonText, color);
   buttons[id]->setPalette(palette);
   buttons[id]->update();
}
void GUIController::onButtonCheckableSignal(uint32_t id, bool checkable)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u checkable %u", __func__, id, checkable);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   buttons[id]->setCheckable(checkable);
}
void GUIController::onButtonCheckedSignal(uint32_t id, bool checked)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u checked %u", __func__, id, checked);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   buttons[id]->setChecked(checked);
}
void GUIController::onButtonEnabledSignal(uint32_t id, bool enabled)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u enabled %u", __func__, id, enabled);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   buttons[id]->setEnabled(enabled);
}
void GUIController::onButtonTextSignal(uint32_t id, QString text)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u text %s", __func__, id, text.toStdString().c_str());
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > id);
   buttons[id]->setText(text);
}
void GUIController::onClearTerminalViewSignal()
{
   UT_Log(MAIN_GUI, HIGH, "%s", __func__);
   ui->terminalView->clear();
   m_terminal_view_status.item_count = 0;
}
void GUIController::onClearTraceViewSignal()
{
   UT_Log(MAIN_GUI, HIGH, "%s", __func__);
   ui->traceView->clear();
   m_trace_view_status.item_count = 0;
}
void GUIController::onAddToTerminalViewSignal(QString text, uint32_t background_color, uint32_t font_color)
{
   UT_Log(MAIN_GUI, HIGH, "%s bg color %.6x font %.6x", __func__, background_color, font_color);
   QListWidgetItem* item = new QListWidgetItem();
   item->setText(text);
   item->setBackground(QColor(background_color));
   item->setForeground(QColor(font_color));
   ui->terminalView->addItem(item);
   m_terminal_view_status.item_count++;
   //TODO do not scroll on each line - it is CPU consuming. Can be replaced with e.g 100ms timer
   if (m_terminal_view_status.scrolling_enabled)
   {
      ui->terminalView->scrollToBottom();
   }
}
void GUIController::onAddToTraceViewSignal(QString text, uint32_t background_color, uint32_t font_color)
{
   UT_Log(MAIN_GUI, HIGH, "%s bg color %.6x font %.6x", __func__, background_color, font_color);
   QListWidgetItem* item = new QListWidgetItem();
   item->setText(text);
   item->setBackground(QColor(background_color));
   item->setForeground(QColor(font_color));
   ui->traceView->addItem(item);
   m_trace_view_status.item_count++;
   //TODO do not scroll on each line - it is CPU consuming. Can be replaced with e.g 100ms timer
   if (m_trace_view_status.scrolling_enabled)
   {
      ui->traceView->scrollToBottom();
   }
}
void GUIController::onSetTerminalScrollingEnabledSignal(bool enabled)
{
   UT_Log(MAIN_GUI, HIGH, "%s %u", __func__, enabled);
   m_terminal_view_status.scrolling_enabled = enabled;
}
void GUIController::onSetTraceScrollingEnabledSignal(bool enabled)
{
   UT_Log(MAIN_GUI, HIGH, "%s %u", __func__, enabled);
   m_trace_view_status.scrolling_enabled = enabled;
}
void GUIController::onRegisterPortOpenedSignal(QString name)
{
   UT_Log(MAIN_GUI, HIGH, "%s %s", __func__, name.toStdString().c_str());
   ui->portComboBox->addItem(name);
}
void GUIController::onRegisterPortClosedSignal(QString name)
{
   UT_Log(MAIN_GUI, HIGH, "%s %s", __func__, name.toStdString().c_str());
   int index = ui->portComboBox->findText(name);
   if (index != -1)
   {
      ui->portComboBox->removeItem(index);
   }
   UT_Log_If(index == -1, MAIN_GUI, ERROR, "%s %s not found", __func__, name.toStdString().c_str());
}
void GUIController::onSetCommandHistorySignal(QVector<QString> history)
{
   UT_Log(MAIN_GUI, HIGH, "%s size %u", __func__, history.size());
   ui->textEdit->clear();
   for (const QString& command : history)
   {
      ui->textEdit->insertItem(0, command);
   }
}
void GUIController::onAddCommandToHistorySignal(QString command)
{
   ui->textEdit->insertItem(0, command);
}
void GUIController::onGuiRequestSignal()
{
   UT_Assert(m_current_command && "Create m_current_command first!");
   m_current_command->execute(ui);
}
void GUIController::onAddLineEndingSignal(QString ending)
{
   UT_Log(MAIN_GUI, HIGH, "%s", __func__);
   ui->lineEndingComboBox->addItem(ending);
}
void GUIController::onSetCurrentLineEndingSignal(QString ending)
{
   UT_Log(MAIN_GUI, HIGH, "%s", __func__);
   ui->lineEndingComboBox->setCurrentText(ending);
}
void GUIController::onSetTraceFilterSignal(uint8_t id, QString filter)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u filter %s", __func__, id, filter.toStdString().c_str());
   auto filters = ui->getTraceFilters();
   UT_Assert(filters.size() > id);
   filters[id].line_edit->setText(filter);
}
void GUIController::onSetTraceFilterEnabledSignal(uint8_t id, bool enabled)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u state %u", __func__, id, enabled);
   auto filters = ui->getTraceFilters();
   UT_Assert(filters.size() > id);
   filters[id].line_edit->setEnabled(enabled);
}
void GUIController::onSetTraceFilterBackgroundColorSignal(uint32_t id, uint32_t color)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u color %u", __func__, id, color);
   auto& filters = ui->getTraceFilters();
   UT_Assert(id < filters.size());
   char stylesheet [300];
   std::snprintf(stylesheet, 300, "background-color: #%.6x;", color);
   filters[id].line_edit->setStyleSheet(QString(stylesheet));
}
void GUIController::onSetTraceFilterFontColorSignal(uint32_t id, uint32_t color)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u color %u", __func__, id, color);
   auto& filters = ui->getTraceFilters();
   UT_Assert(id < filters.size());
   char stylesheet [300];
   std::snprintf(stylesheet, 300, "color: #%.6x;", color);
   filters[id].line_edit->setStyleSheet(QString(stylesheet));
}
void GUIController::onSetPortLabelTextSignal(uint8_t id, QString description)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u text %u", __func__, id, description.toStdString().c_str());
   auto& labels = ui->getLabels();
   UT_Assert(id < labels.size());
   labels[id]->setText(description);
}
void GUIController::onSetPortLabelStylesheetSignal(uint8_t id, QString stylesheet)
{
   UT_Log(MAIN_GUI, HIGH, "%s id %u stylesheet %u", __func__, id, stylesheet.toStdString().c_str());
   auto& labels = ui->getLabels();
   UT_Assert(id < labels.size());
   labels[id]->setStyleSheet(stylesheet);
}
void GUIController::onReloadThemeSignal(uint8_t id)
{
   UT_Assert((GUIController::Theme)id < GUIController::Theme::APPLICATION_THEMES_MAX);
   UT_Log(MAIN_GUI, HIGH, "%s id %u name %s", __func__, id, themeToName((GUIController::Theme)id).c_str());
   ui->loadTheme((GUIController::Theme)id);
   for (auto& listener : m_theme_listeners)
   {
      if (listener) listener->onThemeChange((GUIController::Theme)id);
   }
}
