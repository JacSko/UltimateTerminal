#ifndef TEST_LOGGER_H_
#define TEST_LOGGER_H_

#include <stdint.h>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include <memory>

#include "ILoggerWriter.h"

typedef enum
{
   RPC_SERVER,
   RPC_CLIENT,
   TEST_FRAMEWORK,
   TEST_SERVER,
} LoggerGroupID;

typedef enum
{
   STDOUT_WRITER,
   SOCKET_WRITER,
   FILE_WRITER,
   ALL_WRITERS
} LoggerWriterID;

class TestLogger
{
public:
   /**
    * @brief Creates TestLogger object.
    */
   TestLogger();
   /**
    * @brief Destructs TestLogger object.
    */
   ~TestLogger();

   /**
    * @brief Get TestLogger singleton object
    */
   static TestLogger* get();
   void putLog(const char* logger_group, const char* filename, int line, const char* fmt, ...);
   void assertion(const char* filename, int line, const char* fmt);
   void startFrontends();
   void stopFrontends();
   bool setLogFileName(const std::string& path);

private:
   int writeHeading(const char* logger_group, const char* filename, int line);
   void writeLog(const char* log);
   char m_buffer [4096];
   std::recursive_mutex m_mutex;
   std::vector<std::unique_ptr<ILoggerWriter>> m_frontends;
   std::string m_file_name;
};




#define __FILENAME__ (strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__)

#define TF_Log(group,fmt,...) TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define TF_Log_If(cond,group,fmt,...) if(cond)TestLogger::get()->putLog(#group,__FILENAME__,__LINE__,fmt,##__VA_ARGS__)
#define TF_Assert(cond) if(!(cond))TestLogger::get()->assertion(__FILENAME__,__LINE__,#cond)

#endif
