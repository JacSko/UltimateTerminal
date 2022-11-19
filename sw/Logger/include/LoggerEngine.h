#ifndef LOGGER_ENGINE_H_
#define LOGGER_ENGINE_H_

/* ============================= */
/**
 * @file LoggerEngine.h
 *
 * @brief Definition of LoggerEngine class - the main class responsible for trace handling
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
#include <memory>
#include <stdint.h>
#include <stdarg.h>
#include <mutex>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "Logger.h"
#include "StdoutWriter.h"
#include "FileWriter.h"
#include "SocketWriter.h"
#include "ISocketServer.h"
/* =============================
 *      Global variables
 * =============================*/
static constexpr uint32_t LOGGER_BUFFER_SIZE = 4096;
/* =============================
 *       Data structures
 * =============================*/

class LoggerEngine
{
public:
   /**
    * @brief Creates LoggerEngine object.
    */
   LoggerEngine();
   /**
    * @brief Destructs LoggerEngine object.
    */
   ~LoggerEngine();

   /**
    * @brief Get LoggerEngine singleton object
    */
   static LoggerEngine* get();

   /**
    * @brief Puts data into logger, to be save according to current settings.
    * @param[in] logger_group - Logger group according to LoggerGroupID
    * @param[in] logger_level - Logger level according to LoggerLevelID
    * @param[in] filename - name of the file (from __FILE__ macro)
    * @param[in] line - line in code (from __LINE__ macro)
    * @param[in] fmt - String formatting
    * @param[in] ... - Arguments for formatting
    * @return void
    */
   void putLog(int logger_writer, int logger_group, int logger_level, const char* filename, int line, const char* fmt, ...);

   /**
    * @brief Assertion with file write.
    * @param[in] filename - name of the file (from __FILE__ macro)
    * @param[in] line - line in code (from __LINE__ macro)
    * @param[in] fmt - String formatting
    * @param[in] ... - Arguments for formatting
    * @return void
    */
   void assertion(int logger_writer, const char* filename, int line, const char* fmt);

   /**
    * @brief Starts all aux writers, like socket or file.
    * @return void
    */
   void startFrontends();

   /**
    * @brief Sets the logging level for defined group
    * @param[in] id - id of the logger group
    * @param[in] level - desired level
    * @return void
    */
   void setLevel(LoggerGroupID id, LoggerLevelID level);

   /**
    * @brief Gets the logging level for defined group
    * @param[in] id - id of the logger group
    * @return Current level for requested group
    */
   LoggerLevelID getLevel(LoggerGroupID id);

   /**
    * @brief Returns LoggerGroupID in textual form
    * @param[in] id - id of the logger group
    * @return LoggerGroupID name
    */
   std::string getGroupName(LoggerGroupID id);

   /**
    * @brief Returns LoggerLevelID in textual form
    * @param[in] level - id of the logger level
    * @return LoggerLevelID name
    */
   std::string getLevelName(LoggerLevelID level);

   /**
    * @brief Returns LoggerGroupID from textual form
    * @param[in] name - name of the group
    * @return LoggerGroupID as enum value
    */
   LoggerGroupID groupFromString(const std::string& name);

   /**
    * @brief Returns LoggerLevelID from textual form
    * @param[in] name - name of the level
    * @return LoggerLevelID as enum value
    */
   LoggerLevelID levelFromString(const std::string& name);

private:
   void refreshGroupLevelSettings();
   LoggerLevelID getLevelFromString(const std::string& level);
   bool isLevelAllowed(int logger_group, int logger_level);
   int writeHeading(int logger_group, int logger_level, const char* filename, int line);
   void writeLog(int logger_writer, const char* log);
   void prepareBuffer(int logger_group, int logger_level, const char* fmt, va_list list);
   char m_buffer [LOGGER_BUFFER_SIZE];
   std::recursive_mutex m_mutex;

   std::vector<std::unique_ptr<ILoggerWriter>> m_frontends;


};

#endif
