#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <atomic>
#include <optional>
#include "UserButtonHandler.h"
#include "UserButtonDialogMock.h"
#include "Logger.h"

#include "QtWidgets/QtWidgetsMock.h"
#include <QtWidgets/QPushButton>

namespace GUI
{

using namespace ::testing;

struct WriterMock
{
   MOCK_METHOD1(write, bool(const std::string&));
};

WriterMock* g_writer_mock;

auto WriterFunction = [](const std::string& str)->bool
      {
         UT_Assert(g_writer_mock && "create writer mock");
         return g_writer_mock->write(str);
      };

struct UserButtonHandlerFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
      g_writer_mock = new WriterMock();
      UserButtonDialogMock_init();

      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&test_button, Qt::ContextMenuPolicy::CustomContextMenu));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("clicked"), _, HasSubstr("onUserButtonClicked")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&test_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onUserButtonContextMenuRequested")));
      EXPECT_CALL(*QtCoreMock_get(), QObject_connect(_, HasSubstr("commandsFinished"), _, HasSubstr("onCommandsFinished")));
      EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setCheckable(&test_button, true));

      m_test_subject.reset(new UserButtonHandler(&test_button, nullptr, fake_persistence, WriterFunction));
      m_test_subject->startThread();

   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      UserButtonDialogMock_deinit();
      delete g_writer_mock;
      g_writer_mock = nullptr;
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }

   std::unique_ptr<UserButtonHandler> m_test_subject;
   QPushButton test_button;
   Persistence::PersistenceHandler fake_persistence;
};

/* implementation of signal emited on commands execution finish */
void UserButtonHandler::commandsFinished()
{
   onCommandsFinished();
}

TEST_F(UserButtonHandlerFixture, empty_commands_list_when_execution_requested)
{
   /**
    * <b>scenario</b>: User requested to send commands, but no commands entered by user <br>
    * <b>expected</b>: Button should be deactivated on command execution start.<br>
    *                  No commands sent <br>
    *                  Button should be activated on command execution finish. <br>
    * ************************************************
    */
   std::atomic<bool> test_wait (true);

   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, false)).WillOnce(Invoke([&](QWidget*, bool)
         {
            test_wait = false;
         }));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_repaint(&test_button)).Times(2);
   EXPECT_CALL(*g_writer_mock, write(_)).Times(0);

   m_test_subject->onUserButtonClicked();

   while(test_wait)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
   }

}

TEST_F(UserButtonHandlerFixture, commands_sending_requested)
{
   /**
    * <b>scenario</b>: User requested to send commands  <br>
    * <b>expected</b>: Button should be deactivated on command execution start.<br>
    *                  All commands sent <br>
    *                  Button should be activated on command execution finish. <br>
    * ************************************************
    */

   std::atomic<bool> test_wait (true);

   /* simulate user settings change */
   Dialogs::UserButtonDialog::Settings new_settings;
   new_settings.button_name = "some button name";
   new_settings.raw_commands = "command1\ncommand2\n__wait(100)\ncommand3\n";

   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(new_settings.button_name.c_str())));
   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));

   m_test_subject->onUserButtonContextMenuRequested();

   /* request commands send */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, false))
               .WillOnce(Invoke([&](QPushButton*, bool)
                     {
                        test_wait = false;
                     }));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_repaint(&test_button)).Times(2);
   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));


   m_test_subject->onUserButtonClicked();

   while(test_wait)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
   }
}

TEST_F(UserButtonHandlerFixture, failed_to_send_commands)
{
   /**
    * <b>scenario</b>: User requested to send commands, but sending fails <br>
    * <b>expected</b>: Button should be deactivated on command execution start.<br>
    *                  Sending should be aborted on first fail <br>
    *                  Button should be activated on command execution finish. <br>
    * ************************************************
    */

   std::atomic<bool> test_wait (true);

   /* simulate user settings change */
   Dialogs::UserButtonDialog::Settings new_settings;
   new_settings.button_name = "some button name";
   new_settings.raw_commands = "command1\ncommand2\n__wait(100)\ncommand3\n";

   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(new_settings.button_name.c_str())));
   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));

   m_test_subject->onUserButtonContextMenuRequested();

   /* request commands send */
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, true));
   EXPECT_CALL(*QtWidgetsMock_get(), QWidget_setDisabled(&test_button, false));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setChecked(&test_button, false))
               .WillOnce(Invoke([&](QPushButton*, bool)
                     {
                        test_wait = false;
                     }));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_repaint(&test_button)).Times(2);
   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(false));


   m_test_subject->onUserButtonClicked();

   while(test_wait)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
   }
}

TEST_F(UserButtonHandlerFixture, persistence_read_write)
{
   /**
    * <b>scenario</b>: Persistence write and read requested <br>
    * <b>expected</b>: Data should be correctly serialized.<br>
    *                  New test subject created <br>
    *                  Data should be correctly deserialized. <br>
    * ************************************************
    */
   std::vector<uint8_t> data_buffer;

   /* simulate user settings change */
   Dialogs::UserButtonDialog::Settings new_settings;
   new_settings.button_name = "some button name";
   new_settings.raw_commands = "command1\ncommand2\n__wait(100)\ncommand3\n";

   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&test_button, QString(new_settings.button_name.c_str())));
   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));

   m_test_subject->onUserButtonContextMenuRequested();

   /* simulate persistence write request */
   ASSERT_EQ(data_buffer.size(), 0);
   reinterpret_cast<Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));


   /* create new test object to test if persistence is read correctly */
   QPushButton new_test_button;
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setContextMenuPolicy(&new_test_button, Qt::ContextMenuPolicy::CustomContextMenu));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&new_test_button, HasSubstr("clicked"), _, HasSubstr("onUserButtonClicked")));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(&new_test_button, HasSubstr("customContextMenuRequested"), _, HasSubstr("onUserButtonContextMenuRequested")));
   EXPECT_CALL(*QtCoreMock_get(), QObject_connect(_, HasSubstr("commandsFinished"), _, HasSubstr("onCommandsFinished")));
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setCheckable(&new_test_button, true));

   /* expect that button name will be set to value read from persistence */
   EXPECT_CALL(*QtWidgetsMock_get(), QPushButton_setText(&new_test_button, QString(new_settings.button_name.c_str())));

   std::unique_ptr<UserButtonHandler> new_test_subject = std::unique_ptr<UserButtonHandler>(new UserButtonHandler(&new_test_button, nullptr, fake_persistence, WriterFunction));

   /* simulate persistence read notification */
   reinterpret_cast<Persistence::PersistenceListener*>(new_test_subject.get())->onPersistenceRead(data_buffer);




}

}
