#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "QtWidgets/QtWidgetsMock.h"
#include "GUIController.h"
#include "Settings.h"

using namespace ::testing;

struct IGUIControllerFixture : public testing::Test
{
   void SetUp()
   {
      SETTING_SET_U32(GUI_UserButtons_Tabs, 1);
      SETTING_SET_U32(GUI_UserButtons_RowsPerTab, 2);
      SETTING_SET_U32(GUI_UserButtons_ButtonsPerRow, 5);


      QtCoreMock_init();
      QtWidgetsMock_init();

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_new()).WillOnce(Return(&test_marker_button))
                                                         .WillOnce(Return(&test_logging_button))
                                                         .WillOnce(Return(&test_settings_button))
                                                         .WillOnce(Return(&test_user_button_1))
                                                         .WillOnce(Return(&test_user_button_2))
                                                         .WillOnce(Return(&test_user_button_3))
                                                         .WillOnce(Return(&test_user_button_4))
                                                         .WillOnce(Return(&test_user_button_5))
                                                         .WillOnce(Return(&test_user_button_6))
                                                         .WillOnce(Return(&test_user_button_7))
                                                         .WillOnce(Return(&test_user_button_8))
                                                         .WillOnce(Return(&test_user_button_9))
                                                         .WillOnce(Return(&test_user_button_10))
                                                         .WillOnce(Return(&test_send_button))
                                                         .WillOnce(Return(&test_scroll_button))
                                                         .WillOnce(Return(&test_clear_button))
                                                         .WillOnce(Return(&test_trace_clear_button))
                                                         .WillOnce(Return(&test_trace_scroll_button))
                                                         .WillOnce(Return(&test_trace_filter_button_0))
                                                         .WillOnce(Return(&test_trace_filter_button_1))
                                                         .WillOnce(Return(&test_trace_filter_button_2))
                                                         .WillOnce(Return(&test_trace_filter_button_3))
                                                         .WillOnce(Return(&test_trace_filter_button_4))
                                                         .WillOnce(Return(&test_trace_filter_button_5))
                                                         .WillOnce(Return(&test_trace_filter_button_6))
                                                         .WillOnce(Return(&test_port_button_0))
                                                         .WillOnce(Return(&test_port_button_1))
                                                         .WillOnce(Return(&test_port_button_2))
                                                         .WillOnce(Return(&test_port_button_3))
                                                         .WillOnce(Return(&test_port_button_4));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_new()).WillOnce(Return(&test_info_label))
                                                     .WillOnce(Return(&test_port_label_0))
                                                     .WillOnce(Return(&test_port_label_1))
                                                     .WillOnce(Return(&test_port_label_2))
                                                     .WillOnce(Return(&test_port_label_3))
                                                     .WillOnce(Return(&test_port_label_4));
      EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_port_box))
                                                        .WillOnce(Return(&test_line_ending_box))
                                                        .WillOnce(Return(&test_text_edit));
      EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_new()).WillOnce(Return(&test_trace_view));
      EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_new()).WillOnce(Return(&test_terminal_view));
      EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setReadOnly(&test_terminal_view, true));
      EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setHorizontalScrollBarPolicy(&test_terminal_view, Qt::ScrollBarAlwaysOff));
      EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_setWordWrapMode(&test_terminal_view, QTextOption::WrapAnywhere));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillOnce(Return(&test_trace_filter_0))
                                                        .WillOnce(Return(&test_trace_filter_1))
                                                        .WillOnce(Return(&test_trace_filter_2))
                                                        .WillOnce(Return(&test_trace_filter_3))
                                                        .WillOnce(Return(&test_trace_filter_4))
                                                        .WillOnce(Return(&test_trace_filter_5))
                                                        .WillOnce(Return(&test_trace_filter_6));
      EXPECT_CALL(*QtWidgetsMock_get(), QMenuBar_new()).WillOnce(Return(&test_menu_bar));
      EXPECT_CALL(*QtWidgetsMock_get(), QStatusBar_new()).WillOnce(Return(&test_status_bar));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(_,_)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(_,_)).Times(AtLeast(1));

      EXPECT_CALL(*QtWidgetsMock_get(), QShortcut_new()).WillOnce(Return(&test_loggingButtonShortcut))
                                                        .WillOnce(Return(&test_markerButtonShortcut))
                                                        .WillOnce(Return(&test_port0ButtonShortcut))
                                                        .WillOnce(Return(&test_port1ButtonShortcut))
                                                        .WillOnce(Return(&test_port2ButtonShortcut))
                                                        .WillOnce(Return(&test_port3ButtonShortcut))
                                                        .WillOnce(Return(&test_port4ButtonShortcut))
                                                        .WillOnce(Return(&test_clearButtonShortcut))
                                                        .WillOnce(Return(&test_traceClearButtonShortcut))
                                                        .WillOnce(Return(&test_switchSendPortShortcut));
      EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_new()).WillOnce(Return(&test_tab_widget));
      EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_tab_widget, _,_));

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(_, Qt::ContextMenuPolicy::CustomContextMenu)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setContextMenuPolicy(_, Qt::ContextMenuPolicy::CustomContextMenu)).Times(AtLeast(1));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(_, _, _, _)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setAutoFillBackground(_, true)).Times(AtLeast(1));
      EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setAlignment(_, Qt::AlignCenter)).Times(AtLeast(1));
      m_test_subject = std::unique_ptr<GUIController>(new GUIController(&m_parent));
      m_test_subject->run();
      Mock::VerifyAndClearExpectations(QtCoreMock_get());
      Mock::VerifyAndClearExpectations(QtWidgetsMock_get());
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }

   std::unique_ptr<GUIController> m_test_subject;
   QMainWindow m_parent;
   QTabWidget test_tab_widget;
   QPushButton test_user_button_1;
   QPushButton test_user_button_2;
   QPushButton test_user_button_3;
   QPushButton test_user_button_4;
   QPushButton test_user_button_5;
   QPushButton test_user_button_6;
   QPushButton test_user_button_7;
   QPushButton test_user_button_8;
   QPushButton test_user_button_9;
   QPushButton test_user_button_10;
   QPushButton test_marker_button;
   QPushButton test_logging_button;
   QPushButton test_settings_button;
   QPushButton test_send_button;
   QPushButton test_scroll_button;
   QPushButton test_clear_button;
   QPushButton test_trace_clear_button;
   QPushButton test_trace_scroll_button;
   QPushButton test_trace_filter_button_0;
   QPushButton test_trace_filter_button_1;
   QPushButton test_trace_filter_button_2;
   QPushButton test_trace_filter_button_3;
   QPushButton test_trace_filter_button_4;
   QPushButton test_trace_filter_button_5;
   QPushButton test_trace_filter_button_6;
   QPushButton test_port_button_0;
   QPushButton test_port_button_1;
   QPushButton test_port_button_2;
   QPushButton test_port_button_3;
   QPushButton test_port_button_4;
   QLabel test_info_label;
   QLabel test_port_label_0;
   QLabel test_port_label_1;
   QLabel test_port_label_2;
   QLabel test_port_label_3;
   QLabel test_port_label_4;
   QComboBox test_port_box;
   QComboBox test_line_ending_box;
   QComboBox test_text_edit;
   QTextEdit test_terminal_view;
   QListWidget test_trace_view;
   QLineEdit test_trace_filter_0;
   QLineEdit test_trace_filter_1;
   QLineEdit test_trace_filter_2;
   QLineEdit test_trace_filter_3;
   QLineEdit test_trace_filter_4;
   QLineEdit test_trace_filter_5;
   QLineEdit test_trace_filter_6;
   QMenuBar test_menu_bar;
   QStatusBar test_status_bar;
   QShortcut test_loggingButtonShortcut;
   QShortcut test_markerButtonShortcut;
   QShortcut test_port0ButtonShortcut;
   QShortcut test_port1ButtonShortcut;
   QShortcut test_port2ButtonShortcut;
   QShortcut test_port3ButtonShortcut;
   QShortcut test_port4ButtonShortcut;
   QShortcut test_clearButtonShortcut;
   QShortcut test_traceClearButtonShortcut;
   QShortcut test_switchSendPortShortcut;

};

void GUIController::setButtonBackgroundColorSignal(uint32_t id, uint32_t color){onButtonBackgroundColorSignal(id, color);}
void GUIController::setButtonFontColorSignal(uint32_t id, uint32_t color){onButtonFontColorSignal(id, color);}
void GUIController::setButtonCheckableSignal(uint32_t id, bool checkable){onButtonCheckableSignal(id, checkable);}
void GUIController::setButtonCheckedSignal(uint32_t id, bool checked){onButtonCheckedSignal(id, checked);}
void GUIController::setButtonEnabledSignal(uint32_t id, bool enabled){onButtonEnabledSignal(id, enabled);}
void GUIController::setButtonTextSignal(uint32_t id, QString text){onButtonTextSignal(id, text);}
void GUIController::clearTerminalViewSignal(){onClearTerminalViewSignal();}
void GUIController::clearTraceViewSignal(){onClearTraceViewSignal();}
void GUIController::addToTerminalViewSignal(QString text, uint32_t background_color, uint32_t font_color){onAddToTerminalViewSignal(text, background_color, font_color);}
void GUIController::addToTraceViewSignal(QString text, uint32_t background_color, uint32_t font_color){onAddToTraceViewSignal(text, background_color, font_color);}
void GUIController::scrollTerminalToBottomSignal(){onScrollTerminalToBottomSignal();}
void GUIController::scrollTraceViewToBottomSignal(){onScrollTraceViewToBottomSignal();}
void GUIController::registerPortOpenedSignal(QString name){onRegisterPortOpenedSignal(name);}
void GUIController::registerPortClosedSignal(QString name){onRegisterPortClosedSignal(name);}
void GUIController::setCommandHistorySignal(QVector<QString> history){onSetCommandHistorySignal(history);}
void GUIController::guiRequestSignal(){onGuiRequestSignal();}
void GUIController::addLineEndingSignal(QString ending){onAddLineEndingSignal(ending);}
void GUIController::setTraceFilterSignal(uint8_t id, QString filter){onSetTraceFilterSignal(id, filter);}
void GUIController::setTraceFilterEnabledSignal(uint8_t id, bool enabled){onSetTraceFilterEnabledSignal(id, enabled);}
void GUIController::setTraceFilterBackgroundColorSignal(uint32_t id, uint32_t color){onSetTraceFilterBackgroundColorSignal(id, color);}
void GUIController::setTraceFilterFontColorSignal(uint32_t id, uint32_t color){onSetTraceFilterFontColorSignal(id, color);}
void GUIController::setPortLabelTextSignal(uint8_t id, QString description){onSetPortLabelTextSignal(id, description);}
void GUIController::reloadThemeSignal(uint8_t id){onReloadThemeSignal(id);}
void GUIController::setStatusBarNotificationSignal(QString notification, qint32 timeout){onSetStatusBarNotificationSignal(notification, timeout);}
void GUIController::setInfoLabelTextSignal(QString text){onSetInfoLabelTextSignal(text);}
void GUIController::setApplicationTitle(QString title){onSetApplicationTitle(title);}
void GUIController::setCurrentLineEndingSignal(QString ending){onSetCurrentLineEndingSignal(ending);}
void GUIController::clearCurrentCommand(){onClearCurrentCommand();}

class ButtonEventListenerMock : public ButtonEventListener
{
public:
   MOCK_METHOD2(onButtonEvent, void(uint32_t, ButtonEvent event));
};

class ThemeListenerMock : public ThemeListener
{
public:
   MOCK_METHOD1(onThemeChange, void(Theme));
};

TEST_F(IGUIControllerFixture, getting_element_ids)
{
   /**
    * <b>scenario</b>: Getting existing element ID. <br>
    * <b>expected</b>: ID different than UINT32_MAX returned. <br>
    * ************************************************
    */
   EXPECT_THAT(m_test_subject->getButtonID("markerButton"), Ne(UINT32_MAX));
   EXPECT_THAT(m_test_subject->getButtonID("traceFilterButton_5"), Ne(UINT32_MAX));

   /**
    * <b>scenario</b>: Getting non-existing button ID. <br>
    * <b>expected</b>: ID equal to UINT32_MAX returned. <br>
    * ************************************************
    */
   EXPECT_EQ(m_test_subject->getButtonID("non existing button"), UINT32_MAX);

}

TEST_F(IGUIControllerFixture, button_events_receiving)
{
   ButtonEventListenerMock listener;
   ButtonEventListenerMock listener2;
   ButtonEventListenerMock listener3;
   ButtonEventListenerMock listener4;
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");
   uint32_t test_button_id2 = m_test_subject->getButtonID("loggingButton");

   /**
    * <b>scenario</b>: Triggering button event when not subscribed. <br>
    * <b>expected</b>: No listener called. <br>
    * ************************************************
    */
   EXPECT_CALL(listener, onButtonEvent(_,_)).Times(0);
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_marker_button));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonClicked();

   /**
    * <b>scenario</b>: Triggering button event when two listeners subscribed, one for CLICKED event, one for CONTEXT MENU EVENT. <br>
    * <b>expected</b>: Only listener registered for CLICKED event. <br>
    * ************************************************
    */

   m_test_subject->subscribeForButtonEvent(test_button_id, ButtonEvent::CLICKED, &listener);
   m_test_subject->subscribeForButtonEvent(test_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, &listener2);
   m_test_subject->subscribeForButtonEvent(test_button_id2, ButtonEvent::CLICKED, &listener3);
   m_test_subject->subscribeForButtonEvent(test_button_id2, ButtonEvent::CONTEXT_MENU_REQUESTED, &listener4);

   EXPECT_CALL(listener, onButtonEvent(test_button_id, ButtonEvent::CLICKED));
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_marker_button));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonClicked();

   /**
    * <b>scenario</b>: Triggering button event when two listeners subscribed, one for CLICKED event, one for CONTEXT MENU EVENT. <br>
    * <b>expected</b>: Only listener registered for CONTEXT MENU event. <br>
    * ************************************************
    */

   EXPECT_CALL(listener2, onButtonEvent(test_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED));
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_marker_button));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonContextMenuRequested();

   /**
    * <b>scenario</b>: Listeners unregistered. <br>
    * <b>expected</b>: No listeners called on button event. <br>
    * ************************************************
    */

   m_test_subject->unsubscribeFromButtonEvent(test_button_id, ButtonEvent::CLICKED, &listener);
   m_test_subject->unsubscribeFromButtonEvent(test_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, &listener2);
   m_test_subject->unsubscribeFromButtonEvent(test_button_id2, ButtonEvent::CLICKED, &listener3);
   m_test_subject->unsubscribeFromButtonEvent(test_button_id2, ButtonEvent::CONTEXT_MENU_REQUESTED, &listener4);

   EXPECT_CALL(listener2, onButtonEvent(_, _)).Times(0);
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_marker_button));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonContextMenuRequested();

   /**
    * <b>scenario</b>: No listener provided when subscribing. <br>
    * <b>expected</b>: No application crash. <br>
    * ************************************************
    */
   m_test_subject->subscribeForButtonEvent(test_button_id, ButtonEvent::CLICKED, nullptr);
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_marker_button));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonClicked();

}

TEST_F(IGUIControllerFixture, trace_filters_mapping_tests)
{
   ButtonEventListenerMock listener;
   uint32_t test_button_id = m_test_subject->getButtonID("traceFilterButton_2");
   m_test_subject->subscribeForButtonEvent(test_button_id, ButtonEvent::CLICKED, &listener);
   m_test_subject->subscribeForButtonEvent(test_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED, &listener);

   /**
    * <b>scenario</b>: Line edit click requested for TraceFilter line edit object. <br>
    * <b>expected</b>: Corresponding button event shall be notified. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_trace_filter_2));
   EXPECT_CALL(listener, onButtonEvent(test_button_id, ButtonEvent::CLICKED));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonClicked();

   /**
    * <b>scenario</b>: Line edit context menu requested for TraceFilter line edit object. <br>
    * <b>expected</b>: Corresponding button event shall be notified. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QSender()).WillOnce(Return(&test_trace_filter_2));
   EXPECT_CALL(listener, onButtonEvent(test_button_id, ButtonEvent::CONTEXT_MENU_REQUESTED));
   dynamic_cast<GUIController*>(m_test_subject.get())->onButtonContextMenuRequested();

}

TEST_F(IGUIControllerFixture, setting_button_background_color)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonBackgroundColor requested. <br>
    * <b>expected</b>: Button background color changed. <br>
    * ************************************************
    */
   m_test_subject->setButtonBackgroundColor(test_button_id, 0xDEAD);
   EXPECT_EQ(test_marker_button.palette().color(QPalette::Button).rgb(), 0xDEAD);
}

TEST_F(IGUIControllerFixture, setting_button_font_color)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonFontColor requested. <br>
    * <b>expected</b>: Button font color changed. <br>
    * ************************************************
    */
   m_test_subject->setButtonFontColor(test_button_id, 0xDEAD);
   EXPECT_EQ(test_marker_button.palette().color(QPalette::ButtonText).rgb(), 0xDEAD);
}

TEST_F(IGUIControllerFixture, setting_button_checkable)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonCheckable requested. <br>
    * <b>expected</b>: Button checkable state changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setCheckable(&test_marker_button, false));
   m_test_subject->setButtonCheckable(test_button_id, false);
}

TEST_F(IGUIControllerFixture, setting_button_checked)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonChecked requested. <br>
    * <b>expected</b>: Button checked state changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_marker_button, true));
   m_test_subject->setButtonChecked(test_button_id, true);
}

TEST_F(IGUIControllerFixture, setting_button_enabled)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonEnabled requested. <br>
    * <b>expected</b>: Button enabled state changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_marker_button, false));
   m_test_subject->setButtonEnabled(test_button_id, false);
}

TEST_F(IGUIControllerFixture, setting_button_text)
{
   uint32_t test_button_id = m_test_subject->getButtonID("markerButton");

   /**
    * <b>scenario</b>: setButtonText requested. <br>
    * <b>expected</b>: Button text changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_marker_button, QString("TEST")));
   m_test_subject->setButtonText(test_button_id, "TEST");
}

TEST_F(IGUIControllerFixture, clearing_terminal_view)
{
   /**
    * <b>scenario</b>: clearTerminalView requested. <br>
    * <b>expected</b>: terminal view cleared. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_clear(&test_terminal_view));
   m_test_subject->clearTerminalView();
}

TEST_F(IGUIControllerFixture, clearing_trace_view)
{
   /**
    * <b>scenario</b>: clearTerminalView requested. <br>
    * <b>expected</b>: terminal view cleared. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_clear(&test_trace_view));
   m_test_subject->clearTraceView();
}

TEST_F(IGUIControllerFixture, adding_to_terminal_view)
{
   /**
    * <b>scenario</b>: addToTerminalView requested <br>
    * <b>expected</b>: Text added to terminal. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QTextEdit_append(&test_terminal_view, _));
   m_test_subject->addToTerminalView("TEXT", 0x0001, 0x0002);
   EXPECT_EQ(m_test_subject->countTerminalItems(), 1);
   EXPECT_EQ(m_test_subject->countTraceItems(), 0);
}

TEST_F(IGUIControllerFixture, adding_to_trace_view)
{
   QListWidgetItem list_item;

   /**
    * <b>scenario</b>: addToTraceView requested <br>
    * <b>expected</b>: Text added to trace view. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_new()).WillOnce(Return(&list_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setText(&list_item, QString("TEXT")));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setBackground(&list_item, QColor(0x0001)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidgetItem_setForeground(&list_item, QColor(0x0002)));
   EXPECT_CALL(*QtWidgetsMock_get(), QListWidget_addItem(&test_trace_view, &list_item));

   m_test_subject->addToTraceView("TEXT", 0x0001, 0x0002);

   EXPECT_EQ(m_test_subject->countTerminalItems(), 0);
   EXPECT_EQ(m_test_subject->countTraceItems(), 1);
}

TEST_F(IGUIControllerFixture, active_port_changed_event)
{
   std::string port_name = "PORT X";
   std::string receivied_port_name = "";

   auto callback = [&](const std::string& name) -> bool
         {
            receivied_port_name = name;
            return true;
         };
   /**
    * <b>scenario</b>: Active port changed. <br>
    * <b>expected</b>: Callback with new port called. <br>
    * ************************************************
    */
   m_test_subject->subscribeForActivePortChangedEvent(callback);
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_itemText(&test_port_box, _)).WillOnce(Return(QString(port_name.c_str())));
   dynamic_cast<GUIController*>(m_test_subject.get())->onCurrentPortSelectionChanged(0);
   EXPECT_EQ(port_name, receivied_port_name);
}

TEST_F(IGUIControllerFixture, port_opening_and_closing)
{
   /**
    * <b>scenario</b>: Registering new opened port <br>
    * <b>expected</b>: Port added to combobox. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_port_box, QString("PORT1")));
   m_test_subject->registerPortOpened("PORT1");

   /**
    * <b>scenario</b>: Registering port closed <br>
    * <b>expected</b>: Port removed from combobox. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT1"))).WillOnce(Return(0));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_removeItem(&test_port_box, 0));
   m_test_subject->registerPortClosed("PORT1");

   /**
    * <b>scenario</b>: Registering port closed when port was not opened <br>
    * <b>expected</b>: No reaction. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_findText(&test_port_box, QString("PORT1"))).WillOnce(Return(-1));
   m_test_subject->registerPortClosed("PORT1");

}

TEST_F(IGUIControllerFixture, setting_command_history)
{
   /**
    * <b>scenario</b>: Setting command history <br>
    * <b>expected</b>: All items added to combobox. <br>
    * ************************************************
    */
   std::vector<std::string> commands = {"C1", "C2", "C3", "C4"};
   InSequence s1;
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString("C1")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString("C2")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString("C3")));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_insertItem(&test_text_edit, 0, QString("C4")));
   m_test_subject->setCommandsHistory(commands);
}

TEST_F(IGUIControllerFixture, getting_current_line_ending)
{
   /**
    * <b>scenario</b>: Request current line ending <br>
    * <b>expected</b>: Correct line ending returned. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_line_ending_box)).WillOnce(Return(QString("\n")));
   EXPECT_EQ(m_test_subject->getCurrentLineEnding(), "\n");
}

TEST_F(IGUIControllerFixture, adding_new_line_ending)
{
   /**
    * <b>scenario</b>: Adding new line ending <br>
    * <b>expected</b>: Line ending added to combobox. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_line_ending_box, QString("\n")));
   m_test_subject->addLineEnding("\n");
}

TEST_F(IGUIControllerFixture, setting_trace_filters)
{
   /**
    * <b>scenario</b>: setTraceFilter requested. <br>
    * <b>expected</b>: New trace filter set correctly. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_trace_filter_0, "TEST_FILTER"));
   m_test_subject->setTraceFilter(0, "TEST_FILTER");
}

TEST_F(IGUIControllerFixture, getting_trace_filters)
{
   /**
    * <b>scenario</b>: getTraceFilter requested. <br>
    * <b>expected</b>: Correct trace filter returned. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_trace_filter_0)).WillOnce(Return(QString("TEST_FILTER")));
   EXPECT_EQ(m_test_subject->getTraceFilter(0), "TEST_FILTER");
}

TEST_F(IGUIControllerFixture, setting_trace_filter_enabled)
{
   /**
    * <b>scenario</b>: setTraceFilterEnabled requested. <br>
    * <b>expected</b>: Line edit enabled state changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setEnabled(&test_trace_filter_0, false));
   m_test_subject->setTraceFilterEnabled(0, false);
}

TEST_F(IGUIControllerFixture, setting_trace_filter_background_color)
{
   /**
    * <b>scenario</b>: setTraceFilterBackgroundColor requested. <br>
    * <b>expected</b>: Line edit background color changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setStyleSheet(&test_trace_filter_0, _));
   m_test_subject->setTraceFilterBackgroundColor(0, 0xDEAD);
}

TEST_F(IGUIControllerFixture, setting_trace_filter_font_color)
{
   /**
    * <b>scenario</b>: setTraceFilterFontColor requested. <br>
    * <b>expected</b>: Line edit font color changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setStyleSheet(&test_trace_filter_0, _));
   m_test_subject->setTraceFilterFontColor(0, 0xDEAD);
}

TEST_F(IGUIControllerFixture, setting_port_label_text)
{
   /**
    * <b>scenario</b>: setPortLabelText requested. <br>
    * <b>expected</b>: Port label changed. <br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QLabel_setText(&test_port_label_0, QString("SOME DESC")));
   m_test_subject->setPortLabelText(0, "SOME DESC");
}

TEST_F(IGUIControllerFixture, theme_translation)
{
   /**
    * <b>scenario</b>: Translating from theme enum to name. <br>
    * <b>expected</b>: Corrent names in textual form returned. <br>
    * ************************************************
    */
   EXPECT_EQ(m_test_subject->themeToName(Theme::DEFAULT), "DEFAULT");
   EXPECT_EQ(m_test_subject->themeToName(Theme::DARK), "DARK");

   /**
    * <b>scenario</b>: Translating from theme name to enum. <br>
    * <b>expected</b>: Corrent names in textual form returned. <br>
    * ************************************************
    */
   EXPECT_EQ(m_test_subject->nameToTheme("DEFAULT"), Theme::DEFAULT);
   EXPECT_EQ(m_test_subject->nameToTheme("DARK"), Theme::DARK);
}

TEST_F(IGUIControllerFixture, theme_reload)
{
   ThemeListenerMock theme_listener;

   m_test_subject->subscribeForThemeReloadEvent(&theme_listener);
   m_test_subject->subscribeForThemeReloadEvent(nullptr); // to check verification of listener before call.

   /**
    * <b>scenario</b>: Reloading from default to dark theme. <br>
    * <b>expected</b>: Listener notified. <br>
    * ************************************************
    */
   EXPECT_CALL(theme_listener, onThemeChange(Theme::DARK));
   m_test_subject->reloadTheme(Theme::DARK);
   EXPECT_EQ(m_test_subject->currentTheme(), Theme::DARK);

   /**
    * <b>scenario</b>: Reloading from dark theme to default. <br>
    * <b>expected</b>: Listener notified. <br>
    * ************************************************
    */
   EXPECT_CALL(theme_listener, onThemeChange(Theme::DEFAULT));
   m_test_subject->reloadTheme(Theme::DEFAULT);
   EXPECT_EQ(m_test_subject->currentTheme(), Theme::DEFAULT);

   m_test_subject->unsubscribeFromThemeReloadEvent(&theme_listener);
}

