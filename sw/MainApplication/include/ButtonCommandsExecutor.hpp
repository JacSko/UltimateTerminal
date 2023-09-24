#pragma once

#include <atomic>
#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <thread>
#include "Logger.h"

/**
 * @file ButtonCommandsExecutor.hpp
 *
 * @brief
 *    Class is responsible for parsing user input in textual form, converting to commands that are next send using passed writer.
 *
 * @details
 *    Text input is divided into commands by splitting by '\n' char.
 *    There is also processing of special commands, that begins with '__' prefix (example: __wait(<time>)).
 *    Every __wait command longer than MAXIMUM_WAIT_TIME ms is divided into MAXIMUM_WAIT_TIME ms chunks.
 *    This allows to not block application shutdown when such wait is executed.
 *    Every instance if executor creates a separate thread for commands execution.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

namespace MainApplication
{

/** Timeout for thread starting */
constexpr uint32_t THREAD_START_TIMEOUT = 1000;
/** Maximum waiting time - see details for more description */
constexpr uint32_t MAXIMUM_WAIT_TIME = 500;

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
         m_commands.clear();
         m_raw_text = text;
         std::stringstream ss(m_raw_text);
         std::string command;
         while(std::getline(ss, command, '\n'))
         {
            UT_Log(USER_BTN_HANDLER, HIGH, "command [%s]", command.c_str());
            if(isSpecialCommand(command))
            {
               addSpecialCommand(command);
            }
            else
            {
               m_commands.push_back([this, command]()->bool { return m_writer(command);});
            }
         }
         result = m_commands.size();
         UT_Log(USER_BTN_HANDLER, LOW, "created %u commands", m_commands.size());
      }
      return result;
   }
private:

   bool isSpecialCommand(const std::string& command)
   {
      return command.find("__", 0) == 0;
   }
   void addSpecialCommand(const std::string& command)
   {
      std::smatch matches;
      std::regex_match(command.cbegin(), command.cend(), matches, std::regex("(__.*)\\((.*)\\)"));
      if (matches.size() > 1)
      {
         if (matches[1] == "__wait")
         {
            prepareWaitCommand(std::stoi(matches[2]));
         }
      }
   }
   void prepareWaitCommand(uint32_t time)
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "preparing wait command with %ums timeout", time);
      while (time)
      {
         uint32_t timeout = MAXIMUM_WAIT_TIME;
         if (time < MAXIMUM_WAIT_TIME)
         {
            timeout = time % MAXIMUM_WAIT_TIME;
         }
         time -= timeout;
         m_commands.push_back([timeout]()->bool {std::this_thread::sleep_for(std::chrono::milliseconds(timeout)); return true;});
      }
   }
   void threadLoop()
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "starting thread");
      m_threadRunning = true;
      std::vector<std::function<bool()>>::iterator it = m_commands.begin();
      while (it != m_commands.end() && isExecution())
      {
         if (*it)
         {
            if (!(*it)())
            {
               UT_Log(USER_BTN_HANDLER, ERROR, "Cannot execute command, aborting!");
               break;
            }
         }
         it++;
      }

      if (m_callback)
      {
         UT_Log(USER_BTN_HANDLER, LOW, "Command execution result %u", it == m_commands.end());
         m_callback(it == m_commands.end());
      }
      m_threadRunning = false;
      m_isRunning = false;
   }

   std::string m_raw_text;
   std::function<bool(const std::string&)> m_writer;
   std::vector<std::function<bool()>> m_commands;
   std::function<void(bool)> m_callback;
   std::atomic<bool> m_isRunning;
   bool m_threadRunning;
};

}
