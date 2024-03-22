#pragma once

/**
 * @file IProcessDriver.h
 *
 * @brief
 *    Interface that starts child process and provides process output asynchronously.
 *    Uses linux system pipe's mechanism to gather data from process.
 * @details
 *
 * @author Jacek Skowronek
 * @date   13.03.2024
 *
 */
#include <vector>
#include <stdint.h>
#include <memory>

namespace Drivers
{
namespace Process
{

enum class Event
{
   PROCESS_STARTED,    /**< Child process started successfully */
   PROCESS_STOPPED,    /**< Child process stopped */
   DATA_RECEIVED       /**< New data received from process */
};

class ProcessListener
{
public:
   virtual ~ProcessListener(){};
   /**
    * @brief Callback called on new event.
    * @param[in] ev - Event received from process
    * @param[in] data - Reference to vector with new data, can be empty, depends on event
    * @param[in] size - Size of the data, can be 0, depends on event.
    * @return None.
    */
   virtual void onProcessEvent(Event ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class IProcessDriver
{
public:
   static std::unique_ptr<IProcessDriver> create();
   /**
    * @brief Starts a child process.
    * @param[in] processName - name of the process to be started (shell-type name, like 'adb logcat')
    * @return true if started successfully, otherwise false.
    */
   virtual bool start(const std::string& processName) = 0;
   /**
    * @brief Disconnects client from server.
    * @return void.
    */
   virtual void stop() = 0;
   /**
    * @brief Returns information if process is started.
    * @return True if opened.
    */
   virtual bool isStarted() = 0;
   /**
    * @brief Adds listener to receive notifications about process status or data received.
    * @return None.
    */
   virtual void addListener(ProcessListener* callback) = 0;
   /**
    * @brief Removes listener.
    * @return None.
    */
   virtual void removeListener(ProcessListener* callback) = 0;
   virtual ~IProcessDriver(){};
};

}
}
