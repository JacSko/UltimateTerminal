#pragma once

#include <mutex>
#include <atomic>

#include "IProcessDriver.h"
#include "ThreadWorker.h"
#include "ApplicationExecutor.hpp"

namespace Drivers
{
namespace Process
{


class ProcessDriver : public IProcessDriver
{
public:
   ProcessDriver();
   ~ProcessDriver();
private:
   enum class State
   {
      STOPPED,
      STARTING,
      RUNNING,
      STOPPING,
      EXITING
   };

   bool start(const std::string& ProcessName) override;
   void stop() override;
   bool isStarted() override;
   void addListener(ProcessListener* callback) override;
   void removeListener(ProcessListener* callback) override;
   void receivingThread();
   void startProcessOutputReading(int outputFd, Utilities::ApplicationExecutor& executor);
   void notifyListeners(Event ev, const std::vector<uint8_t>& data, size_t size);
   Utilities::ThreadWorker m_worker;
   std::vector<ProcessListener*> m_listeners;
   std::mutex m_listeners_mutex;
   std::atomic<State> m_state;
   std::mutex m_mutex;
   std::condition_variable m_conditionVariable;
   std::string m_processName;
};


}
}
