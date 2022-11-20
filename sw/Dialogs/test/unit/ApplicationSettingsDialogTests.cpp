#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "PortSettingDialogMock.h"
#include "TraceFilterSettingDialogMock.h"
#include "LoggingSettingDialogMock.h"
#include "PortHandlerMock.h"
#include "TraceFilterHandlerMock.h"
#include "IFileLoggerMock.h"
#include "ITimersMock.h"
#include "PersistenceHandler.h"
#include "IThemeControllerMock.h"
#include "Logger.h"
#include "Settings.h"
#include "SerialDriverMock.h"

#include "ApplicationSettingsDialog.h"

/**
 * @file ApplicationSettingsDialogTests.cpp
 *
 * @brief
 *    Class to test behavior of ApplicationSettingsDialog class.
 *
 * @author Jacek Skowronek
 * @date   18-11-2022
 *
 */

using namespace ::testing;

Utilities::ITimersMock* g_timers_mock;
IFileLoggerMock* g_logger_mock;

std::unique_ptr<IFileLogger> IFileLogger::create()
{
   return std::unique_ptr<IFileLogger>(g_logger_mock);
}

const uint8_t PORT_HANDLERS_COUNT = 5;
const uint8_t TRACE_FILTERS_COUNT = 7;

namespace Dialogs
{

struct ApplicationSettingsDialogFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      PortSettingDialogMock_init();
      TraceFilterSettingDialogMock_init();
      LoggingSettingDialogMock_init();
      GUI::PortHandlerMock_init();
      TraceFilterHandlerMock_init();
      g_timers_mock = new Utilities::ITimersMock;
      g_logger_mock = new IFileLoggerMock;

      for (uint8_t i = 0; i < PORT_HANDLERS_COUNT; i++)
      {
         test_handlers.emplace_back(std::unique_ptr<GUI::PortHandler>(
             new GUI::PortHandler(nullptr, nullptr, nullptr, *g_timers_mock, nullptr, nullptr, test_persistence)));
      }
      for(uint8_t i = 0; i < TRACE_FILTERS_COUNT; i++)
      {
         test_filters.emplace_back(std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(nullptr, nullptr, nullptr, test_persistence)));
      }
      test_file_logger = IFileLogger::create();
      m_test_subject.reset(new Dialogs::ApplicationSettingsDialog(test_handlers, test_filters, test_file_logger, test_logging_path, test_theme_controller));
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      delete g_timers_mock;
      TraceFilterHandlerMock_deinit();
      GUI::PortHandlerMock_deinit();
      LoggingSettingDialogMock_deinit();
      TraceFilterSettingDialogMock_deinit();
      PortSettingDialogMock_deinit();
      QtCoreMock_deinit();
      QtWidgetsMock_deinit();
   }

   QMainWindow m_parent;

   std::vector<std::unique_ptr<GUI::PortHandler>> test_handlers;
   std::vector<std::unique_ptr<TraceFilterHandler>> test_filters;
   std::unique_ptr<IFileLogger> test_file_logger;
   std::string test_logging_path;
   Persistence::PersistenceHandler test_persistence;
   IThemeControllerMock test_theme_controller;
   std::unique_ptr<Dialogs::ApplicationSettingsDialog> m_test_subject;

};

TEST_F(ApplicationSettingsDialogFixture, dialog_presented_items_changed)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user, settings in different tabs changed and dialog was accepted <br>
    *       Changed settings: PortSettings for ports 3 and 5, TraceSettings for filters 2 and 3.<br>
    * <b>expected</b>: <br>
    *       Dialog shall be correctly rendered.<br>
    *       Only changed settings shall be propagated to modules. <br>
    * ************************************************
    */

   const uint8_t new_port_settings_id = 3;
   const uint8_t new_trace_filter_settings_id = 4;

   QDialog test_dialog;
   QTabWidget test_main_tab;
   QDialogButtonBox test_buttonbox;

   QTabWidget test_ports_tab;
   QTabWidget test_filters_tab;
   QTabWidget test_debug_tab;

   QScrollArea test_settings_scroll;
   QComboBox test_logger_item;
   QFormLayout test_general_layout;
   QComboBox test_theme_combobox;
   QFormLayout test_logger_layout;
   QFormLayout test_setting_layout;
   QLineEdit test_setting_item;

   /* current settings */
   test_logging_path = "TEST_PATH";
   Dialogs::PortSettingDialog::Settings port_settings = {};
   port_settings.type = Dialogs::PortSettingDialog::PortType::SERIAL;
   Dialogs::TraceFilterSettingDialog::Settings trace_settings = {};
   trace_settings.background = 0x000000;
   trace_settings.font = 0x000001;
   trace_settings.regex = "OLD_REGEX";
   /* new settings */
   std::string test_new_logging_path = "TEST_NEW_PATH";
   Dialogs::PortSettingDialog::Settings new_port_settings = {};
   new_port_settings.type = Dialogs::PortSettingDialog::PortType::ETHERNET;
   Dialogs::TraceFilterSettingDialog::Settings new_trace_settings = {};
   new_trace_settings.background = 0x000002;
   new_trace_settings.font = 0x000003;
   new_trace_settings.regex = "NEW_REGEX";

   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_new()).WillOnce(Return(&test_dialog));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_new()).WillOnce(Return(&test_main_tab))
                                                      .WillOnce(Return(&test_ports_tab))
                                                      .WillOnce(Return(&test_filters_tab))
                                                      .WillOnce(Return(&test_debug_tab));
   EXPECT_CALL(*QtWidgetsMock_get(), QDialogButtonBox_new()).WillOnce(Return(&test_buttonbox));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_new()).WillOnce(Return(&test_general_layout))
                                                       .WillOnce(Return(&test_logger_layout))
                                                       .WillOnce(Return(&test_setting_layout));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_new()).WillOnce(Return(&test_theme_combobox))
                                                     .WillRepeatedly(Return(&test_logger_item));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_new()).WillRepeatedly(Return(&test_setting_item));

   /* expect connecting the dialog button signals */
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"accepted()",&test_dialog,"accept()"));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_buttonbox,"rejected()",&test_dialog,"reject()"));

   /* expect GENERAL tab fill-in */
   EXPECT_CALL(test_theme_controller, themeToName(_)).Times(AtLeast((uint8_t)IThemeController::Theme::APPLICATION_THEMES_MAX + 1));
   EXPECT_CALL(test_theme_controller, currentTheme()).WillOnce(Return(IThemeController::Theme::DARK))
                                                     .WillOnce(Return(IThemeController::Theme::DEFAULT));
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_theme_combobox,_)).Times((uint8_t)IThemeController::Theme::APPLICATION_THEMES_MAX);
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_theme_combobox, _));
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_general_layout, _, &test_theme_combobox));

   /* expect PORTS subtabs creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_ports_tab,_,HasSubstr("PORT"))).Times(PORT_HANDLERS_COUNT);
   EXPECT_CALL(*GUI::PortHandlerMock_get(), getSettings(_)).WillRepeatedly(ReturnRef(port_settings));
   EXPECT_CALL(*GUI::PortHandlerMock_get(), isOpened(_)).WillRepeatedly(Return(false));
   EXPECT_CALL(*PortSettingDialogMock_get(), createLayout(_,_,_)).WillRepeatedly(Return(nullptr));

   /* expect FILTERS subtabs creation */
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_filters_tab,_,HasSubstr("FILTER"))).Times(TRACE_FILTERS_COUNT);
   EXPECT_CALL(*TraceFilterHandlerMock_get(), getSettings()).WillRepeatedly(Return(trace_settings));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), isActive()).WillRepeatedly(Return(false));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), createLayout(_,_,_)).WillRepeatedly(Return(nullptr));

   /* expect file logging tab creation */
   EXPECT_CALL(*LoggingSettingDialogMock_get(), createLayout(_,_)).WillRepeatedly(Return(nullptr));
   EXPECT_CALL(*g_logger_mock, isActive()).WillOnce(Return(false));

   /* expect Debug subtabs creation, and their layout */
   EXPECT_CALL(*QtWidgetsMock_get(), QScrollArea_new()).WillOnce(Return(&test_settings_scroll));
   EXPECT_CALL(*QtWidgetsMock_get(), QScrollArea_setWidget(_,_));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_debug_tab,_,HasSubstr("LOGGING")));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_debug_tab,&test_settings_scroll,HasSubstr("SETTINGS")));

   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_logger_layout, _, _)).Times(LOGGER_GROUP_MAX);
   EXPECT_CALL(*QtWidgetsMock_get(), QFormLayout_addRow(&test_setting_layout, _, _)).Times(SETTING_GROUP_MAX);
   /* expect creation of comboboxes to present current logging settings */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_addItem(&test_logger_item, _)).Times(LOGGER_GROUP_MAX * LOGGER_LEVEL_MAX);
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_setCurrentText(&test_logger_item, _)).Times(LOGGER_GROUP_MAX);
   /* expect creation of comboboxes to present current system settings */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&test_setting_item, _)).Times(SETTING_GROUP_MAX);

   /* expect all tab added to main tab view */
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_main_tab,_,QString("GENERAL")));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_main_tab,&test_ports_tab,QString("PORTS")));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_main_tab,&test_filters_tab,QString("FILTERS")));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_main_tab,_,QString("FILE LOGGING")));
   EXPECT_CALL(*QtWidgetsMock_get(), QTabWidget_addTab(&test_main_tab,&test_debug_tab,HasSubstr("DEBUG")));

   /* user accepted the dialog */
   EXPECT_CALL(*QtWidgetsMock_get(), QDialog_exec(&test_dialog)).WillOnce(Return(QDialog::Accepted));

   /* expect readout of theme */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_theme_combobox)).WillOnce(Return("DARK"));
   EXPECT_CALL(test_theme_controller, nameToTheme(_)).WillOnce(Return(IThemeController::Theme::DARK));
   EXPECT_CALL(test_theme_controller, reloadTheme(_));

   /* expect settings readout for all ports, two of them returned the new settings */
   EXPECT_CALL(*PortSettingDialogMock_get(), convertGuiValues(new_port_settings_id,_)).WillOnce(DoAll(SetArgReferee<1>(new_port_settings), Return(true)));
   EXPECT_CALL(*PortSettingDialogMock_get(), convertGuiValues(Ne(new_port_settings_id),_)).WillRepeatedly(DoAll(SetArgReferee<1>(port_settings), Return(true)));

   /* expect settings readout for all filters */
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), convertGuiValues(new_trace_filter_settings_id, _)).WillOnce(DoAll(SetArgReferee<1>(new_trace_settings), Return(true)));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), convertGuiValues(Ne(new_trace_filter_settings_id), _)).WillRepeatedly(DoAll(SetArgReferee<1>(trace_settings), Return(true)));
   /* expect settings readout for file logging */
   EXPECT_CALL(*LoggingSettingDialogMock_get(), convertGuiValues()).WillOnce(Return(test_new_logging_path));

   /* expect readout of logging settings */
   EXPECT_CALL(*QtWidgetsMock_get(), QComboBox_currentText(&test_logger_item)).WillRepeatedly(Return("HIGH"));

   /* expect readout of system settings */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_setting_item)).WillRepeatedly(Return("0"));

   /* expect propagation of new settings */
   EXPECT_CALL(*GUI::PortHandlerMock_get(), setSettings(new_port_settings_id,_)).WillOnce(Return(true));
   EXPECT_CALL(*TraceFilterHandlerMock_get(), setSettings(new_trace_filter_settings_id,_)).WillOnce(Return(true));

   std::optional<bool> result = m_test_subject->showDialog(&m_parent);

   ASSERT_TRUE(result.has_value());
   EXPECT_TRUE(result.value());
   EXPECT_EQ(test_logging_path, test_new_logging_path);
}

}
