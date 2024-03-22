#pragma once
#include "ISerialDriver.h"
#include "ISocketClient.h"
#include "IProcessDriver.h"
#include "Logger.h"


namespace MainApplication
{

class DriversProxy : private Drivers::SocketClient::ClientListener,
                     private Drivers::Serial::SerialListener,
                     private Drivers::Process::ProcessListener
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
   m_socketClient(nullptr),
   m_processDriver(nullptr)
   {
      m_serialSettings = settings;
      m_listener = listener;
      m_ipAddress = "";
      m_ipPort = 0;
      m_processName = "";
      m_serialDriver->addListener(this);
   }
   DriversProxy(const std::string& ipAddress, uint32_t port, Listener* listener):
   m_serialDriver(nullptr),
   m_socketClient(Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER)),
   m_processDriver(nullptr)
   {
      m_serialSettings = {};
      m_listener = listener;
      m_ipAddress = ipAddress;
      m_ipPort = port;
      m_processName = "";
      m_socketClient->addListener(this);
   }
   DriversProxy(const std::string& process, Listener* listener):
   m_serialDriver(nullptr),
   m_socketClient(nullptr),
   m_processDriver(Drivers::Process::IProcessDriver::create())
   {
      m_serialSettings = {};
      m_listener = listener;
      m_ipAddress = "";
      m_ipPort = 0;
      m_processName = process;

      m_processDriver->addListener(this);
   }
   bool start()
   {
      if (m_serialDriver)
      {
         return m_serialDriver->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, m_serialSettings);
      }
      else if (m_socketClient)
      {
         return m_socketClient->connect(m_ipAddress, m_ipPort);
      }
      else
      {
         return m_processDriver->start(m_processName);
      }
   }
   void stop()
   {
      if (m_serialDriver)
      {
         m_serialDriver->close();
      }
      else if (m_socketClient)
      {
         m_socketClient->disconnect();
      }
      else
      {
         return m_processDriver->stop();
      }
   }
   bool isStarted()
   {
      if (m_serialDriver)
      {
         return m_serialDriver->isOpened();
      }
      else if (m_socketClient)
      {
         return m_socketClient->isConnected();
      }
      else
      {
         return m_processDriver->isStarted();
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
   void onProcessEvent(Drivers::Process::Event event, const std::vector<uint8_t>& data, size_t size) override
   {
      if (m_listener)
      {
         Event proxyEvent = Event::DATA_RECEIVED;
         if (event == Drivers::Process::Event::PROCESS_STOPPED)
         {
            proxyEvent = Event::ENDPOINT_DISCONNECTED;
         }
         m_listener->onEvent(proxyEvent, data, size);
      }
   }

   std::unique_ptr<Drivers::Serial::ISerialDriver> m_serialDriver;
   std::unique_ptr<Drivers::SocketClient::ISocketClient> m_socketClient;
   std::unique_ptr<Drivers::Process::IProcessDriver> m_processDriver;
   Drivers::Serial::Settings m_serialSettings;
   std::string m_ipAddress;
   uint32_t m_ipPort;
   std::string m_processName;
   Listener* m_listener;
};

}
