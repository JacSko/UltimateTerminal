#pragma once
#include <string>
#include <unistd.h>
#include <signal.h>

namespace TestFramework
{

class ApplicationExecutor
{
public:
   ApplicationExecutor():
   m_pid(-1)
   {

   }
   bool startApplication(const std::string& application_path, const char* arguments)
   {
      m_pid = fork();
      if (m_pid == 0)
      {
         int res = execl(application_path.c_str(), arguments);
         if (res < 0)
         {
            printf("Cannot run %s - returned %d, %s\n", application_path.c_str(), res, strerror(errno));
         }
         exit(1);
      }
      return m_pid != -1;
   }
   bool stopApplication()
   {
      return (kill(m_pid, SIGINT) == 0);
   }
   bool isRunning()
   {
      return (kill(m_pid, 0) == 0);
   }
private:
   pid_t m_pid;
};

}
