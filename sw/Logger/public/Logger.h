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
   DEF_LOGGER_LEVEL(ERROR)   \
   DEF_LOGGER_LEVEL(ALWAYS)  \
   DEF_LOGGER_LEVEL(INFO)    \
   DEF_LOGGER_LEVEL(LOW)     \
   DEF_LOGGER_LEVEL(MEDIUM)  \
   DEF_LOGGER_LEVEL(HIGH)    \

#define LOGGER_GROUPS                     \
   DEF_LOGGER_GROUP(MAIN,     ERROR)      \
   DEF_LOGGER_GROUP(SETTINGS, ERROR)      \
   DEF_LOGGER_GROUP(SOCK_DRV, HIGH)       \
   DEF_LOGGER_GROUP(UTILITIES, HIGH)      \
   DEF_LOGGER_GROUP(I2C_DRIVER, HIGH)     \
   DEF_LOGGER_GROUP(DHT_DRIVER, HIGH)     \
   DEF_LOGGER_GROUP(INT_DRIVER, HIGH)     \
   DEF_LOGGER_GROUP(RPC_SERVER, HIGH)     \
   DEF_LOGGER_GROUP(RPC_CLIENT, HIGH)     \
   DEF_LOGGER_GROUP(HC_API, HIGH)         \
   DEF_LOGGER_GROUP(BOARD, HIGH)          \
   DEF_LOGGER_GROUP(ENV_CTRL, HIGH)       \
   DEF_LOGGER_GROUP(TEST_CORE, HIGH)      \

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

/**
 * @brief Puts data into logger, to be save according to current settings.
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Log(group,level,fmt,...) LoggerEngine::get()->putLog(ALL_WRITERS,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Puts data into logger conditionally, to be save according to current settings.
 * @param[in] cond - if true, log will be processed
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Log_If(cond,group,level,fmt,...) if(cond)LoggerEngine::get()->putLog(ALL_WRITERS,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Assertion with predefined message.
 * @param[in] cond - if true, application crash will be triggerred
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Assert(cond) if(!(cond))LoggerEngine::get()->assertion(ALL_WRITERS,__FILENAME__,__LINE__,#cond)
/**
 * @brief Puts data into logger, to be printed on standard output.
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Stdout_Log(group,level,fmt,...) LoggerEngine::get()->putLog(STDOUT_WRITER,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Puts data into logger conditionally, to be printed on standard output.
 * @param[in] cond - if true, log will be processed
 * @param[in] logger_group - Logger group according to LoggerGroupID
 * @param[in] logger_level - Logger level according to LoggerLevelID
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Stdout_Log_If(cond,group,level,fmt,...) if(cond)LoggerEngine::get()->putLog(STDOUT_WRITER,group,level,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
/**
 * @brief Assertion with predefined message printed only on standard output.
 * @param[in] cond - if true, application crash will be triggerred
 * @param[in] fmt - String formatting
 * @param[in] ... - Arguments for formatting
 * @return void
 */
#define HC_Stdout_Assert(cond) if(!(cond))LoggerEngine::get()->assertion(STDOUT_WRITER,__FILENAME__,__LINE__,#cond)
/**
 * @brief Set level for defined group.
 * @param[in] group - logging group for level changing
 * @param[in] level - desired level
 * @return void
 */
#define HC_Set_Level(group, level) LoggerEngine::get()->setLevel(group, level)

/**
 * @brief Get level for defined group.
 * @param[in] group - logger group
 * @return Group level
 */
#define HC_Get_Level(group) LoggerEngine::get()->getLevel(group)


#endif
