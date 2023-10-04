#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <thread>
#include "Logger.h"

/**
 * @file ButtonCommandsParser.hpp
 *
 * @brief
 *    Class is responsible for parsing user input in textual form and converting to commands.
 *
 * @details
 *    Text input is divided into commands by splitting by '\n' char.
 *    There is also processing of special commands, that begins with '__' prefix (example: __wait(<time>)).
 *    Every __wait command longer than MAXIMUM_WAIT_TIME ms is divided into MAXIMUM_WAIT_TIME ms chunks.
 *    This allows to not block application shutdown when such wait is executed.
 *    Every instance if executor creates a separate thread for commands execution.
 *
 *    Supported special commands:
 *
 *    '__wait(<time_in_ms>)' - pause command execution for defined time. Time is interpreted as milliseconds, time_in_ms can be any value in range <1;UINT32_MAX>.
 *    '__repeat_start(<X>) and __repeat_end()' - repeats execution of commands enclosed by these commands X times. X can be any value in range <1;UINT32_MAX>.
 *                                               These loops can be nested, may also contains other special commands.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

namespace MainApplication
{

/** Maximum waiting time - see details for more description */
constexpr uint32_t MAXIMUM_WAIT_TIME = 500;

class ButtonCommandsParser
{
public:
   int parseCommands(const std::string& text, std::function<bool(const std::string&)> writer)
   {
      m_writer = writer;
      m_commands.clear();
      std::vector<std::function<bool()>> result;
      processStrings(splitTextToStrings(text));
      m_current_command = m_commands.begin();
      return m_commands.size();
   }
   void reset()
   {
      m_current_command = m_commands.begin();
   }
   bool runNext()
   {
      bool result = false;
      if (m_current_command != m_commands.end())
      {
         result = (*m_current_command++)();
      }
      return result;
   }
   int commandsCount()
   {
      return m_commands.size();
   }
   int commandsExecuted()
   {
      return std::distance(m_commands.begin(), m_current_command);
   }
private:
   std::vector<std::string> splitTextToStrings(const std::string& text)
   {
      std::vector<std::string> result;
      std::stringstream ss(text);
      std::string command;
      while(std::getline(ss, command, '\n'))
      {
         result.push_back(command);
      }
      return result;
   }
   void processStrings(const std::vector<std::string>& strings)
   {
      UT_Log(USER_BTN_HANDLER, LOW, "processing %d strings", strings.size());
      auto it = strings.begin();
      while (it != strings.end())
      {
         UT_Log(USER_BTN_HANDLER, HIGH, "command [%s]", (*it).c_str());
         if(isSpecialCommand(*it))
         {
            it += processSpecialCommand({it, strings.end()});
         }
         else
         {
            std::string command = *it;
            m_commands.push_back([this, command]()->bool { return m_writer(command);});
            it++;
         }
      }
   }
   bool isSpecialCommand(const std::string& command)
   {
      return command.find("__", 0) == 0;
   }
   int processSpecialCommand(const std::vector<std::string>& substrings)
   {
      UT_Log(USER_BTN_HANDLER, HIGH, "special command found");
      int commands_processed = 1;
      std::smatch matches;
      std::regex_match(substrings[0].cbegin(), substrings[0].cend(), matches, std::regex("(__.*)\\((.*)\\)"));
      UT_Log_If(matches.size() <=1, USER_BTN_HANDLER, ERROR, "cannot find regex in special command");
      if (matches.size() > 1)
      {
         int commandArgument = atoi(matches[2].str().c_str());
         if (matches[1] == "__wait")
         {
            UT_Log_If(commandArgument<0, USER_BTN_HANDLER, ERROR, "invalid wait time (%dms), skipping", commandArgument);
            if (commandArgument > 0)
            {
               prepareWaitCommand(commandArgument);
               commands_processed = 1;
            }
         }
         else if(matches[1] == "__repeat_start")
         {
            UT_Log_If(commandArgument<=0, USER_BTN_HANDLER, ERROR, "invalid repeat loop count (%d), commands used as normal", commandArgument);
            if (commandArgument > 0)
            {
               commands_processed = prepareRepeatCommand(substrings, commandArgument);
            }
         }
      }
      return commands_processed;
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
   int prepareRepeatCommand(const std::vector<std::string>& substrings,
                             uint32_t repeat_count)
   {
      int commands_processed = 1;
      auto repeat_end = findRepeatEnd(substrings.begin(), substrings.end());
      if (repeat_end == substrings.end())
      {
         UT_Log(USER_BTN_HANDLER, ERROR, "%s - cannot find loop end for command [%s]", __func__, substrings[0].c_str());
         return commands_processed;
      }
      std::vector<std::string> loop_commands = {substrings.begin() + 1, repeat_end};
      UT_Log(USER_BTN_HANDLER, HIGH, "%s - found loop end, loop contains %d commands", __func__, loop_commands.size());
      for (uint32_t i = 0; i < repeat_count; i++)
      {
         processStrings(loop_commands);
      }
      return loop_commands.size() + 2;
   }
   std::vector<std::string>::const_iterator findRepeatEnd(std::vector<std::string>::const_iterator begin,
                                                    std::vector<std::string>::const_iterator end)
   {
      std::vector<std::string>::const_iterator result = end;
      std::vector<std::string>::const_iterator it = begin + 1;
      uint32_t repeatEndsToSkip = 0;
      while (it != end)
      {
         if ((*it).find("__repeat_start") != std::string::npos)
         {
            UT_Log(USER_BTN_HANDLER, LOW, "Found inner loop [%s] inside [%s]", (*it).c_str(), (*begin).c_str());
            repeatEndsToSkip++;
         }
         if ((*it).find("__repeat_end") != std::string::npos)
         {
            if (repeatEndsToSkip)
            {
               repeatEndsToSkip--;
               UT_Log(USER_BTN_HANDLER, LOW, "Found repeat end for inner loop, skipping it. Left to skip: %d", repeatEndsToSkip);
            }
            else
            {
               UT_Log(USER_BTN_HANDLER, LOW, "Found loop end for [%s], returning iterator to [%s]", (*begin).c_str(), (*it).c_str());
               result = it;
               break;
            }
         }
         it++;
      }
      return result;
   }

   std::function<bool(const std::string&)> m_writer;
   std::vector<std::function<bool()>> m_commands;
   std::vector<std::function<bool()>>::iterator m_current_command;
};

} //MainApplication
