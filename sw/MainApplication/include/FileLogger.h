#pragma once

#include "IFileLogger.h"
#include <fstream>

class FileLogger : public IFileLogger
{
public:
   FileLogger();
   ~FileLogger();
private:
   bool isActive();
   bool openFile(const std::string& file);
   void closeFile();
   void putLog(const std::string& log);

   std::ofstream m_file_stream;
};
