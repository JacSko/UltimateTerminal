#pragma once
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sstream>

namespace TF
{

class ApplicationExecutor
{
public:
   ApplicationExecutor():
   m_pid(-1)
   {

   }
   bool startApplication(const std::string& application_path, const std::string& arguments)
   {

      m_pid = fork();
      if (m_pid == 0)
      {
         std::vector<std::string> splitted_arguments;
         std::stringstream ss (arguments);
         std::string arg;
         while(std::getline(ss, arg, ' '))
         {
            splitted_arguments.push_back(arg);
         }
         char* cmd_args [1 + splitted_arguments.size() + 1];
         cmd_args[0] = new char [application_path.size() + 1];
         std::strncpy(cmd_args[0], application_path.c_str(), application_path.size() + 1);
         for (uint8_t i = 1; i < splitted_arguments.size() + 1; i++)
         {
            cmd_args[i] = new char [splitted_arguments[i].size() + 1];
            std::strncpy(cmd_args[i], splitted_arguments[i-1].c_str(), splitted_arguments[i-1].size() + 1);
         }
         cmd_args[splitted_arguments.size() + 1] = 0x00;

         int res = execvp(application_path.c_str(), cmd_args);

         if (res < 0)
         {
            printf("Cannot run %s - returned %d, %s\n", application_path.c_str(), res, strerror(errno));
         }
         for (uint8_t i = 0; i < splitted_arguments.size() + 2; i++)
         {
            delete[] cmd_args[i];
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
