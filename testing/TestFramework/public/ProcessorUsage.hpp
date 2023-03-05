#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Logger.h"

/**
 * @file ProcessorUsage.hpp
 *
 * @brief
 *    Checks the processor usage by defined PID.
 *
 * @details
 *    Usage in percent is calculated after double readout of values from "proc/<pid>/stat" with given period.
 *    Then data is parsed and converted to percents.
 *
 * @author Jacek Skowronek
 * @date   05/03/2023
 *
 */

namespace TF
{
/**
 * @brief Checks the CPU usage for given ProcessID.
 * @param[in] pid - ID of the process.
 * @param[in] timeout - time between consecutive readouts.
 * @return CPU usage value or 0.0 if no process found.
 */
static double getProcessorUsage(pid_t pid, uint32_t timeout = 1000)
{
   double result = 0.0;

   std::ifstream stat_file("/proc/" + std::to_string(pid) + "/stat");
   if (!stat_file.is_open()) {
       UT_Log(TEST_FRAMEWORK, ERROR, "Error: Could not open stat file for process %u", static_cast<uint32_t>(pid));
       return result;
   }

   std::string line;
   std::getline(stat_file, line);
   stat_file.close();

   long utime_ticks, stime_ticks, starttime_ticks;
   std::sscanf(line.c_str(), "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %lu", &utime_ticks, &stime_ticks, &starttime_ticks);

   // Get the clock ticks per second
   long clk_ticks = sysconf(_SC_CLK_TCK);
   // Calculate the total time elapsed in seconds
   double total_seconds = static_cast<double>(utime_ticks + stime_ticks) / clk_ticks;

   // Wait for defined timeout
   std::this_thread::sleep_for(std::chrono::milliseconds(timeout));

   std::ifstream stat_file2("/proc/" + std::to_string(pid) + "/stat");
   if (!stat_file2.is_open()) {
      UT_Log(TEST_FRAMEWORK, ERROR, "Error: Could not open stat file for process %u", static_cast<uint32_t>(pid));
      return result;
   }

   std::getline(stat_file2, line);
   stat_file2.close();

   // Extract the new values from the stat file
   std::sscanf(line.c_str(), "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %lu", &utime_ticks, &stime_ticks, &starttime_ticks);

   // Calculate the time elapsed between the two measurements
   double elapsed_seconds = static_cast<double>(utime_ticks + stime_ticks) / clk_ticks - total_seconds;

   // Calculate the CPU usage as a percentage
   result = elapsed_seconds / 1.0 * 100.0;

   return result;

}

}
