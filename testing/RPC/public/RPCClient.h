#pragma once


#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

#include "TestLogger.h"
#include "ISocketClient.h"
#include "RPCCommon.h"

namespace RPC
{

class RPCClient : public Drivers::SocketClient::ClientListener
{
public:
   RPCClient();
   /**
    * @brief Connects to RPC server.
    * @param[in] ip_address - ip address of remote server
    * @param[in] port - server's listening port
    * @return True if connected succesfully, otherwise false.
    */
   bool connect(const std::string& ip_address, uint16_t port);
   /**
    * @brief Disconnects from server.
    * @return None.
    */
   void disconnect();
   /**
    * @brief Registers handler for notifications sent by server
    * @return None.
    */
   void addNotificationHandler(uint8_t notification, std::function<bool(const std::vector<uint8_t>&)> Handler);
   /**
    * @brief Unregisters notification handler
    * @return None.
    */
   void removeNotificationHandler(uint8_t notification);
   /**
    * @brief Performs the RPC transation with remote client.
    *        The template types from RPCMessages.h shall be used.
    *        This method is blocking for maximum of SOCKET_TRANSACTION_TIMEOUT.
    * @return RPC::result<RETURN_TYPE> object which indicates the transaction result.
    */
   template<typename RETURN_TYPE, typename REQUEST_TYPE>
   result<RETURN_TYPE> invoke(REQUEST_TYPE& request)
   {
      m_transaction_ongoing = true;
      RETURN_TYPE response;
      do
      {
         std::unique_lock<std::mutex> lock(m_mutex);
         prepareHeader(m_buffer, MessageType::Request, (uint8_t)request.cmd);
         m_event_ready = false;

         if(!m_socket_client->isConnected())
         {
            TF_Log(RPC_CLIENT, "not connected to server!");
            break;
         }

         serialize(m_buffer, request);
         if(!m_socket_client->write(m_buffer, m_buffer.size()))
         {
            TF_Log(RPC_CLIENT, "write error");
            break;
         }
         if(!m_cond_var.wait_for(lock, std::chrono::milliseconds(SOCKET_TRANSACTION_TIMEOUT), [&](){return m_event_ready.load();}))
         {
            TF_Log(RPC_CLIENT, "server response timeout");
            break;
         }
         if (m_last_event != Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV)
         {
            TF_Log(RPC_CLIENT, "incorrect event received while waiting for response (%u)", (uint8_t)m_last_event.load());
            m_event_ready = false;
            break;
         }
         deserialize(m_buffer, response);
         break;
      }
      while(1);
      m_transaction_ongoing = false;
      if (!m_event_ready)
      {
         return result<RETURN_TYPE>(false);
      }
      return result<RETURN_TYPE>(response);
   }

private:
   void onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size);
   void prepareHeader(std::vector<uint8_t>& buffer, MessageType msg_type, uint8_t command);
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socket_client;

   std::vector<uint8_t> m_buffer;
   std::atomic<Drivers::SocketClient::ClientEvent> m_last_event;
   std::atomic<bool> m_event_ready;
   std::atomic<bool> m_transaction_ongoing;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;

   std::map<uint8_t, std::function<bool(const std::vector<uint8_t>&)>> m_ntf_handlers;
   std::mutex m_handlers_mutex;

};

}

