#pragma once

#include "TestLogger.h"

#define LOGGER_LEVELS        \
   DEF_LOGGER_LEVEL(ASSERT)  \
   DEF_LOGGER_LEVEL(ERROR)   \
   DEF_LOGGER_LEVEL(ALWAYS)  \
   DEF_LOGGER_LEVEL(INFO)    \
   DEF_LOGGER_LEVEL(LOW)     \
   DEF_LOGGER_LEVEL(MEDIUM)  \
   DEF_LOGGER_LEVEL(HIGH)    \

#define LOGGER_GROUPS                         \
   DEF_LOGGER_GROUP(SOCK_DRV,         HIGH)   \
   DEF_LOGGER_GROUP(UTILITIES,        HIGH)   \
   DEF_LOGGER_GROUP(SERIAL_DRV,       HIGH)   \
   DEF_LOGGER_GROUP(RPC_SERVER,       HIGH)   \
   DEF_LOGGER_GROUP(RPC_CLIENT,       HIGH)   \
   DEF_LOGGER_GROUP(TEST_FRAMEWORK,   HIGH)   \
   DEF_LOGGER_GROUP(TEST_SERVER,      HIGH)   \

#define LOGGER_WRITERS               \
   DEF_LOGGER_WRITER(STDOUT_WRITER)  \
   DEF_LOGGER_WRITER(SOCKET_WRITER)  \
   DEF_LOGGER_WRITER(FILE_WRITER)    \
   DEF_LOGGER_WRITER(ALL_WRITERS)    \

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


#define __FILENAME__ (strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__)

#define TF_Log(group,fmt,...) TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define TF_Log_If(cond,group,fmt,...) if(cond)TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define TF_Assert(cond) if(!(cond))TestLogger::get()->assertion(__FILENAME__,__LINE__,#cond)


//override for macros used in production build to allow use common components
#define UT_Log(group,level,fmt,...) TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define UT_Log_If(cond,group,level,fmt,...) if(cond)TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define UT_Assert(cond) if(!(cond))TestLogger::get()->assertion(__FILENAME__,__LINE__,#cond)
#define UT_Stdout_Log(group,level,fmt,...) TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define UT_Stdout_Log_If(cond,group,level,fmt,...) if(cond)TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define UT_Stdout_Assert(cond) if(!(cond))TestLogger::get()->assertion(__FILENAME__,__LINE__,#cond)

