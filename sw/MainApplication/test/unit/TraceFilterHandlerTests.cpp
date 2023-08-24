#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "TraceFilterSettingDialogMock.h"
#include "TraceFilterHandler.h"
#include "GUIControllerMock.h"

using namespace ::testing;

const std::string TEST_BUTTON_NAME = "TEST_NAME";
constexpr uint32_t TEST_BUTTON_ID = 1;
constexpr uint32_t TEST_FILTER_ID = 0;
constexpr uint32_t GREEN_COLOR = 0x00FF00;
constexpr uint32_t BLACK_COLOR = 0x000000;
constexpr uint32_t BUTTON_DEFAULT_BACKGROUND_COLOR = 0x111111;
constexpr uint32_t BUTTON_DEFAULT_FONT_COLOR = 0x222222;

struct TraceFilterHandlerTests : public testing::Test
{
   TraceFilterHandlerTests():
   test_controller(nullptr)
   {
   }
   void SetUp()
   {
      TraceFilterSettingDialogMock_init();
      GUIControllerMock_init();

      test_palette.setColor(QPalette::Base, QColor(0x000000));
      test_palette.setColor(QPalette::Text, QColor(0x998877));
      EXPECT_CALL(*GUIControllerMock_get(), getApplicationPalette()).WillOnce(Return(test_palette))
                                                                    .WillOnce(Return(test_palette));
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonCheckable(TEST_BUTTON_ID, true));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, true));
      EXPECT_CALL(*GUIControllerMock_get(), getBackgroundColor()).WillRepeatedly(Return(BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), getTextColor()).WillRepeatedly(Return(BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, test_palette.color(QPalette::Base).rgb()));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, test_palette.color(QPalette::Text).rgb()));
      m_test_subject.reset(new TraceFilterHandler(TEST_FILTER_ID, test_controller, TEST_BUTTON_NAME, fake_persistence));
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      GUIControllerMock_deinit();
      TraceFilterSettingDialogMock_deinit();
   }

   std::unique_ptr<TraceFilterHandler> m_test_subject;
   Persistence::PersistenceHandler fake_persistence;
   GUIController test_controller;
   QPalette test_palette;
   ButtonEventListener* m_button_listener;
};

TEST_F(TraceFilterHandlerTests, filtering_requested_when_not_active)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, but filtering was requested <br>
    * <b>expected</b>: std::optional shall not contain a value.<br>
    * ************************************************
    */
   ASSERT_FALSE(m_test_subject->isActive());
   auto result = m_test_subject->tryMatch("some trace");
   EXPECT_FALSE(result.has_value());

}

TEST_F(TraceFilterHandlerTests, enabling_filter)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, user requested to change the color, then filter was activated and correct text was tested <br>
    * <b>expected</b>: Color dialog shall be presented.<br>
    *                  QLineEdit background color shall be changed to new color.<br>
    *                  On filter activation, QPushButton color shall be changed to GREEN.<br>
    *                  std::optional shall contain a value.<br>
    *                  std::optional shall contain filter color value.<br>
    * ************************************************
    */
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(TEST_FILTER_ID, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(TEST_FILTER_ID, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(TEST_FILTER_ID, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* filter enabling */
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(TEST_FILTER_ID, false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

}

TEST_F(TraceFilterHandlerTests, disabling_filter)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, user requested to change the color, then filter was activated and correct text was tested. <br>
    *                  User requested to disable filter. <br>
    * <b>expected</b>: Color dialog shall be presented.<br>
    *                  QLineEdit background color shall be changed to new color.<br>
    *                  On filter activation, QPushButton color shall be changed to GREEN and QLineEdit shall be disabled.<br>
    *                  std::optional shall contain a value.<br>
    *                  std::optional shall contain filter color value.<br>
    *                  On filter deactivation, QPushButton color shall be changed to RED and QLineEdit shall be enabled.<br>
    *                  std::optional shall NOT contain a value.<br>
    * ************************************************
    */
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* filter enabling */
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

   /* filter disabling */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, true));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_FALSE(result.has_value());

}

TEST_F(TraceFilterHandlerTests, color_change_requesting_when_filter_active)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, user requested to change the color, then filter was activated and correct text was tested <br>
    *                  User requests to change the color when filter is active. <br>
    * <b>expected</b>: Color dialog shall be presented.<br>
    *                  QLineEdit background color shall be changed to new color.<br>
    *                  On filter activation, QPushButton color shall be changed to GREEN.<br>
    *                  std::optional shall contain a value.<br>
    *                  std::optional shall contain filter color value.<br>
    *                  On color change request, the color dialog shall not be displayed.<br>
    * ************************************************
    */
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* filter enabling */
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

   /* color change requested, but not allowed */
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,_)).Times(0);
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);
}

TEST_F(TraceFilterHandlerTests, persistence_write_and_read)
{
   /**
    * <b>scenario</b>: Persistence write and read sequence to verify data serialization <br>
    * <b>expected</b>: Newly created object shall correctly process the persistence data.<br>
    * ************************************************
    */
   Persistence::PersistenceListener::PersistenceItems data_buffer;
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* filter enabling */
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

   /* simulate persistence write request */
   ASSERT_EQ(data_buffer.size(), 0);
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
   reinterpret_cast<Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));

   /* create new test subject */
   EXPECT_CALL(*GUIControllerMock_get(), getApplicationPalette()).WillOnce(Return(test_palette))
                                                                 .WillOnce(Return(test_palette));
   EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
   EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
   EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonCheckable(TEST_BUTTON_ID, true));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, test_palette.color(QPalette::Base).rgb()));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, test_palette.color(QPalette::Text).rgb()));
   std::unique_ptr<TraceFilterHandler> new_test_subject = std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(0, test_controller, TEST_BUTTON_NAME, fake_persistence));

   /* expectations to be met when restoring from persistence */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   reinterpret_cast<Persistence::PersistenceListener*>(new_test_subject.get())->onPersistenceRead(data_buffer);

   /* try to match the same text as before persistence write */
   result = new_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

}

TEST_F(TraceFilterHandlerTests, refresh_ui_when_filter_disabled)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, colors are default, UI refresh requested <br>
    * <b>expected</b>: QLineEdit color updated to the new theme color. <br>
    *                  QPushButton color updated to the new theme color. <br>
    * ************************************************
    */
   EXPECT_CALL(*GUIControllerMock_get(), getApplicationPalette()).WillOnce(Return(test_palette))
                                                                 .WillOnce(Return(test_palette));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, test_palette.color(QPalette::Base).rgb()));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, test_palette.color(QPalette::Text).rgb()));
   m_test_subject->refreshUi();
}

TEST_F(TraceFilterHandlerTests, refresh_ui_when_user_colors_used)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, colors are defined by user, UI refresh requested <br>
    * <b>expected</b>: QLineEdit color not changed. <br>
    *                  QPushButton color not changed. <br>
    * ************************************************
    */
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   m_test_subject->refreshUi();
}

TEST_F(TraceFilterHandlerTests, refresh_ui_when_filer_active)
{
   /**
    * <b>scenario</b>: Trace filter is enabled, colors are defined by user, UI refresh requested <br>
    * <b>expected</b>: QLineEdit color not changed. <br>
    *                  QPushButton color not changed. <br>
    * ************************************************
    */
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = 0x112233;
   settings.font = 0x332211;
   settings.regex = "CDE";

   EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
   EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* filter enabling */
   EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value().background, settings.background);
   EXPECT_EQ(result.value().font, settings.font);

   EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
   EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
   m_test_subject->refreshUi();

}

TEST_F(TraceFilterHandlerTests, settings_set_and_get_tests)
{
   uint32_t TEST_BACKGROUND_COLOR = 0x112233;
   uint32_t TEST_FONT_COLOR = 0x332211;
   std::string TEST_REGEX = "TEST_REGEX";
   uint32_t TEST_NEW_BACKGROUND_COLOR = 0x998877;
   uint32_t TEST_NEW_FONT_COLOR = 0x778899;
   std::string TEST_NEW_REGEX = "TEST_NEW_REGEX";
   Dialogs::TraceFilterSettingDialog::Settings settings;
   settings.id = TEST_FILTER_ID;
   settings.background = TEST_BACKGROUND_COLOR;
   settings.font = TEST_FONT_COLOR;
   settings.regex = TEST_REGEX;

   Dialogs::TraceFilterSettingDialog::Settings new_settings;
   settings.id = TEST_FILTER_ID;
   new_settings.background = TEST_NEW_BACKGROUND_COLOR;
   new_settings.font = TEST_NEW_FONT_COLOR;
   new_settings.regex = TEST_NEW_REGEX;

   {
      /**
       * <b>scenario</b>: Settings change requested when filtering active <br>
       * <b>expected</b>: New colors not accepted <br>
       * ************************************************
       */
      EXPECT_CALL(*GUIControllerMock_get(), getParent()).WillOnce(Return(nullptr));
      EXPECT_CALL(*TraceFilterSettingDialogMock_get(), showDialog(_,_,_,true)).WillOnce(DoAll(SetArgReferee<2>(settings),Return(true)));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, settings.background));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, settings.font));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, settings.regex));
      EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(TEST_FILTER_ID)).WillOnce(Return(settings.regex));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

      /* filter enabling */
      EXPECT_CALL(*GUIControllerMock_get(), getTraceFilter(_)).WillOnce(Return(settings.regex));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, GREEN_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BLACK_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, false));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

      /* filter enabled, check current settings */
      Dialogs::TraceFilterSettingDialog::Settings current_settings = m_test_subject->getSettings();
      EXPECT_EQ(current_settings.background, TEST_BACKGROUND_COLOR);
      EXPECT_EQ(current_settings.font, TEST_FONT_COLOR);
      EXPECT_EQ(current_settings.regex, TEST_REGEX);

      /* try to set new settings */
      EXPECT_FALSE(m_test_subject->setSettings(new_settings));

      /* expect old settings once again */
      current_settings = m_test_subject->getSettings();
      EXPECT_EQ(current_settings.background, TEST_BACKGROUND_COLOR);
      EXPECT_EQ(current_settings.font, TEST_FONT_COLOR);
      EXPECT_EQ(current_settings.regex, TEST_REGEX);
   }

   {
      /**
       * <b>scenario</b>: Settings change requested when filtering inactive <br>
       * <b>expected</b>: New colors accepted <br>
       * ************************************************
       */

      /* filter disabling */
      EXPECT_CALL(*GUIControllerMock_get(), setButtonBackgroundColor(TEST_BUTTON_ID, BUTTON_DEFAULT_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonFontColor(TEST_BUTTON_ID, BUTTON_DEFAULT_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterEnabled(_, true));
      m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterBackgroundColor(_, TEST_NEW_BACKGROUND_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilterFontColor(_, TEST_NEW_FONT_COLOR));
      EXPECT_CALL(*GUIControllerMock_get(), setTraceFilter(_, TEST_NEW_REGEX));
      EXPECT_TRUE(m_test_subject->setSettings(new_settings));

      /* expect new settings once again */
      Dialogs::TraceFilterSettingDialog::Settings current_settings = m_test_subject->getSettings();
      EXPECT_EQ(current_settings.background, TEST_NEW_BACKGROUND_COLOR);
      EXPECT_EQ(current_settings.font, TEST_NEW_FONT_COLOR);
      EXPECT_EQ(current_settings.regex, TEST_NEW_REGEX);
   }
}
