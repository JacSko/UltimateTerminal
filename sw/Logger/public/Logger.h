#ifndef LOGGER_H_
#define LOGGER_H_

/* ============================= */
/**
 * @file Logger.h
 *
 * @brief Free functions used for logging
 *
 * @details
 *    Provides functionality to save debug traces to file, forward to standard output, send via socket, etc.
 *
 * @author Jacek Skowronek
 * @date 17/03/2022
 */
/* ============================= */
/* =============================
 *          Defines
 * =============================*/
/* Definitions of all logger groups system-wise */
#define LOGGER_LEVELS        \
   DEF_LOGGER_LEVEL(ASSERT)  \
   DEF_LOGGER_LEVEL(ALWAYS)  \
   DEF_LOGGER_LEVEL(ERROR)   \
   DEF_LOGGER_LEVEL(INFO)    \
   DEF_LOGGER_LEVEL(LOW)     \
   DEF_LOGGER_LEVEL(MEDIUM)  \
   DEF_LOGGER_LEVEL(HIGH)    \

#define LOGGER_GROUPS_RELEASE                  \
   DEF_LOGGER_GROUP(MAIN,             ERROR)   \
   DEF_LOGGER_GROUP(GUI_CONTROLLER,   ERROR)   \
   DEF_LOGGER_GROUP(SETTINGS,         ERROR)   \
   DEF_LOGGER_GROUP(SOCK_DRV,         ERROR)   \
   DEF_LOGGER_GROUP(UTILITIES,        ERROR)   \
   DEF_LOGGER_GROUP(SERIAL_DRV,       ERROR)   \
   DEF_LOGGER_GROUP(GUI_DIALOG,       ERROR)   \
   DEF_LOGGER_GROUP(TRACE_FILTER,     ERROR)   \
   DEF_LOGGER_GROUP(PORT_HANDLER,     ERROR)   \
   DEF_LOGGER_GROUP(USER_BTN_HANDLER, ERROR)   \
   DEF_LOGGER_GROUP(PERSISTENCE,      ERROR)   \
   DEF_LOGGER_GROUP(RPC_SERVER,       ERROR)   \
   DEF_LOGGER_GROUP(RPC_CLIENT,       ERROR)   \
   DEF_LOGGER_GROUP(TEST_FRAMEWORK,   ERROR)   \
   DEF_LOGGER_GROUP(TEST_SERVER,      ERROR)   \

#define LOGGER_GROUPS_DEBUG                   \
   DEF_LOGGER_GROUP(MAIN,             HIGH)   \
   DEF_LOGGER_GROUP(GUI_CONTROLLER,   HIGH)   \
   DEF_LOGGER_GROUP(SETTINGS,         HIGH)   \
   DEF_LOGGER_GROUP(SOCK_DRV,         HIGH)   \
   DEF_LOGGER_GROUP(UTILITIES,        HIGH)   \
   DEF_LOGGER_GROUP(SERIAL_DRV,       HIGH)   \
   DEF_LOGGER_GROUP(GUI_DIALOG,       HIGH)   \
   DEF_LOGGER_GROUP(TRACE_FILTER,     HIGH)   \
   DEF_LOGGER_GROUP(PORT_HANDLER,     HIGH)   \
   DEF_LOGGER_GROUP(USER_BTN_HANDLER, HIGH)   \
   DEF_LOGGER_GROUP(PERSISTENCE,      HIGH)   \
   DEF_LOGGER_GROUP(RPC_SERVER,       HIGH)   \
   DEF_LOGGER_GROUP(RPC_CLIENT,       HIGH)   \
   DEF_LOGGER_GROUP(TEST_FRAMEWORK,   HIGH)   \
   DEF_LOGGER_GROUP(TEST_SERVER,      HIGH)   \

#ifdef DEBUG
#define LOGGER_GROUPS LOGGER_GROUPS_DEBUG
#else
#define LOGGER_GROUPS LOGGER_GROUPS_RELEASE
#endif

#define LOGGER_WRITERS               \
   DEF_LOGGER_WRITER(STDOUT_WRITER)  \
   DEF_LOGGER_WRITER(SOCKET_WRITER)  \
   DEF_LOGGER_WRITER(FILE_WRITER)    \
   DEF_LOGGER_WRITER(ALL_WRITERS)    \

/* =============================
 *      Global variables
 * =============================*/

/* =============================
 *       Data structures
 * =============================*/
#undef DEF_LOGGER_GROUP
#define DEF_LOGGER_GROUP(name, def_level) name,
typedef enum
{
   LOGGER_GROUPS
   LOGGER_GROUP_MAX
} LoggerGroupID;
#undef DEF_LOGGER_GROUP


#undef DEF_LOGGER_LEVEL
#define DEF_LOGGER_LEVEL(name) name,
typedef enum
{
   LOGGER_LEVELS
   LOGGER_LEVEL_MAX
} LoggerLevelID;
#undef DEF_LOGGER_LEVEL

#undef DEF_LOGGER_WRITER
#define DEF_LOGGER_WRITER(name) name,
typedef enum
{
   LOGGER_WRITERS
   LOGGER_WRITERS_MAX
} LoggerWriterID;
#undef DEF_LOGGER_WRITER



/* =============================
 *   Includes of common headers
 * =============================*/
#include <stdint.h>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "LoggerEngine.h"



#define __FILENAME__ (strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__)

#if !defined UT_NO_DEBUG_OUTPUT
/**
 * @brief Puts data into logger, to be save according to current settings.
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Log(group,level,fmt,...) Logger::LoggerEngine::get()->putLog(ALL_WRITERS,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Puts data into logger conditionally, to be save according to current settings.
 * @param[in] cond - if true, log will be processed
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Log_If(cond,group,level,fmt,...) if(cond)Logger::LoggerEngine::get()->putLog(ALL_WRITERS,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Assertion with predefined message.
 * @param[in] cond - if true, application crash will be triggerred
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Assert(cond) if(!(cond))Logger::LoggerEngine::get()->assertion(ALL_WRITERS,__FILENAME__,__LINE__,#cond)
/**
 * @brief Puts data into logger, to be printed on standard output.
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Stdout_Log(group,level,fmt,...) Logger::LoggerEngine::get()->putLog(STDOUT_WRITER,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Puts data into logger conditionally, to be printed on standard output.
 * @param[in] cond - if true, log will be processed
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Stdout_Log_If(cond,group,level,fmt,...) if(cond)Logger::LoggerEngine::get()->putLog(STDOUT_WRITER,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Assertion with predefined message printed only on standard output.
 * @param[in] cond - if true, application crash will be triggerred
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define UT_Stdout_Assert(cond) if(!(cond))Logger::LoggerEngine::get()->assertion(STDOUT_WRITER,__FILENAME__,__LINE__,#cond)
/**
 * @brief Set level for defined group.
 * @param[in] group - logging group for level changing
 * @param[in] level - desired level
 * @return void
 */
#define UT_Set_Level(group, level) Logger::LoggerEngine::get()->setLevel(group, level)

/**
 * @brief Get level for defined group.
 * @param[in] group - logger group
 * @return Group level
 */
#define UT_Get_Level(group) Logger::LoggerEngine::get()->getLevel(group)

#else
#define UT_Log(group,level,fmt,...)
#define UT_Log_If(cond,group,level,fmt,...)
#define UT_Assert(cond)
#define UT_Stdout_Log(group,level,fmt,...)
#define UT_Stdout_Log_If(cond,group,level,fmt,...)
#define UT_Stdout_Assert(cond)
#define UT_Set_Level(group, level)
#define UT_Get_Level(group)
#endif

#endif
