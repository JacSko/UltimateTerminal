#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "Logger.h"
#include "ISocketClient.h"

namespace RPC
{

/**
 * Wrapper for API structs to provide bool flag that represents the RPC invoke result.
 * If RPC transaction succeeds, the ready() method returns true and the object 'reply' is valid
 */
template<typename T>
class result
{
public:
   T reply;

   result(T& result):
   is_ready(true),
   reply(result)
   {

   }
   result():
   is_ready(false),
   reply{}
   {

   }
   result(bool result):
   is_ready(result),
   reply{}
   {

   }
   bool ready() const
   {
      return is_ready;
   }

private:
   bool is_ready;
};

constexpr uint16_t SOCKET_TRANSACTION_TIMEOUT = 100;

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
         m_buffer.clear();
         m_event_ready = false;

         if(!m_socket_client->isConnected())
         {
            UT_Log(RPC_CLIENT, ERROR, "not connected to server!");
            break;
         }

         serialize(m_buffer, request);
         if(!m_socket_client->write(m_buffer, m_buffer.size()))
         {
            UT_Log(RPC_CLIENT, ERROR, "write error");
            break;
         }
         if(!m_cond_var.wait_for(lock, std::chrono::milliseconds(SOCKET_TRANSACTION_TIMEOUT), [&](){return m_event_ready.load();}))
         {
            UT_Log(RPC_CLIENT, ERROR, "client response timeout");
            break;
         }
         if (m_last_event != Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV)
         {
            UT_Log(RPC_CLIENT, ERROR, "incorrect event received while waiting for response (%u)", (uint8_t)m_last_event.load());
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
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socket_client;

   std::vector<uint8_t> m_buffer;
   std::atomic<Drivers::SocketClient::ClientEvent> m_last_event;
   std::atomic<bool> m_event_ready;
   std::atomic<bool> m_transaction_ongoing;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;

};

}


#endif
