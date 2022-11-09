#pragma once

#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <thread>
#include "Logger.h"
#include "ThreadWorker.h"

constexpr uint32_t THREAD_START_TIMEOUT = 1000;
constexpr uint32_t MAXIMUM_WAIT_TIME = 500;

namespace GUI
{

class ButtonCommandsExecutor : public Utilities::ThreadWorker
{
public:
   ButtonCommandsExecutor(std::function<bool(const std::string&)> writer, std::function<void(bool)> callback):
   Utilities::ThreadWorker(std::bind(&ButtonCommandsExecutor::threadLoop, this), "CmdExec"),
   m_writer(writer),
   m_callback(callback),
   m_isActive(false)
   {

   }
   ~ButtonCommandsExecutor()
   {
      Utilities::ThreadWorker::stop();
   }
   void execute()
   {
      UT_Log(USER_BTN_HANDLER, LOW, "starting execution");
      std::lock_guard<std::mutex> lock(m_mutex);
      m_isActive = true;
      m_cond_var.notify_all();
   }
   int parseCommands(const std::string& text)
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "parsing new commands");

      std::lock_guard<std::mutex> lock(m_mutex);
      int result = 0;
      if (!m_isActive)
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
      while(Utilities::ThreadWorker::isRunning())
      {
         bool activated = false;
         {
            std::unique_lock<std::mutex> lock(m_mutex);
            activated = m_cond_var.wait_for(lock, std::chrono::milliseconds(200), [&](){return m_isActive;});
         }

         if (activated)
         {
            UT_Log(USER_BTN_HANDLER, HIGH, "Thread unlocked");
            std::vector<std::function<bool()>>::iterator it = m_commands.begin();
            while (it != m_commands.end() && Utilities::ThreadWorker::isRunning())
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

            {
               std::lock_guard<std::mutex> lock(m_mutex);
               m_isActive = false;
               if (m_callback)
               {
                  UT_Log(USER_BTN_HANDLER, LOW, "Command execution result %u", it == m_commands.end());
                  m_callback(it == m_commands.end());
               }
            }
         }
      }
   }

   std::string m_raw_text;
   std::function<bool(const std::string&)> m_writer;
   std::vector<std::function<bool()>> m_commands;
   std::function<void(bool)> m_callback;
   bool m_isActive;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;
};

}
