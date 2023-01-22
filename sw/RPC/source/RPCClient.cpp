#include "RPCClient.h"
#include "Logger.h"

namespace RPC
{

RPCClient::RPCClient():
m_socket_client(Drivers::SocketClient::ISocketClient::create()),
m_last_event(Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED),
m_event_ready(false),
m_transaction_ongoing(false)
{
   m_socket_client->addListener(this);
}

bool RPCClient::connect(const std::string& ip_address, uint16_t port)
{
   return m_socket_client->connect(Drivers::SocketClient::DataMode::PAYLOAD_HEADER, ip_address, port);
}

void RPCClient::disconnect()
{
   m_socket_client->disconnect();
}

void RPCClient::onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_mutex);
   switch(ev)
   {
   case Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED:
      break;
   case Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV:
      if (m_transaction_ongoing && !m_event_ready)
      {
         m_buffer = data;
         m_last_event = ev;
         m_event_ready = true;
         m_cond_var.notify_all();
      }
      else
      {
         UT_Log(RPC_SERVER, ERROR, "Data received when no expected!! transaction:%u, event %u", m_transaction_ongoing.load(), m_event_ready.load());
      }
      break;
   }
}


}
