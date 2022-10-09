#ifndef TIMERSIMPL_H_
#define TIMERSIMPL_H_

#include <vector>
#include <chrono>

#include "ITimers.h"
#include "ThreadWorker.h"

namespace Utilities
{

class TimersImpl : public ITimers
{
public:
   TimersImpl();
   ~TimersImpl();
private:

   struct Timer
   {
      uint32_t id;
      uint32_t default_timeout;
      uint32_t current_timeout;
      bool is_running;
      bool periodic;
      std::chrono::time_point<std::chrono::steady_clock> start_ts;
      ITimerClient* callback;
   };

   bool start() override;
   void stop() override;
   uint32_t createTimer(ITimerClient* client, uint32_t default_timout) override;
   void removeTimer(uint32_t timer_id) override;
   bool setTimeout(uint32_t timer_id, uint32_t timeout) override;
   uint32_t getTimeout(uint32_t timer_id) override;
   void startTimer(uint32_t timer_id) override;
   void startTimer(uint32_t timer_id, uint32_t timeout) override;
   void startTimer(uint32_t timer_id, bool periodic) override;
   void stopTimer(uint32_t timer_id) override;

   void execute();
   bool isTimeoutValid(uint32_t timeout);

   ThreadWorker m_thread;
   std::vector<Timer> m_timers;
   uint32_t m_last_timer_id;
   std::recursive_mutex m_timers_mutex;
};


}
#endif
