#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "ButtonCommandsExecutor.hpp"
#include "Logger.h"
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

struct ButtonCommandExecutorTests : public testing::Test
{
   void SetUp()
   {
      g_writer_mock = new WriterMock();
   }
   void TearDown()
   {
      delete g_writer_mock;
   }
};

TEST_F(ButtonCommandExecutorTests, executing_one_command)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
    * ************************************************
    */
   std::optional<bool> callback_result;
   auto callback = [&](bool result){ callback_result = result;};
   std::string raw_commands_text = "command1\n";
   ButtonCommandsExecutor test_subject (WriterFunction, raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write(raw_commands_text)).WillOnce(Return(true));
   test_subject.execute(callback);

}

TEST_F(ButtonCommandExecutorTests, executing_more_than_one_command)
{
   /**
    * <b>scenario</b>: Three commands added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
    * ************************************************
    */
   std::optional<bool> callback_result;
   auto callback = [&](bool result){ callback_result = result;};
   std::string raw_commands_text = "command1\ncommand2\ncommand3";
   ButtonCommandsExecutor test_subject (WriterFunction, raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1\n")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2\n")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3\n")).WillOnce(Return(true));
   test_subject.execute(callback);

}

TEST_F(ButtonCommandExecutorTests, commands_with_wait_added)
{
   /**
    * <b>scenario</b>: Three commands added by user, with waiting <br>
    * <b>expected</b>: Thread started, callback called.<br>
    * ************************************************
    */
   std::condition_variable condition_variable;
   std::mutex mutex;


   std::optional<bool> callback_result;
   auto callback = [&](bool result)
         {
            callback_result = result;
            condition_variable.notify_all();
         };
   std::string raw_commands_text = "command1\n__wait(100)\ncommand3";
   ButtonCommandsExecutor test_subject (WriterFunction, raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1\n")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3\n")).WillOnce(Return(true));
   test_subject.execute(callback);

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});
}

}
