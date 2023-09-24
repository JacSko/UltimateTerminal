#ifndef LOGGER_WRITER_H_
#define LOGGER_WRITER_H_

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
/* =============================
 *  Includes of project headers
 * =============================*/
#include "Logger.h"
/* =============================
 *      Global variables
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/

class ILoggerWriter
{
public:
   /**
    * @brief Initialize the writer.
    * @return True if everything is OK, in case of problems with file open false is returned
    */
   virtual bool init () = 0;
   /**
    * @brief Deinitialize the writer.
    * @return void
    */
   virtual void deinit () = 0;
   /**
    * @brief Write log
    * @param[in] log - trace to write.
    * @return void
    */
   virtual void writeLog(const char* log) = 0;

   /**
    * @brief Returns writer type (according to LoggerWriterID enum)
    * @return Writer type
    */
   virtual int getType() = 0;

   virtual ~ILoggerWriter() {}
};

#endif
