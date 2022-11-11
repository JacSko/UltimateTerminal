/* =============================
 *          Defines
 * =============================*/
/* =============================
 *   Includes of common headers
 * =============================*/
/* =============================
 *  Includes of project headers
 * =============================*/
#include "ThreadWorker.h"
#include "Logger.h"
/* =============================
 *      Global variables
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/

namespace Utilities
{

ThreadWorker::ThreadWorker(std::function<void()> worker, const std::string& thread_name):
m_worker(std::move(worker)),
m_state(ThreadState::STOPPED),
m_thread_name(thread_name)
{

}

ThreadWorker::~ThreadWorker()
{
   stop();
}

bool ThreadWorker::start(uint32_t timeout)
{
   bool result = true;
   std::unique_lock<std::mutex> guard(m_lock);
   m_state = ThreadState::STOPPED;
   m_thread = std::thread(&ThreadWorker::execute, this);

   if (timeout > 0)
   {
      result = m_cv.wait_for(guard, std::chrono::milliseconds(timeout), [&]{return (m_state == ThreadState::RUNNING);});

   }
   else
   {
      m_cv.wait(guard, [&]{return (m_state == ThreadState::RUNNING);});
   }
   return result;

}

void ThreadWorker::stop()
{
   bool join = false;
   {
      std::lock_guard<std::mutex> guard(m_lock);
      join = (m_state != ThreadState::STOPPING);
      m_state = ThreadState::STOPPING;
   }
   if (join)
   {
      if(m_thread.joinable())
      {
         m_thread.join();
      }
   }
}

bool ThreadWorker::setPriority(int prio_level)
{
   int status = 0;
   std::lock_guard<std::mutex> guard(m_lock);
   int policy;
   struct sched_param params;
   status = pthread_getschedparam(m_thread.native_handle(), &policy, &params);
   if (status == 0)
   {
      if(SCHED_RR != policy)
      {
         policy = SCHED_RR;
      };
      params.sched_priority = prio_level;
      status = pthread_setschedparam(m_thread.native_handle(), policy, &params);
   }
   return status == 0;
}
bool ThreadWorker::isRunning()
{
   std::lock_guard<std::mutex> guard(m_lock);
   return m_state == ThreadState::RUNNING;
}

std::string ThreadWorker::getThreadName()
{
   return m_thread_name;
}

void ThreadWorker::execute()
{
   {
      std::lock_guard<std::mutex> guard(m_lock);
      m_state = ThreadState::RUNNING;
      m_cv.notify_all();
   }
   m_worker();
   {
      std::lock_guard<std::mutex> guard(m_lock);
      m_state = ThreadState::STOPPED;
   }
}

ThreadState ThreadWorker::getState()
{
   std::lock_guard<std::mutex> guard(m_lock);
   return m_state;
}
}
