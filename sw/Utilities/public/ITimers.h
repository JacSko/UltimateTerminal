#ifndef ITIMERS_H_
#define ITIMERS_H_

/* ============================= */
/**
 * @file Timers.h
 *
 * @brief Provides simple timer mechanism.
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
#include <stdint.h>
#include <memory>
/* =============================
 *  Includes of project headers
 * =============================*/

/* =============================
 *      Global variables
 * =============================*/

/* =============================
 *       Data structures
 * =============================*/

#define TIMERS_INVALID_ID 0xFF

namespace Utilities
{

class ITimerClient
{
public:
   /**
    * @brief Interface method, called when the registered timer expires.
    * @param[in] timer_id - ID of the timer assigned via createTimer() method
    * @return void
    */
   virtual void onTimeout(uint32_t timer_id) = 0;

   virtual ~ITimerClient() {};
};

class ITimers
{
public:
   /**
    * @brief Initialize the timers module.
    * @return True if initialized successfully
    */
   virtual bool start() = 0;
   /**
    * @brief Stops the timers module.
    * @return void
    */
   virtual void stop() = 0;
   /**
    * @brief Creates the new timer.
    * @param[in] client - timer client for call on timer timeout
    * @param[in] default_timeout - the default timeout in [ms]
    * @return ID of the timer
    */
   virtual uint32_t createTimer(ITimerClient* client, uint32_t default_timout) = 0;
   /**
    * @brief Destroys the existing timer by ID.
    * @param[in] timer_id - ID of the timer
    * @return void
    */
   virtual void removeTimer(uint32_t timer_id) = 0;
   /**
    * @brief Sets the new timout for defined timer. If timer is already running, it is restarted with new timeout.
    * @param[in] timer_id - ID of the timer
    * @param[in] timeout - time period in [ms]
    * @return True is set successfully
    */
   virtual bool setTimeout(uint32_t timer_id, uint32_t timeout) = 0;
   /**
    * @brief Returns the current timer timeout.
    * @param[in] timer_id - ID of the timer
    * @return Timeout in [ms]
    */
   virtual uint32_t getTimeout(uint32_t timer_id) = 0;
   /**
    * @brief Starts the timer. Timer will be started only once and stopped when expired.
    * @param[in] timer_id - ID of the timer
    * @return void
    */
   virtual void startTimer(uint32_t timer_id) = 0;
   /**
    * @brief Starts the timer. Timer will be started only once and stopped when expired.
    *        Timeout will be used ony once, after that default one will be restored
    * @param[in] timer_id - ID of the timer
    * @param[in] timeout - timeout in [ms].
    * @return void
    */
   virtual void startTimer(uint32_t timer_id, uint32_t timeout) = 0;
   /**
    * @brief Starts the periodic timer.
    * @param[in] timer_id - ID of the timer
    * @param[in] periodic - if true, timer will be automatically restarted.
    * @return void
    */
   virtual void startTimer(uint32_t timer_id, bool periodic) = 0;
   /**
    * @brief Stops running timer.
    * @param[in] timer_id - ID of the timer
    * @return void
    */
   virtual void stopTimer(uint32_t timer_id) = 0;

   virtual ~ITimers(){};
};

struct ITimersFactory
{
   static std::unique_ptr<ITimers> create();
};


}

#endif
