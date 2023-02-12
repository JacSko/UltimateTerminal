#ifndef _ISOCKETCLIENT_H_
#define _ISOCKETCLIENT_H_

/**
 * @file ISocketClient.h
 *
 * @brief
 *    Interface to control Socket Client - it is responsible for connecting to server and writing/reading data to/from socket.
 *
 * @details
 *
 * @author Jacek Skowronek
 * @date   08/04/2022
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
namespace SocketClient
{

constexpr uint16_t SOCKET_MAX_PAYLOAD_LENGTH = 4096;

enum class ClientType
{
   RAW_DATA,
   SSL_ENCRYPTION,
};

enum class ClientEvent
{
   SERVER_DISCONNECTED,    /**< Server closed it's connection */
   SERVER_DATA_RECV,       /**< New data received from server */
};

/** This data mode must match the client mode!! */
enum class DataMode
{
   NEW_LINE_DELIMITER,  /**< Client waits for newline char, then notifies listeners about new data */
   PAYLOAD_HEADER,      /**< Client read header, then reads the payload basing on length in header, then notifies listeners about data */
};

class ClientListener
{
public:
   virtual ~ClientListener(){};
   /**
    * @brief Callback called on new event.
    * @param[in] ev - Event received from server
    * @param[in] data - Reference to vector with new data, can be empty, depends on event
    * @param[in] size - Size of the data, can be 0, depends on event.
    * @return None.
    */
   virtual void onClientEvent(ClientEvent ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class ISocketClient
{
public:

   static std::unique_ptr<ISocketClient> create();
   /**
    * @brief Connects to server.
    * @param[in] ip_address - address of remote server in form "XXX.XXX.XXX.XXX"
    * @param[in] port - port number
    * @return true if connected successfully, otherwise false.
    */
   virtual bool connect(std::string ip_address, uint16_t port) = 0;
   /**
    * @brief Disconnects client from server.
    * @return void.
    */
   virtual void disconnect() = 0;
   /**
    * @brief Returns information if client is currently connected to the server.
    * @return True if connected.
    */
   virtual bool isConnected() = 0;
   /**
    * @brief Adds listener to receive notifications about driver status or data received from client.
    * @return None.
    */
   virtual void addListener(ClientListener* callback) = 0;
   /**
    * @brief Removes listener.
    * @return None.
    */
   virtual void removeListener(ClientListener* callback) = 0;
   /**
    * @brief Writes data to server.
    * @param[in] data - bytes to write.
    * @param[in] size - number of bytes to write - if not provided, data.size() bytes will be written.
    */
   virtual bool write(const std::vector<uint8_t>& data, size_t size = 0) = 0;
   virtual ~ISocketClient(){};
};

}
}

#endif
