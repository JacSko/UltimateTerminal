#pragma once

#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <thread>
#include "Logger.h"
#include "ThreadWorker.h"


namespace GUI
{

class ButtonCommandsExecutor : public Utilities::ThreadWorker
{
public:
   ButtonCommandsExecutor(std::function<bool(const std::string&)> writer, const std::string& raw_text):
   Utilities::ThreadWorker(std::bind(&ButtonCommandsExecutor::runAll, this), "CmdExec"),
   m_writer(writer),
   m_raw_text(raw_text),
   m_use_thread(false),
   m_isError(false)
   {
      createCommands();
   }
   ~ButtonCommandsExecutor()
   {
      Utilities::ThreadWorker::stop();
   }
   void execute(std::function<void(bool)> callback)
   {
      m_callback = callback;
      m_isError = false;
      if (m_use_thread)
      {
         m_isError = !start(1000);
      }
      else
      {
         runAll();
         if (callback) callback(!m_isError);
      }
   }
   const std::string& getText()
   {
      return m_raw_text;
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
         UT_Log(MAIN_GUI, HIGH, "got %u matches", matches.size());
         if (matches[1] == "__wait")
         {
            uint32_t time = std::stoi(matches[2]);
            m_use_thread = true;
            m_commands.push_back([time]()->bool {std::this_thread::sleep_for(std::chrono::milliseconds(time)); return true;});
         }
      }
   }
   void createCommands()
   {
      std::stringstream ss(m_raw_text);
      std::string command;
      while(std::getline(ss, command, '\n'))
      {
         if(isSpecialCommand(command))
         {
            addSpecialCommand(command);
         }
         else
         {
            command += '\n';
            m_commands.push_back([this, command]()->bool { return m_writer(command);});
         }
      }
      UT_Log(MAIN, LOW, "created %u commands", m_commands.size());
   }

   void runAll()
   {
      std::vector<std::function<bool()>>::iterator it = m_commands.begin();
      while (it != m_commands.end())
      {
         if (*it)
         {
            m_isError = !(*it)();
            if (m_isError)
            {
               break;
            }
         }
         it++;
      }
      m_callback(!m_isError);
   }

   std::string m_raw_text;
   std::function<bool(const std::string&)> m_writer;
   std::vector<std::function<bool()>> m_commands;
   bool m_use_thread;
   std::function<void(bool)> m_callback;
   std::atomic<bool> m_isError;
};

}
