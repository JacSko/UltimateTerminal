#include "GUIController.h"
#include <QtCore/QVector>

#undef APPLICATION_THEME
#define APPLICATION_THEME(theme_name) #theme_name,
std::array<std::string, (uint32_t)Theme::APPLICATION_THEMES_MAX> m_themes_names = { APPLICATION_THEMES };
#undef APPLICATION_THEME


GUIController::GUIController(QWidget *parent):
QMainWindow(parent),
ui(new Ui::MainWindow),
m_current_theme(Theme::DEFAULT),
m_terminal_view_status{},
m_trace_view_status{},
m_current_command(nullptr)
{
}
GUIController::~GUIController()
{
   delete ui;
}
void GUIController::run()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->setupUi(this);

   for (QPushButton* button : ui->getButtons())
   {
      button->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
      connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
      connect(button, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
   }

   for (auto& port : ui->getPorts())
   {
      port.label->setAutoFillBackground(true);
      port.label->setAlignment(Qt::AlignCenter);
      port.throughputLabel->setAutoFillBackground(true);
      port.throughputLabel->setAlignment(Qt::AlignCenter);
   }

   for (auto& filter : ui->getTraceFilters())
   {
      filter.line_edit->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
      filter.button->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
      connect(filter.line_edit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onButtonContextMenuRequested()));
      connect(filter.line_edit, SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
      m_shortcuts_map[filter.line_edit] = filter.button;
   }

   connect(ui->textEdit->lineEdit(), SIGNAL(returnPressed()), this, SLOT(onButtonClicked()));
   m_shortcuts_map[ui->textEdit->lineEdit()] = ui->sendButton;
   connect(ui->markerButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->loggingButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->clearButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->portComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentPortSelectionChanged(int)));
   connect(ui->switchSendPortShortcut, SIGNAL(activated()), this, SLOT(onPortSwitchRequest()));
   connect(ui->traceClearButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port0ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port1ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port2ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port3ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(ui->port4ButtonShortcut, SIGNAL(activated()), this, SLOT(onButtonClicked()));
   connect(this, SIGNAL(setButtonBackgroundColorSignal(qint32, qint32)), this, SLOT(onButtonBackgroundColorSignal(qint32, qint32)));
   connect(this, SIGNAL(setButtonFontColorSignal(qint32, qint32)), this, SLOT(onButtonFontColorSignal(qint32, qint32)));
   connect(this, SIGNAL(setButtonCheckableSignal(qint32, bool)), this, SLOT(onButtonCheckableSignal(qint32, bool)));
   connect(this, SIGNAL(setButtonCheckedSignal(qint32, bool)), this, SLOT(onButtonCheckedSignal(qint32, bool)));
   connect(this, SIGNAL(setButtonEnabledSignal(qint32, bool)), this, SLOT(onButtonEnabledSignal(qint32, bool)));
   connect(this, SIGNAL(setButtonTextSignal(qint32, QString)), this, SLOT(onButtonTextSignal(qint32, QString)));
   connect(this, SIGNAL(clearTerminalViewSignal()), this, SLOT(onClearTerminalViewSignal()));
   connect(this, SIGNAL(clearTraceViewSignal()), this, SLOT(onClearTraceViewSignal()));
   connect(this, SIGNAL(addToTerminalViewSignal(QString, qint32, qint32)), this, SLOT(onAddToTerminalViewSignal(QString, qint32, qint32)));
   connect(this, SIGNAL(addToTraceViewSignal(QString, qint32, qint32)), this, SLOT(onAddToTraceViewSignal(QString, qint32, qint32)));
   connect(this, SIGNAL(scrollTerminalToBottomSignal()), this, SLOT(onScrollTerminalToBottomSignal()));
   connect(this, SIGNAL(scrollTraceViewToBottomSignal()), this, SLOT(onScrollTraceViewToBottomSignal()));
   connect(this, SIGNAL(registerPortOpenedSignal(QString)), this, SLOT(onRegisterPortOpenedSignal(QString)));
   connect(this, SIGNAL(registerPortClosedSignal(QString)), this, SLOT(onRegisterPortClosedSignal(QString)));
   connect(this, SIGNAL(setCommandHistorySignal(QVector<QString>)), this, SLOT(onSetCommandHistorySignal(QVector<QString>)));
   connect(this, SIGNAL(addLineEndingSignal(QString)), this, SLOT(onAddLineEndingSignal(QString)));
   connect(this, SIGNAL(setCurrentLineEndingSignal(QString)), this, SLOT(onSetCurrentLineEndingSignal(QString)));
   connect(this, SIGNAL(setTraceFilterSignal(qint8, QString)), this, SLOT(onSetTraceFilterSignal(qint8, QString)));
   connect(this, SIGNAL(setTraceFilterEnabledSignal(qint8, bool)), this, SLOT(onSetTraceFilterEnabledSignal(qint8, bool)));
   connect(this, SIGNAL(setTraceFilterBackgroundColorSignal(qint32, qint32)), this, SLOT(onSetTraceFilterBackgroundColorSignal(qint32, qint32)));
   connect(this, SIGNAL(setTraceFilterFontColorSignal(qint32, qint32)), this, SLOT(onSetTraceFilterFontColorSignal(qint32, qint32)));
   connect(this, SIGNAL(setPortLabelTextSignal(qint8, QString)), this, SLOT(onSetPortLabelTextSignal(qint8, QString)));
   connect(this, SIGNAL(setThroughputTextSignal(qint8, QString)), this, SLOT(onSetThroughputTextSignal(qint8, QString)));
   connect(this, SIGNAL(reloadThemeSignal(qint8)), this, SLOT(onReloadThemeSignal(qint8)));
   connect(this, SIGNAL(setStatusBarNotificationSignal(QString, qint32)), this, SLOT(onSetStatusBarNotificationSignal(QString, qint32)));
   connect(this, SIGNAL(setInfoLabelTextSignal(QString)), this, SLOT(onSetInfoLabelTextSignal(QString)));
   connect(this, SIGNAL(setApplicationTitle(QString)), this, SLOT(onSetApplicationTitle(QString)));
   connect(this, SIGNAL(clearCurrentCommand()), this, SLOT(onClearCurrentCommand()));

   connect(this, SIGNAL(guiRequestSignal()), this, SLOT(onGuiRequestSignal()));
#ifdef SIMULATION
   UT_Log(GUI_CONTROLLER, LOW, "SIMULATION build");
   m_gui_test_server = std::unique_ptr<GUITestServer>(new GUITestServer(ui, *this));
   /* ugly hack to make terminalView slider position working */
   show();
   hide();
#else
   UT_Log(GUI_CONTROLLER, LOW, "showing main application window");
   show();
#endif
}
uint32_t GUIController::getButtonID(const std::string& name)
{
   uint32_t result = UINT32_MAX;
   auto elements = ui->getButtons();
   for (uint32_t i = 0; i < elements.size(); i++)
   {
      UT_Assert(elements[i]);
      if (elements[i]->objectName().toStdString() == name)
      {
         result = i;
         break;
      }
   }
   UT_Log_If(result == UINT32_MAX, GUI_CONTROLLER, ERROR, "Element with name %s not found", name.c_str());
   return result;
}
void GUIController::subscribeForButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) && (item.event == event);});
   if (it != m_button_listeners.end())
   {
      UT_Log(GUI_CONTROLLER, ERROR, "event %u for buttonID %u already registered, replacing!");
      *it = ButtonEventItem{button_id, event, listener};
   }
   else
   {
      UT_Log(GUI_CONTROLLER, MEDIUM, "event %u for buttonID %u registered!", (uint32_t)event, button_id);
      m_button_listeners.push_back(ButtonEventItem{button_id, event, listener});
   }
}
void GUIController::unsubscribeFromButtonEvent(uint32_t button_id, ButtonEvent event, ButtonEventListener* listener)
{
   std::lock_guard<std::mutex> lock(m_button_listeners_mutex);
   auto it = std::find_if(m_button_listeners.begin(), m_button_listeners.end(), [&](const ButtonEventItem& item){return (item.id == button_id) &&
                                                                                                                        (item.event == event) &&
                                                                                                                        (item.listener == listener);});
   UT_Log_If(it == m_button_listeners.end(), GUI_CONTROLLER, ERROR, "event %u for buttonID %u not found!");
   if (it != m_button_listeners.end())
   {
      UT_Log(GUI_CONTROLLER, MEDIUM, "event %u for buttonID %u unregistered", (uint32_t)event, button_id);
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
void GUIController::scrollTerminalToBottom()
{
   emit scrollTerminalToBottomSignal();
}
void GUIController::scrollTraceViewToBottom()
{
   emit scrollTraceViewToBottomSignal();
}
void GUIController::subscribeForActivePortChangedEvent(std::function<bool(const std::string&)> callback)
{
   UT_Log(GUI_CONTROLLER, HIGH, "%s", __func__);
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
std::string GUIController::getCurrentCommand()
{
   GetCommandRequest request;
   executeGUIRequest(&request);
   return request.command;
}
void GUIController::clearCommand()
{
   emit clearCurrentCommand();
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
void GUIController::setThroughputText(uint8_t port_id, const std::string& text)
{
   emit setThroughputTextSignal(port_id, QString(text.c_str()));
}
void GUIController::reloadTheme(Theme theme)
{
   emit reloadThemeSignal((uint8_t)theme);
}
Theme GUIController::currentTheme()
{
   return ui->theme;
}
std::string GUIController::themeToName(Theme theme)
{
   UT_Assert(theme < Theme::APPLICATION_THEMES_MAX);
   return m_themes_names[(uint32_t)theme];
}
Theme GUIController::nameToTheme(const std::string& name)
{
   Theme result = Theme::APPLICATION_THEMES_MAX;
   auto it = std::find(m_themes_names.begin(), m_themes_names.end(), name);
   if (it != m_themes_names.end())
   {
      result = (Theme)(std::distance(m_themes_names.begin(), it));
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
   UT_Log_If(it != m_theme_listeners.end(), GUI_CONTROLLER, ERROR, "Theme listener already found!");
   if (it == m_theme_listeners.end())
   {
      m_theme_listeners.push_back(listener);
   }
}
void GUIController::unsubscribeFromThemeReloadEvent(ThemeListener* listener)
{
   std::lock_guard<std::mutex> lock(m_theme_listeners_mutex);
   auto it = std::find_if(m_theme_listeners.begin(), m_theme_listeners.end(), [&](ThemeListener* theme_listener){return (theme_listener == listener);});
   UT_Log_If(it == m_theme_listeners.end(), GUI_CONTROLLER, ERROR, "Theme listener not found!");
   if (it != m_theme_listeners.end())
   {
      m_theme_listeners.erase(it);
   }
}
QWidget* GUIController::getParent()
{
   return this;
}
void GUIController::setStatusBarNotification(const std::string& notification, uint32_t timeout)
{
   emit setStatusBarNotificationSignal(QString(notification.c_str()), timeout);
}
void GUIController::setInfoLabelText(const std::string& text)
{
   emit setInfoLabelTextSignal(QString(text.c_str()));
}
void GUIController::setApplicationTitle(const std::string& title)
{
   emit setApplicationTitle(QString(title.c_str()));
}
uint32_t GUIController::countUserButtons()
{
   return ui->countUserButtons();
}
uint32_t GUIController::countPorts()
{
   return ui->countPorts();
}
uint32_t GUIController::countTraceFilters()
{
   return ui->countTraceFilters();
}
bool GUIController::executeGUIRequest(CommandExecutor* request)
{
   std::lock_guard<std::mutex> lock(m_command_mutex);
   bool result = true;
   m_current_command = request;

   emit guiRequestSignal();
   if (!request->waitReady(500))
   {
      UT_Log(GUI_CONTROLLER, ERROR, "%s timeout", __func__);
      result = false;
   }
   m_current_command = nullptr;

   return result;
}
void GUIController::onButtonClicked()
{
   QObject* object = sender();

   UT_Log(GUI_CONTROLLER, HIGH, "button %s clicked", object->objectName().toStdString().c_str());

   QPushButton* button = nullptr;
   if (m_shortcuts_map.find(object) != m_shortcuts_map.end())
   {
      // check if sender has respective button alternative
      button = m_shortcuts_map[object];
      UT_Log(GUI_CONTROLLER, HIGH, "found alternative for %s : %s", object->objectName().toStdString().c_str(),
                                                              button->objectName().toStdString().c_str());
   }
   else
   {
      button = reinterpret_cast<QPushButton*>(object);
   }

   //find button id
   auto& buttons = ui->getButtons();
   auto it = std::find(buttons.begin(), buttons.end(), button);
   if (it != buttons.end())
   {
      uint32_t id = std::distance(buttons.begin(), it);
      UT_Log(GUI_CONTROLLER, HIGH, "notifying button%u click event", id);
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
   QObject* object = sender();

   UT_Log(GUI_CONTROLLER, HIGH, "object %s context requested", object->objectName().toStdString().c_str());

   QPushButton* button = nullptr;
   if (m_shortcuts_map.find(object) != m_shortcuts_map.end())
   {
      // check if sender has respective button alternative
      button = m_shortcuts_map[object];
      UT_Log(GUI_CONTROLLER, HIGH, "found alternative for %s : %s", object->objectName().toStdString().c_str(),
                                                              button->objectName().toStdString().c_str());
   }
   else
   {
      button = reinterpret_cast<QPushButton*>(object);
   }
   auto& buttons = ui->getButtons();
   auto it = std::find(buttons.begin(), buttons.end(), button);
   if (it != buttons.end())
   {
      uint32_t id = std::distance(buttons.begin(), it);
      UT_Log(GUI_CONTROLLER, HIGH, "notifying button%u context menu event", id);
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
   UT_Log(GUI_CONTROLLER, LOW, "Selected port changed, idx %u, name %s", (uint8_t) index, port_name.c_str());
   if (m_active_port_listener)
   {
      bool result = m_active_port_listener(port_name);
      UT_Log(GUI_CONTROLLER, LOW, "%s - Event %s consumed", __func__, result? "" : "not");
   }
}
void GUIController::onPortSwitchRequest()
{
   int element_count = ui->portComboBox->count();
   UT_Log_If(element_count == 0, GUI_CONTROLLER, ERROR, "Empty port list, cannot select next item");

   if (element_count > 1)
   {
      int idx = ui->portComboBox->currentIndex();
      if (idx == (element_count - 1))
      {
         UT_Log(GUI_CONTROLLER, LOW, "List end reached, selecting first available port");
         ui->portComboBox->setCurrentIndex(0);
      }
      else
      {
         UT_Log(GUI_CONTROLLER, LOW, "Selecting next available port");
         ui->portComboBox->setCurrentIndex(idx+1);
      }
   }
}
void GUIController::onButtonBackgroundColorSignal(qint32 id, qint32 color)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u color %.6x", __func__, id, color);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   QPalette palette = buttons[id]->palette();
   palette.setColor(QPalette::Button, color);
   buttons[id]->setPalette(palette);
   buttons[id]->update();
}
void GUIController::onButtonFontColorSignal(qint32 id, qint32 color)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u color %.6x", __func__, id, color);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   QPalette palette = buttons[id]->palette();
   palette.setColor(QPalette::ButtonText, color);
   buttons[id]->setPalette(palette);
   buttons[id]->update();
}
void GUIController::onButtonCheckableSignal(qint32 id, bool checkable)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u checkable %u", __func__, id, checkable);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   buttons[id]->setCheckable(checkable);
}
void GUIController::onButtonCheckedSignal(qint32 id, bool checked)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u checked %u", __func__, id, checked);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   buttons[id]->setChecked(checked);
}
void GUIController::onButtonEnabledSignal(qint32 id, bool enabled)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u enabled %u", __func__, id, enabled);
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   buttons[id]->setEnabled(enabled);
}
void GUIController::onButtonTextSignal(qint32 id, QString text)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u text %s", __func__, id, text.toStdString().c_str());
   auto buttons = ui->getButtons();
   UT_Assert(buttons.size() > (size_t)id);
   buttons[id]->setText(text);
}
void GUIController::onClearTerminalViewSignal()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->terminalView->clear();
   m_terminal_view_status.item_count = 0;
}
void GUIController::onClearTraceViewSignal()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->traceView->clear();
   m_trace_view_status.item_count = 0;
}
void GUIController::onAddToTerminalViewSignal(QString text, qint32, qint32)
{
   ui->terminalView->append(text);
   m_terminal_view_status.item_count++;
}
void GUIController::onAddToTraceViewSignal(QString text, qint32 background_color, qint32 font_color)
{
   UT_Log(GUI_CONTROLLER, HIGH, "%s bg color %.6x font %.6x", __func__, background_color, font_color);
   QListWidgetItem* item = new QListWidgetItem();
   item->setText(text);
   item->setBackground(QColor(background_color));
   item->setForeground(QColor(font_color));
   ui->traceView->addItem(item);
   m_trace_view_status.item_count++;
   if ((ui->traceView->verticalScrollBar()->maximum() == ui->traceView->verticalScrollBar()->sliderPosition()))
   {
      ui->traceView->scrollToBottom();
   }
}
void GUIController::onScrollTerminalToBottomSignal()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->terminalView->verticalScrollBar()->setValue(ui->terminalView->verticalScrollBar()->maximum());
}
void GUIController::onScrollTraceViewToBottomSignal()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %u", __func__);
   ui->traceView->verticalScrollBar()->setValue(ui->traceView->verticalScrollBar()->maximum());
}
void GUIController::onRegisterPortOpenedSignal(QString name)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %s", __func__, name.toStdString().c_str());
   ui->portComboBox->addItem(name);
}
void GUIController::onRegisterPortClosedSignal(QString name)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %s", __func__, name.toStdString().c_str());
   int index = ui->portComboBox->findText(name);
   if (index != -1)
   {
      ui->portComboBox->removeItem(index);
   }
   UT_Log_If(index == -1, GUI_CONTROLLER, ERROR, "%s %s not found", __func__, name.toStdString().c_str());
}
void GUIController::onSetCommandHistorySignal(QVector<QString> history)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s size %u", __func__, history.size());
   ui->textEdit->clear();
   for (const QString& command : history)
   {
      UT_Log(GUI_CONTROLLER, HIGH, "%s inserting %s", __func__, command.toStdString().c_str());
      ui->textEdit->insertItem(0, command);
   }
   ui->textEdit->setCurrentIndex(-1);
}
void GUIController::onGuiRequestSignal()
{
   UT_Assert(m_current_command && "Create m_current_command first!");
   m_current_command->execute(ui);
}
void GUIController::onAddLineEndingSignal(QString ending)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->lineEndingComboBox->addItem(ending);
}
void GUIController::onSetCurrentLineEndingSignal(QString ending)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->lineEndingComboBox->setCurrentText(ending);
}
void GUIController::onSetTraceFilterSignal(qint8 id, QString filter)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u filter %s", __func__, id, filter.toStdString().c_str());
   auto filters = ui->getTraceFilters();
   UT_Assert(filters.size() > (size_t)id);
   filters[id].line_edit->setText(filter);
}
void GUIController::onSetTraceFilterEnabledSignal(qint8 id, bool enabled)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u state %u", __func__, id, enabled);
   auto filters = ui->getTraceFilters();
   UT_Assert(filters.size() > (size_t)id);
   filters[id].line_edit->setEnabled(enabled);
}
void GUIController::onSetTraceFilterBackgroundColorSignal(qint32 id, qint32 color)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u color %x", __func__, id, color);
   auto& filters = ui->getTraceFilters();
   UT_Assert((size_t)id < filters.size());
   Stylesheet ss (filters[id].line_edit->styleSheet().toStdString());
   ss.setColor(Stylesheet::Item::BACKGROUND_COLOR, color);
   filters[id].line_edit->setStyleSheet(QString(ss.stylesheet().c_str()));
   filters[id].line_edit->update();
   UT_Log(GUI_CONTROLLER, ERROR, "%s stylesheet [%s] trace filter %u, color %d", __func__, ss.stylesheet().c_str(), id, color);
}
void GUIController::onSetTraceFilterFontColorSignal(qint32 id, qint32 color)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u color %x", __func__, id, color);
   auto& filters = ui->getTraceFilters();
   UT_Assert((size_t)id < filters.size());
   Stylesheet ss (filters[id].line_edit->styleSheet().toStdString());
   ss.setColor(Stylesheet::Item::COLOR, color);
   filters[id].line_edit->setStyleSheet(QString(ss.stylesheet().c_str()));
   filters[id].line_edit->update();
   UT_Log(GUI_CONTROLLER, ERROR, "%s stylesheet [%s] trace filter %u, color %d", __func__, ss.stylesheet().c_str(), id, color);
}
void GUIController::onSetPortLabelTextSignal(qint8 id, QString description)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u text %s", __func__, id, description.toStdString().c_str());
   auto& ports = ui->getPorts();
   UT_Assert((size_t)id < ports.size());
   ports[id].label->setText(description);
}
void GUIController::onSetThroughputTextSignal(qint8 id, QString text)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u text %s", __func__, id, text.toStdString().c_str());
   auto& ports = ui->getPorts();
   UT_Assert((size_t)id < ports.size());
   ports[id].throughputLabel->setText(text);

}
void GUIController::onReloadThemeSignal(qint8 id)
{
   UT_Assert((Theme)id < Theme::APPLICATION_THEMES_MAX);
   UT_Log(GUI_CONTROLLER, LOW, "%s id %u name %s", __func__, id, themeToName((Theme)id).c_str());
   ui->loadTheme((Theme)id);
   for (auto& listener : m_theme_listeners)
   {
      if (listener) listener->onThemeChange((Theme)id);
   }
}
void GUIController::onSetStatusBarNotificationSignal(QString notification, qint32 timeout)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %s time %u", __func__, notification.toStdString().c_str(), timeout);
   ui->statusbar->showMessage(notification, timeout);
}
void GUIController::onSetInfoLabelTextSignal(QString text)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %s ", __func__, text.toStdString().c_str());
   ui->infoLabel->setText(text);
}
void GUIController::onSetApplicationTitle(QString title)
{
   UT_Log(GUI_CONTROLLER, LOW, "%s %s ", __func__, title.toStdString().c_str());
   this->setWindowTitle(title);
}
void GUIController::onClearCurrentCommand()
{
   UT_Log(GUI_CONTROLLER, LOW, "%s", __func__);
   ui->textEdit->lineEdit()->clear();
}
