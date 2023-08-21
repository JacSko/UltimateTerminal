/* ============================= */
/**
 * @file TestLogger.cpp
 *
 * @brief Implementation of TestLogger class - the main class responsible for trace handling
 *
 * @details
 *    Provides functionality to save debug traces to file, forward to standard output, send via socket, etc.
 *
 * @author Jacek Skowronek
 * @date 18/03/2022
 */
/* ============================= */

/* =============================
 *          Defines
 * =============================*/

/* =============================
 *   Includes of common headers
 * =============================*/
#include <array>
#include <time.h>
#include <chrono>
#include <stdarg.h>
#include <cstdio>
#include <string.h>
#include <algorithm>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "TestLogger.h"
#include "StdoutWriter.h"
#include "FileWriter.h"
/* =============================
 *       Data structures
 * =============================*/

TestLogger* TestLogger::get()
{
   static TestLogger logger_engine;
   return &logger_engine;
}

TestLogger::TestLogger()
{
}

TestLogger::~TestLogger()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   for (auto& frontend : m_frontends)
   {
      frontend->deinit();
   }
}

void TestLogger::startFrontends()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   m_frontends.emplace_back(new LoggerStdoutWriter());
   m_frontends.emplace_back(new LoggerFileWriter(""));

   for (auto& frontend : m_frontends)
   {
      frontend->init();
   }
}

void TestLogger::stopFrontends()
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);

   for (auto& frontend : m_frontends)
   {
      frontend->deinit();
   }
   m_frontends.clear();
}

void TestLogger::putLog(const char* logger_group, const char* filename, int line, const char* fmt, ...)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   int idx = writeHeading(logger_group, filename, line);

   va_list va;
   va_start(va, fmt);
   idx += vsprintf(m_buffer + idx, fmt, va);
   va_end(va);

   m_buffer[idx++] = '\n';
   m_buffer[idx] = 0x00;

   writeLog(m_buffer);
}

int TestLogger::writeHeading(const char* logger_group, const char* filename, int line)
{
   auto currentTime = std::chrono::system_clock::now();
   auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);

   return std::snprintf(m_buffer, 4096, "[%.2u:%.2u:%.2u.%.3u]-%s-%s:%u: ", ts->tm_hour, ts->tm_min, ts->tm_sec, (uint32_t)millis,
                                                                                             logger_group, filename, line);

}

void TestLogger::assertion(const char* filename, int line, const char* fmt)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   int idx = writeHeading("ASSERT", filename, line);

   idx += std::snprintf(m_buffer + idx, 4096 - idx, "Assertion failed(%s)!", fmt);

   m_buffer[idx++] = '\n';
   m_buffer[idx] = 0x00;

   writeLog(m_buffer);
   /* put assertion even in case of disabled stdout logging */
   printf("%s\n", m_buffer);

   std::abort();
}

void TestLogger::writeLog(const char* log)
{
   for (auto& frontend : m_frontends)
   {
      frontend->writeLog(log);
   }
}

bool TestLogger::setLogFileName(const std::string& path)
{
   bool result = false;

   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   auto it = std::find_if(m_frontends.begin(), m_frontends.end(), [](std::unique_ptr<ILoggerWriter>& writer){return writer->getType() == FILE_WRITER;});
   if (it != m_frontends.end())
   {
      std::unique_ptr<ILoggerWriter> writer = std::unique_ptr<ILoggerWriter>(new LoggerFileWriter(path));
      result = writer->init();
      if (result)
      {
         (*it)->deinit();
         m_frontends.erase(it);
         m_frontends.emplace_back(std::move(writer));
      }
   }
   return result;
}










