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

TEST_F(ButtonCommandExecutorTests, no_commands_execution)
{
   /**
    * <b>scenario</b>: No commands added by user, executed requested <br>
    * <b>expected</b>: No crash, callback with false called.<br>
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
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.execute();

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});

   EXPECT_TRUE(callback_result.has_value());
   EXPECT_TRUE(callback_result.value());

}

TEST_F(ButtonCommandExecutorTests, executing_one_command)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Callback with true called.<br>
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
   std::string raw_commands_text = "command1";
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   test_subject.execute();

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});

   EXPECT_TRUE(callback_result.has_value());
   EXPECT_TRUE(callback_result.value());

}

TEST_F(ButtonCommandExecutorTests, executing_more_than_one_command)
{
   /**
    * <b>scenario</b>: Three commands added by user, executed correctly <br>
    * <b>expected</b>: Callback with true called.<br>
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
   std::string raw_commands_text = "command1\ncommand2\ncommand3";
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));
   test_subject.execute();

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});

   EXPECT_TRUE(callback_result.has_value());
   EXPECT_TRUE(callback_result.value());
}

TEST_F(ButtonCommandExecutorTests, executing_twice_the_same_command_set)
{
   /**
    * <b>scenario</b>: Three commands added by user, executed correctly two times <br>
    * <b>expected</b>: Callback with true called two times.<br>
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
   std::string raw_commands_text = "command1\ncommand2\ncommand3";
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));

   {
      /* first execution */
      test_subject.execute();

      std::unique_lock<std::mutex> lock(mutex);
      condition_variable.wait(lock, [&](){ return callback_result;});

      EXPECT_TRUE(callback_result.has_value());
      EXPECT_TRUE(callback_result.value());
   }

   {
      /* second execution */
      callback_result.reset();
      test_subject.execute();

      std::unique_lock<std::mutex> lock(mutex);
      condition_variable.wait(lock, [&](){ return callback_result;});

      EXPECT_TRUE(callback_result.has_value());
      EXPECT_TRUE(callback_result.value());
   }
}

TEST_F(ButtonCommandExecutorTests, commands_with_wait_added)
{
   /**
    * <b>scenario</b>: Three commands added by user, with waiting <br>
    * <b>expected</b>: Callback with true called.<br>
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
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));
   test_subject.execute();

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});

   EXPECT_TRUE(callback_result.has_value());
   EXPECT_TRUE(callback_result.value());
}

TEST_F(ButtonCommandExecutorTests, commands_with_wait_started_and_aborted)
{
   /**
    * <b>scenario</b>: Three commands added by user, one with long waiting, but application shutdown was requested. <br>
    * <b>expected</b>: Callback with false called. <br>
    *                  Object destruction not blocked, this means execution time lower than requested in command. <br>
    *                  Commands after __wait shall not be executed.<br>
    * ************************************************
    */
   std::condition_variable condition_variable;
   std::mutex mutex;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::optional<bool> callback_result;
   auto callback = [&](bool result)
         {
            callback_result = result;
            condition_variable.notify_all();
         };
   std::string raw_commands_text = "command1\n__wait(10000)\ncommand3";
   ButtonCommandsExecutor test_subject (WriterFunction, callback);
   test_subject.parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();
   test_subject.execute();

   std::this_thread::sleep_for(std::chrono::seconds(3));
   test_subject.stop();

   std::unique_lock<std::mutex> lock(mutex);
   condition_variable.wait(lock, [&](){ return callback_result;});

   execution_finish = std::chrono::high_resolution_clock::now();

   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();

   EXPECT_TRUE(callback_result.has_value());
   EXPECT_FALSE(callback_result.value());
   EXPECT_THAT(execution_time_ms, Lt(8000));
}

}
