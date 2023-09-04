#ifndef STDOUT_WRITER_H_
#define STDOUT_WRITER_H_

/* ============================= */
/**
 * @file StdoutWriter.h
 *
 * @brief Class resonsible for writing traces to standard output
 *
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
/* =============================
 *  Includes of project headers
 * =============================*/
#include "ILoggerWriter.h"
#include "Logger.h"
/* =============================
 *      Global variables
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/
namespace Logger
{

class LoggerStdoutWriter : public ILoggerWriter
{
public:
   /**
    * @brief Initialize the StdOut writer.
    * @return void
    */
   bool init () override {return true;}
   /**
    * @brief Deinitialize the StdOut writer.
    * @return void
    */
   void deinit () override {}
   /**
    * @brief Write log to standard output
    * @param[in] log - trace to write.
    * @return void
    */
   void writeLog(const char* log) override
   {
      printf("%s", log);
   }

   /**
    * @brief Returns writer type (according to LoggerWriterID enum)
    * @return Writer type
    */
   int getType() override
   {
      return STDOUT_WRITER;
   }
};

}
#endif
