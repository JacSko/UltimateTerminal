#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "QtWidgets/QtWidgetsMock.h"
#include <QtWidgets/QPushButton>

#include "TraceFilterHandler.h"


using namespace ::testing;

struct UserButtonHandlerFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();

      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("clicked"), _, HasSubstr("onButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onContextMenuRequested")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_line_edit, HasSubstr("customContextMenuRequested"), _, HasSubstr("onContextMenuRequested")));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&test_button, Qt::ContextMenuPolicy::CustomContextMenu));
      EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setContextMenuPolicy(&test_line_edit, Qt::ContextMenuPolicy::CustomContextMenu));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setCheckable(&test_button, true));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, 0)));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::red)));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, false));
      EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, test_default_color)));
      m_test_subject.reset(new TraceFilterHandler(&test_parent, &test_line_edit, &test_button, fake_persistence));

      Mock::VerifyAndClearExpectations(QtCoreMock_get());
      Mock::VerifyAndClearExpectations(QtWidgetsMock_get());
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }

   std::unique_ptr<TraceFilterHandler> m_test_subject;
   QLineEdit test_line_edit;
   QPushButton test_button;
   QDialog test_parent;
   Persistence::PersistenceHandler fake_persistence;
   QColor test_default_color = 0x00DEAD;
};

TEST_F(UserButtonHandlerFixture, filtering_requested_when_not_active)
{
   /**
    * <b>scenario</b>: Trace filter is disabled, but filtering was requested <br>
    * <b>expected</b>: std::optional shall not contain a value.<br>
    * ************************************************
    */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&test_line_edit)).WillOnce(Return(true));
   auto result = m_test_subject->tryMatch("some trace");
   EXPECT_FALSE(result.has_value());

}

TEST_F(UserButtonHandlerFixture, enabling_filter)
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
   QColor new_trace_color = 0x112233;
   std::string test_regex = "CDE";

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&test_line_edit)).WillOnce(Return(true)) // check before opening the color dialog
                                                                        .WillOnce(Return(true)) // check before enabling filter
                                                                        .WillOnce(Return(false)); // check during tryMatch

   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(new_trace_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_line_edit, QPalette(QPalette::ColorRole::Base, new_trace_color.rgb())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_line_edit));
   m_test_subject->onContextMenuRequested();

   /* filter enabling */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_line_edit)).WillOnce(Return(QString(test_regex.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   m_test_subject->onButtonClicked();

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value(), new_trace_color.rgb());

}

TEST_F(UserButtonHandlerFixture, disabling_filter)
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
   QColor new_trace_color = 0x112233;
   std::string test_regex = "CDE";

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&test_line_edit)).WillOnce(Return(true)) // check before opening the color dialog
                                                                        .WillOnce(Return(true)) // check before enabling filter
                                                                        .WillOnce(Return(false))// check during tryMatch
                                                                        .WillOnce(Return(false))// check during disabling filter
                                                                        .WillOnce(Return(true));// check during second tryMatch

   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(new_trace_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_line_edit, QPalette(QPalette::ColorRole::Base, new_trace_color.rgb())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_line_edit));
   m_test_subject->onContextMenuRequested();

   /* filter enabling */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_line_edit)).WillOnce(Return(QString(test_regex.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   m_test_subject->onButtonClicked();

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value(), new_trace_color.rgb());

   /* filter disabling */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, false));
   m_test_subject->onButtonClicked();

   result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_FALSE(result.has_value());

}

TEST_F(UserButtonHandlerFixture, color_change_requesting_when_filter_active)
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
   QColor new_trace_color = 0x112233;
   std::string test_regex = "CDE";

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&test_line_edit)).WillOnce(Return(true))  // check before opening the color dialog
                                                                        .WillOnce(Return(true))  // check before enabling filter
                                                                        .WillOnce(Return(false)) // check during tryMatch
                                                                        .WillOnce(Return(false));// check during color change

   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(new_trace_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_line_edit, QPalette(QPalette::ColorRole::Base, new_trace_color.rgb())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_line_edit));
   m_test_subject->onContextMenuRequested();

   /* filter enabling */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_line_edit)).WillOnce(Return(QString(test_regex.c_str())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   m_test_subject->onButtonClicked();

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value(), new_trace_color.rgb());

   /* color change requested, but not allowed */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).Times(0);
   m_test_subject->onContextMenuRequested();

}

TEST_F(UserButtonHandlerFixture, persistence_write_and_read)
{
   /**
    * <b>scenario</b>: Persistence write and read sequence to verify data serialization <br>
    * <b>expected</b>: Newly created object shall correctly process the persistence data.<br>
    * ************************************************
    */
   std::vector<uint8_t> data_buffer;
   QColor new_trace_color = 0x112233;
   std::string test_regex = "CDE";

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&test_line_edit)).WillOnce(Return(true)) // check before opening the color dialog
                                                                        .WillOnce(Return(true)) // check before enabling filter
                                                                        .WillOnce(Return(false))// check during tryMatch
                                                                        .WillOnce(Return(false));// check during persistence write
   /* color change */
   EXPECT_CALL(*QtWidgetsMock_get(), QColorDialog_getColor(_,_,_)).WillOnce(Return(new_trace_color));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_line_edit, QPalette(QPalette::ColorRole::Base, new_trace_color.rgb())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_line_edit));
   m_test_subject->onContextMenuRequested();

   /* filter enabling */
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_text(&test_line_edit)).WillOnce(Return(QString(test_regex.c_str()))) // filter enable
                                                                     .WillOnce(Return(QString(test_regex.c_str())));// persistence write
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_line_edit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&test_button, QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   m_test_subject->onButtonClicked();

   auto result = m_test_subject->tryMatch("ABCDEF");
   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value(), new_trace_color.rgb());

   /* simulate persistence write request */
   ASSERT_EQ(data_buffer.size(), 0);
   reinterpret_cast<Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));

   /* create new test subject */
   QLineEdit new_test_line_edit;
   QPushButton new_test_button;
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&new_test_button, HasSubstr("clicked"), _, HasSubstr("onButtonClicked")));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&new_test_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onContextMenuRequested")));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&new_test_line_edit, HasSubstr("customContextMenuRequested"), _, HasSubstr("onContextMenuRequested")));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&new_test_button, Qt::ContextMenuPolicy::CustomContextMenu));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setContextMenuPolicy(&new_test_line_edit, Qt::ContextMenuPolicy::CustomContextMenu));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setCheckable(&new_test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&new_test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, 0)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&new_test_button, QPalette(QPalette::ColorRole::Button, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&new_test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&new_test_line_edit, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&new_test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, test_default_color)));
   std::unique_ptr<TraceFilterHandler> new_test_subject = std::unique_ptr<TraceFilterHandler>(new TraceFilterHandler(&test_parent, &new_test_line_edit, &new_test_button, fake_persistence));

   /* expectations to be met when restoring from persistence */
   /* button state restoration */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&new_test_button)).WillOnce(Return(QPalette(QPalette::ColorRole::Button, 0)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&new_test_button, QPalette(QPalette::ColorRole::Button, Qt::green)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&new_test_button));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&new_test_button, true));
   /* Line edit restoration */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&new_test_line_edit, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_palette(&new_test_line_edit)).WillOnce(Return(QPalette(QPalette::ColorRole::Base, Qt::red)));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setPalette(&new_test_line_edit, QPalette(QPalette::ColorRole::Base, new_trace_color.rgb())));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_update(&new_test_line_edit));
   EXPECT_CALL(*QtWidgetsMock_get(), QLineEdit_setText(&new_test_line_edit, QString(test_regex.c_str())));
   reinterpret_cast<Persistence::PersistenceListener*>(new_test_subject.get())->onPersistenceRead(data_buffer);

   /* try to match the same text as before persistence write */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_isEnabled(&new_test_line_edit)).WillOnce(Return(false)); // check during tryMatch
   result = new_test_subject->tryMatch("ABCDEF");

   EXPECT_TRUE(result.has_value());
   EXPECT_EQ(result.value(), new_trace_color.rgb());

}

