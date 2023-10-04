#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "ButtonCommandsExecutor.hpp"
#include "Logger.h"
namespace MainApplication
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
      auto callback = [&](bool result)
         {
            std::unique_lock<std::mutex> lock(mutex);
            m_callbackResult = result;
            conditionVariable.notify_all();
         };
     m_testSubject = std::unique_ptr<ButtonCommandsExecutor>(new ButtonCommandsExecutor(WriterFunction, callback));
   }
   void TearDown()
   {
      delete g_writer_mock;
   }
   std::optional<bool> executeAndWaitResult()
   {
      std::unique_lock<std::mutex> lock(mutex);
      m_callbackResult.reset();
      m_testSubject->execute();
      conditionVariable.wait_for(lock, std::chrono::seconds(10), [&](){ return m_callbackResult.has_value();});
      return m_callbackResult;
   }
   std::optional<bool> m_callbackResult;
   std::condition_variable conditionVariable;
   std::mutex mutex;
   std::unique_ptr<ButtonCommandsExecutor> m_testSubject;
};

TEST_F(ButtonCommandExecutorTests, noCommandsAdded_NoCommandsSentOnTrigger)
{
   /**
    * <b>scenario</b>: No commands added by user, executed requested <br>
    * <b>expected</b>: No crash, callback with false called.<br>
    * ************************************************
    */

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());

}

TEST_F(ButtonCommandExecutorTests, oneCommandAdded_oneCommandSent)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Callback with true called.<br>
    * ************************************************
    */
   std::string raw_commands_text = "command1";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());

}

TEST_F(ButtonCommandExecutorTests, moreThanOneCommandAdded_allCommandsSent)
{
   /**
    * <b>scenario</b>: Three commands added by user, executed correctly <br>
    * <b>expected</b>: Callback with true called.<br>
    * ************************************************
    */
   std::string raw_commands_text = "command1\ncommand2\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, buttonPressedTwoTimes_allCommandsSentTwice)
{
   /**
    * <b>scenario</b>: Three commands added by user, executed correctly two times <br>
    * <b>expected</b>: Callback with true called two times.<br>
    * ************************************************
    */
   std::string raw_commands_text = "command1\ncommand2\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true))
                                                   .WillOnce(Return(true));

   {
      /* first execution */
      auto callbackResult = executeAndWaitResult();
      EXPECT_TRUE(callbackResult.has_value());
      EXPECT_TRUE(callbackResult.value());
   }
   {
      /* second execution */
      auto callbackResult = executeAndWaitResult();
      EXPECT_TRUE(callbackResult.has_value());
      EXPECT_TRUE(callbackResult.value());
   }
}

TEST_F(ButtonCommandExecutorTests, commandWithWaitAdded_waitExecuted)
{
   /**
    * <b>scenario</b>: Three commands added by user, with waiting <br>
    * <b>expected</b>: Callback with true called.<br>
    * ************************************************
    */
   std::string raw_commands_text = "command1\n__wait(100)\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, waitWithNegativeValueAdded_commandsExecutedWithoutWait)
{
   /**
    * <b>scenario</b>: Wait command was added, but has invalid parameter - value is negative <br>
    * <b>expected</b>: Wait shall be skipped, only normal commands shall be sent.<br>
    * ************************************************
    */
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::string raw_commands_text = "command1\n__wait(-5000)\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();
   auto callbackResult = executeAndWaitResult();
   execution_finish = std::chrono::high_resolution_clock::now();
   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
   EXPECT_THAT(execution_time_ms, Lt(3000));
}

TEST_F(ButtonCommandExecutorTests, waitWithTextArgumentAdded_commandsExecutedWithoutWait)
{
   /**
    * <b>scenario</b>: Wait command was added, but has invalid parameter - value is textual <br>
    * <b>expected</b>: Wait shall be skipped, only normal commands shall be sent.<br>
    * ************************************************
    */
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::string raw_commands_text = "command1\n__wait(aabbcc5000)\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();
   auto callbackResult = executeAndWaitResult();
   execution_finish = std::chrono::high_resolution_clock::now();
   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
   EXPECT_THAT(execution_time_ms, Lt(3000));
}

TEST_F(ButtonCommandExecutorTests, commandsWithWaitAbortedByUser_waitShallBeInterrupted)
{
   /**
    * <b>scenario</b>: Three commands added by user, one with long waiting, but application shutdown was requested. <br>
    * <b>expected</b>: Callback with false called. <br>
    *                  Object destruction not blocked, this means execution time lower than requested in command. <br>
    *                  Commands after __wait shall not be executed.<br>
    * ************************************************
    */
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::string raw_commands_text = "command1\n__wait(10000)\ncommand3";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();

   m_callbackResult.reset();
   m_testSubject->execute();
   std::this_thread::sleep_for(std::chrono::seconds(3));
   m_testSubject->abort();

   execution_finish = std::chrono::high_resolution_clock::now();
   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();

   EXPECT_TRUE(m_callbackResult.has_value());
   EXPECT_FALSE(m_callbackResult.value());
   EXPECT_THAT(execution_time_ms, Lt(8000));
}

TEST_F(ButtonCommandExecutorTests, repeatCommandUsed_commandsRepeated)
{
   /**
    * <b>scenario</b>: User use the __repeat feature with 2 commands repeated 2 times <br>
    * <b>expected</b>: Commands shall be repeated (4 commands send).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, repeatCommandUsedTwice_commandsRepeated)
{
   /**
    * <b>scenario</b>: User use the __repeat feature twice <br>
    * <b>expected</b>: Commands shall be repeated.<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "__repeat_start(3)\n" +
                                               "commandToRepeat3\n"
                                               "commandToRepeat4\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat3")).WillOnce(Return(true))
                                                         .WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat4")).WillOnce(Return(true))
                                                         .WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}


TEST_F(ButtonCommandExecutorTests, oneEmptyLoop_commandsSkipped)
{
   /**
    * <b>scenario</b>: User use the __repeat feature but loop body is empty. <br>
    * <b>expected</b>: Only commands outside of the loop were sent.<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, twoEmptyLoops_commandsSkipped)
{
   /**
    * <b>scenario</b>: User use the __repeat feature twice but loop body is empty. <br>
    * <b>expected</b>: Only commands outside of the loop were sent.<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "__repeat_end()\n"
                                               "__repeat_start(2)\n" +
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, repeatStartButNoEnd_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: Only __repeat_start found in commands<br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "command2\n"
                                               "command3\n"
                                               "command4\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command3")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command4")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, invalidRepeatStartCommand_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with invalid argument entered <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start()\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, negativeRepeatCount_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with negative argument entered <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(-5)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, textRepeatCount_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with invalid argument entered (random text) <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(aabbdd2)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, mistypedStartCommand_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with invalid argument entered (random text) <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repaet_start(2)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, mistypedEndCommand_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with invalid argument entered (random text) <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repaet_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, missingStartCommand_commandsSentWithoutRepeating)
{
   /**
    * <b>scenario</b>: __repeat_start with invalid argument entered (random text) <br>
    * <b>expected</b>: Commands shall be sent as usual (without repeating).<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "commandToRepeat1\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

TEST_F(ButtonCommandExecutorTests, repeatWithWaitadded_commandsRepeated)
{
   /**
    * <b>scenario</b>: User use the __repeat feature with 3 commands repeated 2 times <br>
    * <b>expected</b>: Commands shall be repeated (4 commands send and 2 wait's executed).<br>
    * ************************************************
    */
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n" +
                                               "commandToRepeat1\n"
                                               "__wait(500)\n"
                                               "commandToRepeat2\n"
                                               "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat1")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("commandToRepeat2")).WillOnce(Return(true))
                                                         .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();
   auto callbackResult = executeAndWaitResult();
   execution_finish = std::chrono::high_resolution_clock::now();
   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
   EXPECT_THAT(execution_time_ms, Gt(700));
}

TEST_F(ButtonCommandExecutorTests, nestedRepeatWithWaitadded_commandsRepeated)
{
   /**
    * <b>scenario</b>: User enter 2 nested __repeat, one with __wait <br>
    * <b>expected</b>: Commands shall be repeated.<br>
    * ************************************************
    */
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_start;
   std::chrono::time_point<std::chrono::high_resolution_clock> execution_finish;
   uint32_t execution_time_ms = 0;

   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n"
                                                  "firstRepeat1\n"
                                                  "__repeat_start(3)\n" +
                                                     "secondRepeat1\n"
                                                     "__wait(200)\n"
                                                     "secondRepeat2\n"
                                                  "__repeat_end()\n"
                                                  "firstRepeat2\n"
                                                "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("firstRepeat1")).WillOnce(Return(true))
                                                     .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("firstRepeat2")).WillOnce(Return(true))
                                                     .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("secondRepeat1")).WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("secondRepeat2")).WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true))
                                                      .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   execution_start = std::chrono::high_resolution_clock::now();
   auto callbackResult = executeAndWaitResult();
   execution_finish = std::chrono::high_resolution_clock::now();
   execution_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(execution_finish - execution_start).count();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
   EXPECT_THAT(execution_time_ms, Gt(1000));
}

TEST_F(ButtonCommandExecutorTests, nestedRepeatWithOneInvalidCommand_onlyCorrectLoopRepeated)
{
   /**
    * <b>scenario</b>: User enter 2 nested __repeat, but inner start command in invalid <br>
    * <b>expected</b>: Inner __repeat_end shall be interpreted as close to outer __repeat_start
    *                  It doesn't make any sense, but this test case is only to verify stability to weird input.<br>
    * ************************************************
    */
   std::string raw_commands_text = std::string("command1\n") +
                                               "__repeat_start(2)\n"
                                                  "firstRepeat1\n"
                                                  "__repeat_strt(3)\n" +
                                                     "secondRepeat1\n"
                                                     "__wait(200)\n"
                                                     "secondRepeat2\n"
                                                  "__repeat_end()\n"
                                                  "firstRepeat2\n"
                                                "__repeat_end()\n"
                                               "command2\n";
   m_testSubject->parseCommands(raw_commands_text);

   EXPECT_CALL(*g_writer_mock, write("command1")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("firstRepeat1")).WillOnce(Return(true))
                                                     .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("firstRepeat2")).WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("secondRepeat1")).WillOnce(Return(true))
                                                      .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("secondRepeat2")).WillOnce(Return(true))
                                                      .WillOnce(Return(true));
   EXPECT_CALL(*g_writer_mock, write("command2")).WillOnce(Return(true));

   auto callbackResult = executeAndWaitResult();
   EXPECT_TRUE(callbackResult.has_value());
   EXPECT_TRUE(callbackResult.value());
}

}
