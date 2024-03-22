#pragma once
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sstream>

#include <sys/prctl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>

namespace Utilities
{

class ApplicationExecutor
{
public:
   ApplicationExecutor():
   m_pid(-1),
   m_outputFd(-1)
   {

   }
   ~ApplicationExecutor()
   {
      stopApplication();
   }
   bool startApplication(const std::string& application_path)
   {
      pid_t ppid_before_fork = getpid();
      m_pid = fork();
      if (m_pid == 0)
      {
         int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
         if (r == -1) { perror(0); exit(1); }
         // test in case the original parent exited just
         // before the prctl() call
         if (getppid() != ppid_before_fork)
         {
          exit(1);
         }
         if (m_outputFd != -1)
         {
            if (dup2(m_outputFd, STDOUT_FILENO) == -1)
            {
               perror("Cannot bind pipe for stdout!\n");
               exit(1);
            }
            if (dup2(m_outputFd, STDERR_FILENO) == -1)
            {
               perror("Cannot bind pipe for stderr!\n");
               exit(1);
            }
         }
         /* execute the application */
         int res = system(application_path.c_str());
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
      bool result = false;
      if (isRunning())
      {
         result = (kill(m_pid, SIGINT) == 0);
      }
      return result;
   }
   bool isRunning()
   {
      return (m_pid != -1) && (kill(m_pid, 0) == 0);
   }
   void setOutputFd(int fd)
   {
      m_outputFd = fd;
   }
private:
   pid_t m_pid;
   int m_outputFd;
};

}
