#ifndef _RPC_SERVER_H_
#define _RPC_SERVER_H_
#include <memory>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <map>

#include "ISocketServer.h"
#include "Logger.h"


namespace RPC
{

enum class Command : uint8_t;

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
   bool respond(const std::vector<uint8_t>&);
   /**
    * @brief Adds a command executor. Such executor is responsible for calling the respond() method
    * @param[in] cmd - command that executor is handling
    * @param[in] function - the function to be called on 'cmd' request.
    * @return None.
    */
   void addCommandExecutor(Command cmd, std::function<bool(const std::vector<uint8_t>&)>);
   /**
    * @brief Removes the executor for command.
    * @param[in] cmd - id of the command.
    * @return None.
    */
   void removeCommandExecutor(Command cmd);

private:
   void onServerEvent(int client_id, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size);
   std::unique_ptr<Drivers::SocketServer::ISocketServer> m_server;

   std::map<Command, std::function<bool(const std::vector<uint8_t>&)>> m_executors;
   std::mutex m_executors_mutex;
};

}


#endif
