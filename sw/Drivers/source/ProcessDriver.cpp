#include <vector>

#include <sys/epoll.h>
#include <unistd.h>
#include "ProcessDriver.h"
#include "Logger.h"

namespace Drivers
{
namespace Process
{

const std::chrono::milliseconds COND_VAR_WAIT_MS = std::chrono::milliseconds(500);
constexpr char DATA_DELIMITER = '\n';
const uint32_t THREAD_START_TIMEOUT = 1000;

std::unique_ptr<IProcessDriver> IProcessDriver::create()
{
   return std::unique_ptr<IProcessDriver>(new ProcessDriver());
}

ProcessDriver::ProcessDriver():
m_worker(std::bind(&ProcessDriver::receivingThread, this), "PROCESS_WORKER"),
m_state(State::STOPPED)
{
   m_worker.start(THREAD_START_TIMEOUT);
}
ProcessDriver::~ProcessDriver()
{
   UT_Log(PROCESS_DRV, HIGH, "%s", __func__);
   if (isStarted())
   {
      stop();
   }
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::EXITING;
      m_conditionVariable.notify_all();
   }
   if (m_worker.isRunning())
   {
      m_worker.stop();
   }
}
bool ProcessDriver::start(const std::string& processName)
{
   bool result = false;
   UT_Log(PROCESS_DRV, LOW, "%s command [%s]", __func__, processName.c_str());
   if (!isStarted())
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::STARTING;
      m_processName = processName;
      m_conditionVariable.notify_all();
      if (m_conditionVariable.wait_for(lock, COND_VAR_WAIT_MS, [&](){return m_state != State::STARTING;}))
      {
         UT_Log_If(m_state != State::RUNNING, PROCESS_DRV, ERROR, "[%s] thread start timeout! status %d",
                                                                                        processName.c_str(),
                                                                                        static_cast<uint8_t>(m_state.load()));
         result = m_state == State::RUNNING;
      }
   }
   return result;
}
void ProcessDriver::stop()
{
   UT_Log(PROCESS_DRV, LOW, "[%s] %s", m_processName.c_str(), __func__);
   if (isStarted())
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::STOPPING;
      m_conditionVariable.notify_all();
      m_conditionVariable.wait_for(lock, COND_VAR_WAIT_MS, [&](){return m_state == State::STOPPED;});
      UT_Log(PROCESS_DRV, HIGH, "[%s] %s finished, thread state %d", m_processName.c_str(), __func__, (uint8_t)m_state.load());
   }
}
bool ProcessDriver::isStarted()
{
   return m_state == State::STARTING || m_state == State::RUNNING;
}
void ProcessDriver::addListener(ProcessListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void ProcessDriver::removeListener(ProcessListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}
void ProcessDriver::receivingThread()
{
   UT_Log(PROCESS_DRV, LOW, "starting receiving thread");
   while(m_worker.isRunning())
   {
      constexpr int PIPE_READ = 0;
      constexpr int PIPE_WRITE = 1;
      Utilities::ApplicationExecutor executor;
      int pipesFd [2] = {-1, -1};

      {
         std::unique_lock<std::mutex> lock(m_mutex);
         UT_Log(PROCESS_DRV, LOW, "[%s] waiting for new process request", m_processName.c_str());
         m_conditionVariable.wait(lock, [&](){return m_state != State::STOPPED;});
         UT_Log(PROCESS_DRV, LOW, "[%s] new process start requested", m_processName.c_str());
         if (m_state == State::STARTING)
         {
            UT_Log(PROCESS_DRV, HIGH, "[%s] creating pipes to process", m_processName.c_str());
            int pipesReady = pipe(pipesFd);
            UT_Log_If(pipesReady == -1, PROCESS_DRV, ERROR, "Cannot create pipe! errno %d(%s)", errno, strerror(errno));
            executor.setOutputFd(pipesFd[PIPE_WRITE]);
            bool appStarted = executor.startApplication(m_processName.c_str());
            UT_Log_If(!appStarted, PROCESS_DRV, ERROR, "[%s] cannot start application", m_processName.c_str());
            if (appStarted)
            {
               UT_Log(PROCESS_DRV, LOW, "[%s] process started", m_processName.c_str());
               m_state = State::RUNNING;
               m_conditionVariable.notify_all();
            }
         }
         else if (m_state == State::EXITING)
         {
            UT_Log(PROCESS_DRV, LOW, "[%s] stopping main thread! probably close request received?", m_processName.c_str());
            break;
         }
      }
      if (m_state == State::RUNNING)
      {
         startProcessOutputReading(pipesFd[PIPE_READ], executor);
      }
      m_state = State::STOPPED;
      UT_Log(PROCESS_DRV, LOW, "[%s] stopping executor", m_processName.c_str());
      executor.stopApplication();
      if (pipesFd[PIPE_READ] != -1)
      {
         close(pipesFd[PIPE_WRITE]);
         close(pipesFd[PIPE_READ]);
         pipesFd[PIPE_READ] = -1;
         pipesFd[PIPE_WRITE] = -1;
      }
      m_conditionVariable.notify_all();
   }
   UT_Log(PROCESS_DRV, LOW, "closing receiving thread");

}
void ProcessDriver::startProcessOutputReading(int outputFd, Utilities::ApplicationExecutor& executor)
{
   const uint32_t MAX_LINE_LENGTH = 4096;
   std::vector<uint8_t> buffer (MAX_LINE_LENGTH);
   uint32_t bufferIndex = 0;
   notifyListeners(Event::PROCESS_STARTED, {}, 0);
   UT_Log(PROCESS_DRV, MEDIUM, "[%s] start outpt reading on fd %d", m_processName.c_str(), outputFd);

   int epollFd = epoll_create(1);
   UT_Log_If(epollFd == -1, PROCESS_DRV, ERROR, "Cannot create epoll! %d(%s)", errno, strerror(errno));

   struct epoll_event event = {};
   event.events = EPOLLIN;
   event.data.fd = outputFd;
   int status = epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event);
   UT_Log_If(status != 0, PROCESS_DRV, ERROR, "Epoll error! %d(%s)", errno, strerror(errno));

   while(m_state == State::RUNNING)
   {
      if (bufferIndex == buffer.size())
      {
         bufferIndex = 0;
      }

      epoll_event epoll_event = {};
      auto ready_cnt = epoll_wait(epollFd, &epoll_event, 1, 500);
      UT_Log_If(ready_cnt == 0, PROCESS_DRV, LOW, "no data received in last 500ms");
      if (ready_cnt > 0)
      {
         int bytesReceived = read(outputFd, buffer.data() + bufferIndex,
                                            buffer.size() - bufferIndex);
         if (bytesReceived > 0)
         {
            bufferIndex += bytesReceived;
            while(1)
            {
               auto it = std::find(buffer.begin(),
                                  (buffer.begin() + bufferIndex),
                                  (uint8_t)DATA_DELIMITER);
               if (it != (buffer.begin() + bufferIndex))
               {
                  it++; //include the found newline too
                  notifyListeners(Event::DATA_RECEIVED,
                                  std::vector<uint8_t>(buffer.begin(), it),
                                  std::distance(buffer.begin(), it));
                  std::copy(it, buffer.begin() + bufferIndex, buffer.begin());
                  bufferIndex = std::distance(it, buffer.begin() + bufferIndex);
               }
               else
               {
                  break;
               }
            }
         }
         else if(bytesReceived == 0)
         {
            UT_Log(PROCESS_DRV, ERROR, "[%s] no bytes received", m_processName.c_str());
            notifyListeners(Event::PROCESS_STOPPED, {}, 0);
         }
         else
         {
            UT_Log(PROCESS_DRV, ERROR, "[%s] read returned %d, aborting", m_processName.c_str(), bytesReceived);
            notifyListeners(Event::PROCESS_STOPPED, {}, 0);
            break;
         }
      }
      else
      {
         UT_Log(PROCESS_DRV, LOW, "[%s] no data ready, checking if process still alive", m_processName.c_str());
         bool alive = executor.isRunning();
         if (!alive)
         {
            UT_Log(PROCESS_DRV, ERROR, "[%s] Process not alive", m_processName.c_str());
            notifyListeners(Event::PROCESS_STOPPED, {}, 0);
            break;
         }
      }
   }
   UT_Log(PROCESS_DRV, LOW, "[%s] closing read loop", m_processName.c_str());
}
void ProcessDriver::notifyListeners(Event ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   for (auto& listener : m_listeners)
   {
      if (listener)
      {
         listener->onProcessEvent(ev, data, size);
      }
   }
}

}
}
