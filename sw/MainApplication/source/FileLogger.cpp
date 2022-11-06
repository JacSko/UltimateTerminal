#include "FileLogger.h"


std::unique_ptr<IFileLogger> IFileLogger::create()
{
   return std::unique_ptr<IFileLogger>(new FileLogger);
}
FileLogger::FileLogger()
{

}
FileLogger::~FileLogger()
{
   closeFile();
}
bool FileLogger::isActive()
{
   return m_file_stream.is_open();
}
bool FileLogger::openFile(const std::string& file)
{
   bool result = false;
   if(!isActive())
   {
      m_file_stream.open(file, std::ios::out);
      if (m_file_stream)
      {
         result = true;
      }
   }
   return result;
}
void FileLogger::closeFile()
{
   if (isActive())
   {
      m_file_stream.close();
   }
}
void FileLogger::putLog(const std::string& log)
{
   if (isActive())
   {
      m_file_stream << log;
      m_file_stream.flush();
   }
}
