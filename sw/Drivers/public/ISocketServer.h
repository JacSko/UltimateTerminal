#ifndef _ISocketServer_H_
#define _ISocketServer_H_

/**
 * @file ISocketServer.h
 *
 * @brief
 *    Interface to control Socket Server - it is responsible for writing/reading data to/from socket.
 *
 * @details
 *    Data can be write to socket using write() method. Received data is provided through registered listeners.
 *    IP address cannot be empty. It is possible to provide either raw IP address and hostname. Port should be different than 0.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */

/* =============================
 *   Includes of common headers
 * =============================*/
#include "stdint.h"
#include <string>
#include <vector>
#include <functional>

namespace Drivers
{
namespace SocketServer
{

constexpr uint16_t SOCKET_MAX_PAYLOAD_LENGTH = 4096;

enum class ServerType
{
   RAW_DATA,
   SSL_ENCRYPTION,
};

enum class ServerEvent
{
   CLIENT_CONNECTED,    /**< Client connects successfully to server */
   CLIENT_DISCONNECTED, /**< Cient disconnected - connection closed or error appears */
   CLIENT_DATA_RECV,    /**< New data received from client */
};

/** This data mode must match the client mode!! */
enum class DataMode
{
   NEW_LINE_DELIMITER,  /**< Server waits for newline char, then notifies listeners about new data */
   PAYLOAD_HEADER,      /**< Server read header, then reads the payload basing on length in header, then notifies listeners about data */
};

class ServerListener
{
public:
   virtual ~ServerListener(){};
   /**
    * @brief Callback called on new event.
    * @param[in] client_id - id of the client that event refers to.
    * @param[in] data - Reference to vector with new data, can be empty, depends on event
    * @param[in] size - Size of the data, can be 0, depends on event.
    * @return None.
    */
   virtual void onServerEvent(int client_id, ServerEvent ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class ISocketServer
{
public:

   static std::unique_ptr<ISocketServer> create();
   /**
    * @brief Starts server.
    * @param[in] mode - server mode
    * @param[in] port - connection port (see @details).
    * @param[in] max_clients - defines how many clients can connect to server
    * @param[in] cert_file - SSL *.pem file
    * @param[in] key_file - SSL *.pem file
    * @return True if listening started, otherwise false.
    */
   virtual bool start(DataMode mode, uint16_t port, uint8_t max_clients = 1) = 0;
   /**
    * @brief Stops server.
    * @return void.
    */
   virtual void stop() = 0;
   /**
    * @brief Returns number of clients are connected.
    * @return Number of clients.
    */
   virtual uint32_t clientsCount() = 0;
   /**
    * @brief Adds listener to receive notifications about driver status or data received from client.
    * @return None.
    */
   virtual void addListener(ServerListener* callback) = 0;
   /**
    * @brief Removes listener.
    * @return None.
    */
   virtual void removeListener(ServerListener* callback) = 0;
   /**
    * @brief Writes data to client.
    * @param[in] data - bytes to write.
    * @param[in] size - number of bytes to write - if not provided, data.size() bytes will be written.
    */
   virtual bool write(const std::vector<uint8_t>& data, size_t size = 0) = 0;
   virtual ~ISocketServer(){};
};

}
}

#endif
