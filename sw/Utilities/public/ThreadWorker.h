#ifndef THREAD_WORKER_H_
#define THREAD_WORKER_H_

/* ============================= */
/**
 * @file ThreadWorker.h
 *
 * @brief Class that helps to implement working thread in derived classes.
 *
 * @author Jacek Skowronek
 * @date 17/03/2022
 */
/* ============================= */

/* =============================
 *          Defines
 * =============================*/
/* =============================
 *   Includes of common headers
 * =============================*/
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <string>
#include <functional>
/* =============================
 *  Includes of project headers
 * =============================*/

/* =============================
 *      Global variables
 * =============================*/

/* =============================
 *       Data structures
 * =============================*/
namespace Utilities
{

enum class ThreadState
{
   INIT,       /* Worker thread not started yet */
   STARTING,   /* Worker thread is starting up */
   RUNNING,    /* Worker thread is running */
   STOPPING,   /* Worker thread is stopping */
   STOPPED,    /* Worker thread is stopped */
   ERROR,      /* Error occurs */
};

class ThreadWorker
{
public:
   ThreadWorker(std::function<void()> worker, const std::string& thread_name);
   ~ThreadWorker();

   /**
    * @brief Starts the worker thread - block until thread started.
    * @param[in] timeout - timeout in milliseconds for thread start
    * @return True if started correctly, otherwise false.
    */
   bool start(uint32_t timeout);
   /**
    * @brief Stops the worker thread - block until thread stopped.
    * @return void
    */
   void stop();
   /**
    * @brief Sets thread priority.
    * @param[in] prio_level - thread priority
    * @return void
    */
   bool setPriority(int prio_level);
   /**
    * @brief Informs about current worker thread state.
    * @return Corresponding ThreadState
    */
   ThreadState getState();
   /**
    * @brief Informs about current worker thread state.
    * @return True is thread is running.
    */
   bool isRunning();
   /**
    * @brief Returns current thread name.
    * @return Thread name.
    */
   std::string getThreadName();

private:
   void execute();


   std::function<void()> m_worker;
   std::mutex m_lock;
   std::thread m_thread;
   std::condition_variable m_cv;
   ThreadState m_state;
   const std::string m_thread_name;

};


}





#endif
