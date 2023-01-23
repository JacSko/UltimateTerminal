#include "RPCClient.h"
#include "Logger.h"
#include "IRPCSocketDriverFactory.h"
namespace RPC
{

RPCClient::RPCClient():
m_socket_client(RPC::SocketFactory::createClient(SocketClient::ClientType::RAW_DATA)),
m_last_event(SocketClient::ClientEvent::SERVER_DISCONNECTED),
m_event_ready(false),
m_transaction_ongoing(false)
{
   m_socket_client->addListener(this);
}

bool RPCClient::connect(const std::string& ip_address, uint16_t port)
{
   return m_socket_client->connect(SocketClient::DataMode::PAYLOAD_HEADER, ip_address, port);
}

void RPCClient::disconnect()
{
   m_socket_client->disconnect();
}
void RPCClient::addNotificationHandler(uint8_t notification, std::function<bool(const std::vector<uint8_t>&)> handler)
{
   std::lock_guard<std::mutex> lock (m_handlers_mutex);
   m_ntf_handlers[notification] = handler;
}
void RPCClient::removeNotificationHandler(uint8_t notification)
{
   std::lock_guard<std::mutex> lock (m_handlers_mutex);
   m_ntf_handlers.erase(notification);
}
void RPCClient::onClientEvent(SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   switch(ev)
   {
   case SocketClient::ClientEvent::SERVER_DISCONNECTED:
      break;
   case SocketClient::ClientEvent::SERVER_DATA_RECV:
      if (m_transaction_ongoing
          && !m_event_ready
          && (data[RPC_MESSAGE_TYPE_BYTE_OFFSET] == (uint8_t)MessageType::RequestResponse))
      {
         m_buffer = std::vector<uint8_t>(data.begin() + RPC_MESSAGE_HEADER_SIZE, data.end());
         m_last_event = ev;
         m_event_ready = true;
         m_cond_var.notify_all();
      }
      else if (data[RPC_MESSAGE_TYPE_BYTE_OFFSET] == (uint8_t)MessageType::Notification)
      {
         std::lock_guard<std::mutex> lock (m_handlers_mutex);
         if (m_ntf_handlers[data[RPC_COMMAND_BYTE_OFFSET]])
         {
            m_ntf_handlers[data[RPC_COMMAND_BYTE_OFFSET]](std::vector<uint8_t>(data.begin() + RPC_MESSAGE_HEADER_SIZE, data.end()));
         }
      }
      else
      {
         UT_Log(RPC_SERVER, ERROR, "Data received when no expected!! transaction:%u, event %u", m_transaction_ongoing.load(), m_event_ready.load());
      }
      break;
   }
}

void RPCClient::prepareHeader(std::vector<uint8_t>& buffer, MessageType msg_type, uint8_t command)
{
   buffer.clear();
   buffer.push_back((uint8_t)msg_type);
   buffer.push_back((uint8_t)command);
}


}
