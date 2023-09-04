#pragma once
#include <string>
#include <memory>

namespace MainApplication
{

class IFileLogger
{
public:
   virtual ~IFileLogger(){}
   static std::unique_ptr<IFileLogger> create();
   /**
    * @brief Checks if currently file logging is enabled.
    *
    * @return True if file already opened and ready to write log, otherwise false.
    *
    */
   virtual bool isActive() = 0;
   /**
    * @brief Creates and openes the file on the filesystem.
    *
    * If file already exists, it is cleared.
    *
    * @param[in] file - file path.
    *
    * @return True if file was opened, otherwise false.
    * @note: False is returned also, when the file is already opened.
    *
    */
   virtual bool openFile(const std::string& file) = 0;
   /**
    * @brief Close the already opened file. No action if file was not opened previously.
    *
    * @return None.
    *
    */
   virtual void closeFile() = 0;
   /**
    * @brief Writes log to the file.
    *
    * If file was not opened, no action is taken.
    *
    * @param[in] log - text to write.
    *
    * @return None.
    *
    */
   virtual void putLog(const std::string& log) = 0;
};

}
