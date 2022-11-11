#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>
#include "Logger.h"
#include "TimersImpl.h"

using namespace testing;

namespace Utilities
{

constexpr uint32_t TEST_TIMER_DEFAULT_TIMEOUT = 50;

struct ITimerClientMock : public ITimerClient
{
   MOCK_METHOD1(onTimeout, void(uint32_t timer_id));
};

struct TimersFixture : public testing::Test
{
   void SetUp()
   {
      m_test_subject = ITimersFactory::create();
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
   }
   std::unique_ptr<ITimers> m_test_subject;
   ITimerClientMock m_client_mock;
};


void WAIT_FOR_VARIABLE(std::atomic<bool>& wait, uint32_t timeout)
{
   auto start_wait = std::chrono::steady_clock::now();
   while(wait && ((std::chrono::steady_clock::now() - start_wait) < std::chrono::milliseconds(timeout)))
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }
}

TEST_F(TimersFixture, create_timer_and_wait_for_callback)
{
   /* atomic flag to wait for callback */
   std::atomic<bool> wait(true);
   constexpr uint32_t TEST_MAX_WAIT_TIMEOUT = 100;
   /**
    * <b>scenario</b>: Create timer, start it, then wait for callback.<br>
    * <b>expected</b>: Callback should be called.<br>
    * ************************************************
    */
   m_test_subject->start();

   /* ID should be different than TIMERS_INVALID_ID */
   uint32_t test_timer_id = m_test_subject->createTimer(&m_client_mock, TEST_TIMER_DEFAULT_TIMEOUT);
   ASSERT_TRUE(test_timer_id != TIMERS_INVALID_ID);

   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Invoke([&](uint32_t){wait = false;}));
   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id);

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);

   m_test_subject->stop();

}

TEST_F(TimersFixture, change_timeout_when_timer_is_running)
{
   /* atomic flag to wait for callback */
   std::atomic<bool> wait(true);
   constexpr uint32_t TEST_MAX_WAIT_TIMEOUT = 300;
   /**
    * <b>scenario</b>: Create timer, start it, wait for first callback. Then change timeout.<br>
    * <b>expected</b>: Callback should be called after new timeout.<br>
    * ************************************************
    */
   m_test_subject->start();

   /* ID should be different than TIMERS_INVALID_ID */
   uint32_t test_timer_id = m_test_subject->createTimer(&m_client_mock, TEST_TIMER_DEFAULT_TIMEOUT);
   ASSERT_TRUE(test_timer_id != TIMERS_INVALID_ID);

   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Invoke([&](uint32_t){wait = false;}));
   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id);

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);

   wait = true;
   const uint32_t new_timer_timeout = 2* TEST_TIMER_DEFAULT_TIMEOUT;
   EXPECT_TRUE(m_test_subject->setTimeout(test_timer_id, new_timer_timeout));
   auto new_timeout_timestamp = std::chrono::steady_clock::now();
   auto callback_timestamp = std::chrono::steady_clock::now();
   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Invoke([&](uint32_t){wait = false; callback_timestamp = std::chrono::steady_clock::now();}));

   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id);

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);

   m_test_subject->stop();

   EXPECT_THAT((callback_timestamp - new_timeout_timestamp), AllOf(Ge(std::chrono::milliseconds(new_timer_timeout - 20)), Le(std::chrono::milliseconds(new_timer_timeout + 20))));

}

TEST_F(TimersFixture, start_timer_with_temporary_timeout)
{
   /* atomic flag to wait for callback */
   std::atomic<bool> wait(true);
   constexpr uint32_t TEST_MAX_WAIT_TIMEOUT = 300;
   uint32_t new_timer_timeout = 2 * TEST_TIMER_DEFAULT_TIMEOUT;
   /**
    * <b>scenario</b>: Create timer with default timeout, but override this timeout when starting timer.<br>
    * <b>expected</b>: Callback should be called with new timeout, but afterwards the default timeout should be restored.<br>
    * ************************************************
    */
   m_test_subject->start();

   /* ID should be different than TIMERS_INVALID_ID */
   uint32_t test_timer_id = m_test_subject->createTimer(&m_client_mock, TEST_TIMER_DEFAULT_TIMEOUT);
   ASSERT_TRUE(test_timer_id != TIMERS_INVALID_ID);
   auto callback_timestamp = std::chrono::steady_clock::now();

   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Invoke([&](uint32_t){wait = false; callback_timestamp = std::chrono::steady_clock::now();}));
   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id, new_timer_timeout);
   auto start_timestamp = std::chrono::steady_clock::now();

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);

   EXPECT_THAT((callback_timestamp - start_timestamp), AllOf(Ge(std::chrono::milliseconds(new_timer_timeout - 20)), Le(std::chrono::milliseconds(new_timer_timeout + 20))));

   wait = true;
   callback_timestamp = std::chrono::steady_clock::now();
   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Invoke([&](uint32_t){wait = false; callback_timestamp = std::chrono::steady_clock::now();}));

   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id);
   start_timestamp = std::chrono::steady_clock::now();

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);
   m_test_subject->stop();
   EXPECT_THAT((callback_timestamp - start_timestamp), AllOf(Ge(std::chrono::milliseconds(TEST_TIMER_DEFAULT_TIMEOUT - 20)), Le(std::chrono::milliseconds(TEST_TIMER_DEFAULT_TIMEOUT + 20))));

}

TEST_F(TimersFixture, start_periodic_timer)
{
   /* atomic flag to wait for callback */
   std::atomic<bool> wait(true);
   constexpr uint32_t TEST_MAX_WAIT_TIMEOUT = 500;
   /**
    * <b>scenario</b>: Create timer and start as periodic.<br>
    * <b>expected</b>: Callback should be called in the loop, without starting.<br>
    * ************************************************
    */
   m_test_subject->start();

   /* ID should be different than TIMERS_INVALID_ID */
   uint32_t test_timer_id = m_test_subject->createTimer(&m_client_mock, TEST_TIMER_DEFAULT_TIMEOUT);
   ASSERT_TRUE(test_timer_id != TIMERS_INVALID_ID);

   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).WillOnce(Return())
      .WillOnce(Return())
      .WillOnce(Return())
      .WillOnce(Return())
      .WillOnce([&](uint32_t){m_test_subject->stopTimer(test_timer_id); wait = false;});

   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id, true);

   WAIT_FOR_VARIABLE(wait, TEST_MAX_WAIT_TIMEOUT);

   m_test_subject->stop();
}

TEST_F(TimersFixture, timer_stopping)
{
   /* atomic flag to wait for callback */
   std::atomic<bool> wait(true);
   /**
    * <b>scenario</b>: Create timer and start as periodic, than stop.<br>
    * <b>expected</b>: No callback called.<br>
    * ************************************************
    */
   m_test_subject->start();

   /* ID should be different than TIMERS_INVALID_ID */
   uint32_t test_timer_id = m_test_subject->createTimer(&m_client_mock, TEST_TIMER_DEFAULT_TIMEOUT);
   ASSERT_TRUE(test_timer_id != TIMERS_INVALID_ID);

   EXPECT_CALL(m_client_mock, onTimeout(test_timer_id)).Times(0);

   /* start timer for one shot */
   m_test_subject->startTimer(test_timer_id, true);
   m_test_subject->stopTimer(test_timer_id);
   m_test_subject->stop();
}


}
