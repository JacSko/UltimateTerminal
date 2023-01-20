#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <atomic>
#include <optional>
#include "UserButtonHandler.h"
#include "UserButtonDialogMock.h"
#include "Logger.h"
#include "GUIControllerMock.h"

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

const std::string TEST_BUTTON_NAME = "TEST_NAME";
constexpr uint32_t TEST_BUTTON_ID = 1;

struct UserButtonHandlerFixture : public testing::Test
{
   UserButtonHandlerFixture():
   test_controller(nullptr)
   {}

   void SetUp()
   {
      g_writer_mock = new WriterMock();
      UserButtonDialogMock_init();
      GUIControllerMock_init();
      EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));
      EXPECT_CALL(*GUIControllerMock_get(), setButtonCheckable(TEST_BUTTON_ID, true));
      m_test_subject.reset(new UserButtonHandler(test_controller, TEST_BUTTON_NAME, fake_persistence, WriterFunction));
      m_test_subject->startThread();

   }
   void TearDown()
   {
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _));
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));

      m_test_subject.reset(nullptr);
      GUIControllerMock_deinit();
      UserButtonDialogMock_deinit();
      delete g_writer_mock;
      g_writer_mock = nullptr;
   }

   std::unique_ptr<UserButtonHandler> m_test_subject;
   QPushButton test_button;
   GUIController test_controller;
   Persistence::PersistenceHandler fake_persistence;
   ButtonEventListener* m_button_listener;
};

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

   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, false));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, true)).WillOnce(Invoke([&](uint32_t,  bool)
         {
            test_wait = false;
         }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, false));
   EXPECT_CALL(*g_writer_mock, write(_)).Times(0);
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

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

   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, new_settings.button_name));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);
   /* request commands send */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, false));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, true)).WillOnce(Invoke([&](uint32_t,  bool)
         {
            test_wait = false;
         }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, false));
   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

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

   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, new_settings.button_name));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);


   /* request commands send */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, false));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonEnabled(TEST_BUTTON_ID, true)).WillOnce(Invoke([&](uint32_t,  bool)
         {
            test_wait = false;
         }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, true));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonChecked(TEST_BUTTON_ID, false));
   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(false));

   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED);

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

   EXPECT_CALL(*UserButtonDialogMock_get(), showDialog(_, _, _, true))
               .WillOnce(Invoke([&](QWidget*, const Dialogs::UserButtonDialog::Settings&, Dialogs::UserButtonDialog::Settings& out, bool)->std::optional<bool>
                     {
                        out = new_settings;
                        return true;
                     }));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, new_settings.button_name));
   m_button_listener->onButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED);

   /* simulate persistence write request */
   ASSERT_EQ(data_buffer.size(), 0);
   reinterpret_cast<Persistence::PersistenceListener*>(m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_THAT(data_buffer.size(), Gt(0));

   /* create new test object to test if persistence is read correctly */
   EXPECT_CALL(*GUIControllerMock_get(), getButtonID(TEST_BUTTON_NAME)).WillOnce(Return(TEST_BUTTON_ID));
   EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _)).WillOnce(SaveArg<2>(&m_button_listener));
   EXPECT_CALL(*GUIControllerMock_get(), subscribeForButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));
   EXPECT_CALL(*GUIControllerMock_get(), setButtonCheckable(TEST_BUTTON_ID, true));

   /* expect that button name will be set to value read from persistence */
   EXPECT_CALL(*GUIControllerMock_get(), setButtonText(TEST_BUTTON_ID, new_settings.button_name));
   std::unique_ptr<UserButtonHandler> new_test_subject = std::unique_ptr<UserButtonHandler>(new UserButtonHandler(test_controller, TEST_BUTTON_NAME, fake_persistence, WriterFunction));

   /* simulate persistence read notification */
   reinterpret_cast<Persistence::PersistenceListener*>(new_test_subject.get())->onPersistenceRead(data_buffer);

   EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TEST_BUTTON_ID, ButtonEvent::CLICKED, _));
   EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromButtonEvent(TEST_BUTTON_ID, ButtonEvent::CONTEXT_MENU_REQUESTED,_));

}

}
