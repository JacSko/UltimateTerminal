#pragma once

#include <memory>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <map>

#include "ISocketServer.h"
#include "RPCCommon.h"

namespace RPC
{

class RPCServer : public Drivers::SocketServer::ServerListener
{
public:
   RPCServer();
   /**
    * @brief Starts the RPC server.
    * @param[in] port - server's listening port
    * @return True if connected succesfully, otherwise false.
    */
   bool start(uint16_t port);
   /**
    * @brief Stops the server.
    * @return None.
    */
   void stop();
   /**
    * @brief Respond with serialized data to server.
    * @param[in] data - data buffer to send.
    * @return True is responded successfully.
    */
   template<typename RESPONSE_TYPE>
   bool respond(const RESPONSE_TYPE& response)
   {
      std::vector<uint8_t> buffer;
      buffer.push_back((uint8_t)MessageType::RequestResponse);
      buffer.push_back((uint8_t)response.cmd);
      serialize(buffer, response);
      return m_server->write(buffer, buffer.size());
   }
   /**
    * @brief Send notification to client.
    * @param[in] data - data buffer to send.
    * @return True is sent successfully.
    */
   template<typename NOTIFICATION_TYPE>
   bool notify(const NOTIFICATION_TYPE& notification)
   {
      std::vector<uint8_t> buffer;
      buffer.push_back((uint8_t)MessageType::Notification);
      buffer.push_back((uint8_t)notification.cmd);
      serialize(buffer, notification);
      return m_server->write(buffer, buffer.size());
   }
   /**
    * @brief Adds a command executor. Such executor is responsible for calling the respond() method
    * @param[in] cmd - command that executor is handling
    * @param[in] function - the function to be called on 'cmd' request.
    * @return None.
    */
   void addCommandExecutor(uint8_t cmd, std::function<bool(const std::vector<uint8_t>&)>);

   /**
    * @brief Removes the executor for command.
    * @param[in] cmd - id of the command.
    * @return None.
    */
   void removeCommandExecutor(uint8_t cmd);

private:
   void onServerEvent(int client_id, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size);
   std::unique_ptr<Drivers::SocketServer::ISocketServer> m_server;

   std::map<uint8_t, std::function<bool(const std::vector<uint8_t>&)>> m_executors;
   std::mutex m_executors_mutex;
};

}
