/* ============================= */
/**
 * @file LoggerEngine.cpp
 *
 * @brief Implementation of LoggerEngine class - the main class responsible for trace handling
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
#include <cstdio>
#include <string.h>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "LoggerEngine.h"
#include "Settings.h"
/* =============================
 *      Global variables
 * =============================*/
#undef DEF_LOGGER_GROUP
#define DEF_LOGGER_GROUP(name, def_level) #name,
std::array<std::string, LOGGER_GROUP_MAX> m_group_names = { LOGGER_GROUPS };
#undef DEF_LOGGER_GROUP

#undef DEF_LOGGER_GROUP
#define DEF_LOGGER_GROUP(name, def_level) def_level,
std::array<LoggerLevelID, LOGGER_GROUP_MAX> m_group_levels = { LOGGER_GROUPS };
#undef DEF_LOGGER_GROUP


#undef DEF_LOGGER_LEVEL
#define DEF_LOGGER_LEVEL(a) #a,
std::array<std::string, LOGGER_LEVEL_MAX> m_level_names = { LOGGER_LEVELS };
#undef DEF_LOGGER_LEVEL
/* =============================
 *       Data structures
 * =============================*/
namespace Logger
{

LoggerEngine* LoggerEngine::get()
{
   static LoggerEngine logger_engine;
   return &logger_engine;
}

void LoggerEngine::startFrontends(const std::string&)
{
}

void LoggerEngine::stopFrontends()
{
}

void LoggerEngine::refreshGroupLevelSettings()
{
}

LoggerLevelID LoggerEngine::getLevelFromString(const std::string& level)
{
   LoggerLevelID result = ERROR;
   for (uint32_t i = 0; i < LOGGER_LEVEL_MAX; i++)
   {
      if (m_level_names[i] == level)
      {
         result = (LoggerLevelID)i;
         break;
      }
   }
   return result;
}
void LoggerEngine::putLog(int logger_writer, int logger_group, int logger_level, const char* filename, int line, const char* fmt, ...)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   if (isLevelAllowed(logger_group, logger_level))
   {
      int idx = writeHeading(logger_group, logger_level, filename, line);

      va_list va;
      va_start(va, fmt);
      idx += vsprintf(m_buffer + idx, fmt, va);
      va_end(va);

      m_buffer[idx++] = '\n';
      m_buffer[idx] = 0x00;

      writeLog(logger_writer, m_buffer);
   }
}

bool LoggerEngine::isLevelAllowed(int logger_group, int logger_level)
{
   bool result = false;

   if (logger_level <= m_group_levels[logger_group])
   {
      result = true;
   }
   return result;
}

int LoggerEngine::writeHeading(int logger_group, int logger_level, const char* filename, int line)
{
   auto currentTime = std::chrono::system_clock::now();
   auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);

   return std::snprintf(m_buffer, LOGGER_BUFFER_SIZE, "[%.2u:%.2u:%.2u.%.3u]-%s-%s-%s:%u: ", ts->tm_hour, ts->tm_min, ts->tm_sec, (uint32_t)millis,
                        m_level_names[logger_level].c_str(), m_group_names[logger_group].c_str(), filename, line);

}

void LoggerEngine::assertion(int logger_writer, const char* filename, int line, const char* fmt)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   int idx = writeHeading(ASSERT, ASSERT, filename, line);

   idx += std::snprintf(m_buffer + idx, LOGGER_BUFFER_SIZE - idx, "Assertion failed(%s)!", fmt);

   m_buffer[idx++] = '\n';
   m_buffer[idx] = 0x00;

   writeLog(logger_writer, m_buffer);
   /* put assertion even in case of disabled stdout logging */
   printf("%s\n", m_buffer);

   std::abort();
}

void LoggerEngine::writeLog(int, const char* log)
{
   printf("%s", log);
}

void LoggerEngine::setLevel(LoggerGroupID id, LoggerLevelID level)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   m_group_levels[id] = level;
}
LoggerLevelID LoggerEngine::getLevel(LoggerGroupID id)
{
   std::lock_guard<std::recursive_mutex> lock(m_mutex);
   return m_group_levels[id];
}
std::string LoggerEngine::getGroupName(LoggerGroupID id)
{
   UT_Assert(id < LOGGER_GROUP_MAX);
   return m_group_names[id];
}
std::string LoggerEngine::getLevelName(LoggerLevelID level)
{
   UT_Assert(level < LOGGER_LEVEL_MAX);
   return m_level_names[level];
}
LoggerGroupID LoggerEngine::groupFromString(const std::string& name)
{
   LoggerGroupID result = LOGGER_GROUP_MAX;
   auto it = std::find(m_group_names.begin(), m_group_names.end(), name);
   if (it != m_group_names.end())
   {
      result = (LoggerGroupID)(std::distance(m_group_names.begin(), it));
   }
   return result;
}
LoggerLevelID LoggerEngine::levelFromString(const std::string& name)
{
   LoggerLevelID result = LOGGER_LEVEL_MAX;
   auto it = std::find(m_level_names.begin(), m_level_names.end(), name);
   if (it != m_level_names.end())
   {
      result = (LoggerLevelID)(std::distance(m_level_names.begin(), it));
   }
   return result;
}
void LoggerEngine::setPersistence(Utilities::Persistence::Persistence* persistence)
{
   Utilities::Persistence::PersistenceListener::setName("LOGGERENGINESTUB");
   m_persistence = persistence;
}
void LoggerEngine::onPersistenceWrite(PersistenceItems&)
{
}
void LoggerEngine::onPersistenceRead(const PersistenceItems&)
{
}

}
