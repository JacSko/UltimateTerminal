#include <algorithm>
#include "TimersImpl.h"
#include "Logger.h"

namespace Utilities
{

std::unique_ptr<ITimers> ITimersFactory::create()
{
   return std::unique_ptr<ITimers>(new TimersImpl());
}

TimersImpl::TimersImpl():
m_thread(std::bind(&TimersImpl::execute, this), "TIMERS"),
m_timers {},
m_last_timer_id(0)
{

}
TimersImpl::~TimersImpl()
{
   stop();
}
bool TimersImpl::start()
{
   HC_Log(UTILITIES, INFO, "starting timers");
   constexpr uint32_t THREAD_START_TIMEOUT = 1000;
   return m_thread.start(THREAD_START_TIMEOUT);
}
void TimersImpl::stop()
{
   HC_Log(UTILITIES, INFO, "stopping timers");
   m_thread.stop();
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   m_timers.clear();
}

uint32_t TimersImpl::createTimer(ITimerClient* client, uint32_t default_timeout)
{
   uint32_t result = TIMERS_INVALID_ID;
   if (isTimeoutValid(default_timeout) && client)
   {
      std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
      m_last_timer_id++;
      if (m_last_timer_id == TIMERS_INVALID_ID)
      {
         /* skip TIMERS_INVALID_ID */
         m_last_timer_id++;
      }
      m_timers.push_back((Timer){m_last_timer_id, default_timeout, default_timeout, false, false, {}, client});
      result = m_last_timer_id;
   }
   HC_Log(UTILITIES, LOW, "creating timer %u with default timeout %u - %s", result, default_timeout, result == TIMERS_INVALID_ID? "NOK" : "OK");
   return result;
}
void TimersImpl::removeTimer(uint32_t timer_id)
{
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      m_timers.erase(it);
   }
   HC_Log(UTILITIES, LOW, "destroying timer %u", timer_id);
}
bool TimersImpl::setTimeout(uint32_t timer_id, uint32_t timeout)
{
   bool result = false;
   if (isTimeoutValid(timeout))
   {
      std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
      auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
      if (it != m_timers.end())
      {
         it->start_ts = std::chrono::steady_clock::now();
         it->default_timeout = timeout;
         it->current_timeout = timeout;
         result = true;
      }
   }
   HC_Log(UTILITIES, LOW, "setting timeout on timer %u to %u - %s", timer_id, timeout, result? "OK" : "NOK");
   return result;
}

bool TimersImpl::isTimeoutValid(uint32_t timeout)
{
   return (timeout > 0) && ((timeout%10) == 0);
}
uint32_t TimersImpl::getTimeout(uint32_t timer_id)
{
   uint32_t result = 0;
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      result = it->current_timeout;
   }
   return result;
}
void TimersImpl::startTimer(uint32_t timer_id)
{
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      it->is_running = true;
      it->start_ts = std::chrono::steady_clock::now();
   }
   HC_Log(UTILITIES, HIGH, "%s id %u", __func__, timer_id);
}
void TimersImpl::startTimer(uint32_t timer_id, uint32_t timeout)
{
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      it->is_running = true;
      it->current_timeout = timeout;
      it->start_ts = std::chrono::steady_clock::now();
   }
   HC_Log(UTILITIES, HIGH, "%s id %u timeout %u", __func__, timer_id, timeout);
}
void TimersImpl::startTimer(uint32_t timer_id, bool periodic)
{
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      it->is_running = true;
      it->periodic = periodic;
      it->start_ts = std::chrono::steady_clock::now();
   }
   HC_Log(UTILITIES, HIGH, "%s id %u periodic %u", __func__, timer_id, (uint8_t)periodic);
}
void TimersImpl::stopTimer(uint32_t timer_id)
{
   std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
   auto it = std::find_if(m_timers.begin(), m_timers.end(), [&](const Timer& timer){return timer.id == timer_id;});
   if (it != m_timers.end())
   {
      it->is_running = false;
   }
   HC_Log(UTILITIES, HIGH, "%s %u", __func__, timer_id);
}

void TimersImpl::execute()
{
   constexpr uint32_t THREAD_SLEEP_TIMEOUT = 1;
   while(m_thread.isRunning())
   {
      {
         std::lock_guard<std::recursive_mutex> lock(m_timers_mutex);
         for (auto& timer : m_timers)
         {
            /* check if this timer is started */
            if (timer.is_running)
            {
               if ((std::chrono::steady_clock::now() - timer.start_ts) >= std::chrono::milliseconds(timer.current_timeout))
               {
                  timer.current_timeout = timer.default_timeout;
                  timer.start_ts = std::chrono::steady_clock::now();
                  if (!timer.periodic)
                  {
                     timer.is_running = false;
                  }
                  timer.callback->onTimeout(timer.id);
               }
            }
         }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_TIMEOUT));
   }
}

}
