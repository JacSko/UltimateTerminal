#ifndef SOCKET_WRITER_H_
#define SOCKET_WRITER_H_

/* ============================= */
/**
 * @file FileWriter.h
 *
 * @brief Class resonsible for writing traces via passed ISocketServer i-face
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
#include <cstring>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "ILoggerWriter.h"
#include "ISocketDriverFactory.h"
/* =============================
 *      Global variables
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/

class LoggerSocketWriter : public ILoggerWriter
{
public:
   LoggerSocketWriter(uint16_t port):
   m_server(Drivers::SocketFactory::createServer(Drivers::SocketServer::ServerType::RAW_DATA)),
   m_port(port)
   {
   }

   /**
    * @brief Initialize the Socket writer.
    * @param[in[ port - path for log file
    * @return True if everything is OK, in case of problems with file open false is returned
    */
   bool init () override {return m_server->start(Drivers::SocketServer::DataMode::NEW_LINE_DELIMITER, m_port, 1);}
   /**
    * @brief Deinitialize the File writer.
    * @return void
    */
   void deinit () override {m_server->stop();}
   /**
    * @brief Write log to file
    * @param[in] log - trace to write.
    * @return void
    */
   void writeLog(const char* log) override
   {
      m_server->write({(const uint8_t*) log, (const uint8_t*) log + strlen(log)}, strlen(log));
   }

   /**
    * @brief Returns writer type (according to LoggerWriterID enum)
    * @return Writer type
    */
   int getType() override
   {
      return SOCKET_WRITER;
   }

private:
   std::unique_ptr<Drivers::SocketServer::ISocketServer> m_server;
   uint16_t m_port;
};

#endif
