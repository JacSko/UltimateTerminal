#pragma once
#include "ISerialDriver.h"
#include "ISocketClient.h"
#include "Logger.h"


namespace MainApplication
{

class DriversProxy : private Drivers::SocketClient::ClientListener,
                     private Drivers::Serial::SerialListener
{
public:
   enum class Event
   {
      ENDPOINT_DISCONNECTED,
      DATA_RECEIVED,
   };

   class Listener
   {
   public:
      virtual ~Listener(){};
      virtual void onEvent(Event event, const std::vector<uint8_t>& data, uint32_t size) = 0;
   };

   DriversProxy(const Drivers::Serial::Settings& settings, Listener* listener):
   m_serialDriver(Drivers::Serial::ISerialDriver::create()),
   m_socketClient(nullptr)
   {
      m_serialSettings = settings;
      m_listener = listener;
      m_ipAddress = "";
      m_ipPort = 0;
      m_serialDriver->addListener(this);
   }
   DriversProxy(const std::string& ipAddress, uint32_t port, Listener* listener):
   m_serialDriver(nullptr),
   m_socketClient(Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER))
   {
      m_serialSettings = {};
      m_listener = listener;
      m_ipAddress = ipAddress;
      m_ipPort = port;
      m_socketClient->addListener(this);
   }
   bool start()
   {
      if (m_serialDriver)
      {
         return m_serialDriver->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, m_serialSettings);
      }
      else
      {
         return m_socketClient->connect(m_ipAddress, m_ipPort);
      }
   }
   void stop()
   {
      if (m_serialDriver)
      {
         m_serialDriver->close();
      }
      else
      {
         m_socketClient->disconnect();
      }
   }
   bool isStarted()
   {
      if (m_serialDriver)
      {
         return m_serialDriver->isOpened();
      }
      else
      {
         return m_socketClient->isConnected();
      }
   }
   bool write(const std::vector<uint8_t>& data, size_t size)
   {
      if (m_serialDriver)
      {
         return m_serialDriver->write(data, size);
      }
      else
      {
         return m_socketClient->write(data, size);
      }
   }
private:
   void onSerialEvent(Drivers::Serial::DriverEvent event, const std::vector<uint8_t>& data, size_t size) override
   {
      if (m_listener) m_listener->onEvent(event == Drivers::Serial::DriverEvent::DATA_RECV? Event::DATA_RECEIVED : Event::ENDPOINT_DISCONNECTED, data, size);
   }
   void onClientEvent(Drivers::SocketClient::ClientEvent event, const std::vector<uint8_t>& data, size_t size) override
   {
      if (m_listener) m_listener->onEvent(event == Drivers::SocketClient::ClientEvent::SERVER_DATA_RECV? Event::DATA_RECEIVED : Event::ENDPOINT_DISCONNECTED, data, size);
   }

   std::unique_ptr<Drivers::Serial::ISerialDriver> m_serialDriver;
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socketClient;
   Drivers::Serial::Settings m_serialSettings;
   std::string m_ipAddress;
   uint32_t m_ipPort;
   Listener* m_listener;
};

}
