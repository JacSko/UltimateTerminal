#pragma once

#include <atomic>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <thread>
#include "Logger.h"
#include "ButtonCommandsParser.hpp"
/**
 * @file ButtonCommandsExecutor.hpp
 *
 * @brief
 *    Class is responsible for executing commands returned from parser in a separate thread.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

namespace MainApplication
{

/** Timeout for thread starting */
constexpr uint32_t THREAD_START_TIMEOUT = 1000;

class ButtonCommandsExecutor
{
public:
   /**
    * @brief Creates object.
    * @param[in] writer - function that is sending commands to port.
    * @param[in] callback - function called on command execution finish
    *
    * @return None.
    */
   ButtonCommandsExecutor(std::function<bool(const std::string&)> writer, std::function<void(bool)> callback):
   m_writer(writer),
   m_callback(callback),
   m_isRunning(false),
   m_threadRunning(false)
   {
   }
   ~ButtonCommandsExecutor()
   {
   }
   bool isExecution()
   {
      return m_isRunning;
   }
   /**
    * @brief Starts execution of  currently stored commands.
    *
    * @return None.
    */
   void execute()
   {
      if (!m_isRunning)
      {
         UT_Log(USER_BTN_HANDLER, LOW, "starting execution");
         m_isRunning = true;
         std::thread(&ButtonCommandsExecutor::threadLoop, this).detach();
      }
   }

   void abort()
   {
      UT_Log_If(!m_isRunning, USER_BTN_HANDLER, LOW, "cannot abort - not started");
      if (m_isRunning && m_threadRunning)
      {
         UT_Log(USER_BTN_HANDLER, LOW, "aborting ongoing execution!");
         m_isRunning = false;
         auto start = std::chrono::steady_clock::now();
         while (std::chrono::duration_cast<std::chrono::milliseconds>
               (std::chrono::steady_clock::now() - start) < std::chrono::milliseconds(500) && m_threadRunning)
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            UT_Log(USER_BTN_HANDLER, LOW, "waiting for thread end!");
         }
      }
   }

   /**
    * @brief Process new text input to commands.
    *
    * @param[in] text - commands in textual form.
    * @return Number of created commands.
    */
   int parseCommands(const std::string& text)
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "parsing new commands");

      int result = 0;
      if (!m_isRunning)
      {
         result = m_parser.parseCommands(text, m_writer);
         m_raw_text = text;
         UT_Log(USER_BTN_HANDLER, LOW, "created %u commands", result);
      }
      return result;
   }
private:

  void threadLoop()
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "starting thread");
      m_threadRunning = true;
      m_parser.reset();
      while ((m_parser.commandsExecuted() != m_parser.commandsCount()) && isExecution())
      {
            if (!m_parser.runNext())
            {
               UT_Log(USER_BTN_HANDLER, ERROR, "Cannot execute command, aborting!");
               break;
            }
      }

      if (m_callback)
      {
         UT_Log(USER_BTN_HANDLER, LOW, "Command execution result %u", m_parser.commandsExecuted() != m_parser.commandsCount());
         m_callback(m_parser.commandsExecuted() == m_parser.commandsCount());
      }
      m_threadRunning = false;
      m_isRunning = false;
   }

   std::string m_raw_text;
   std::function<bool(const std::string&)> m_writer;
   ButtonCommandsParser m_parser;
   std::function<void(bool)> m_callback;
   std::atomic<bool> m_isRunning;
   bool m_threadRunning;
};

}
