#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_

/* ============================= */
/**
 * @file FileWriter.h
 *
 * @brief Class resonsible for writing traces to defined file
 *
 * @author Jacek Skowronek
 * @date 18/03/2022
 */
/* ============================= */

/* =============================
 *          Defines
 * =============================*/

/* =============================
 *   Includes of common headers
 * =============================*/
#include <string>
#include <fstream>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "ILoggerWriter.h"
/* =============================
 *      Global variables
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/

class LoggerFileWriter : public ILoggerWriter
{
public:

   LoggerFileWriter(const std::string& file_path):
   m_path(file_path)
   {}
   /**
    * @brief Initialize the File writer.
    * @param[in[ file_path - path for log file
    * @return True if everything is OK, in case of problems with file open false is returned
    */
   bool init () override
   {
      bool result = false;
      m_file_stream.open(m_path, std::ios::out);
      if (m_file_stream)
      {
         result = true;
      }
      return result;
   }
   /**
    * @brief Deinitialize the File writer.
    * @return void
    */
   void deinit () override
   {
      m_file_stream.close();
   }
   /**
    * @brief Write log to file
    * @param[in] log - trace to write.
    * @return void
    */
   void writeLog(const char* log) override
   {
      if (m_file_stream)
      {
         m_file_stream << std::string(log);
         m_file_stream.flush();
      }
   }

   /**
    * @brief Returns writer type (according to LoggerWriterID enum)
    * @return Writer type
    */
   int getType() override
   {
      return FILE_WRITER;
   }

private:
   std::string m_path;
   std::ofstream m_file_stream;
};

#endif
