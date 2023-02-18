#include "ISocketServer.h"
#include "ISocketClient.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <atomic>

#define APP_VERSION "1.0"

bool g_isClient = true;
uint8_t g_maxServerClients = 1;
std::string g_ipAddress = "127.0.0.1";
uint32_t g_ipPort = 1234;
uint32_t g_payloadRate = 100;
uint32_t g_payloadSize = 100;
std::string g_payload = "";
bool g_exitOnLastClient = true;

void printVersion()
{
   std::cout << "SocketDataGenerator v" << std::string(APP_VERSION) << std::endl;
   std::cout << "Author: Jacek Skowronek (jacekskowronekk@gmail.com)" << std::endl;
   std::cout << "Application designed to generate adjustable traffic on socket, acting as server or client" << std::endl;
}
void printHelp()
{
   printVersion();

   std::cout << "options:" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--mode [server|client]" << std::endl;
   std::cout << "Selects the socket type. In server mode application waits for first client, then start data generation. Default is CLIENT" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--max_server_clients [1-10]" << std::endl;
   std::cout << "Specifies how many parallel connections can be made to server. Option ignored when --mode is CLIENT. Default is 1" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--address" << std::endl;
   std::cout << "IP address (IPv4) of remote server. Option ignored when --mode is SERVER. Default is 127.0.0.1" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--port" << std::endl;
   std::cout << "Socket port. Default is 1234" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--payload_rate [1-10000]" << std::endl;
   std::cout << "Data send period in ms. The default value is 100ms, it means that payload will be end every 100ms" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--payload_size [1-4096]" << std::endl;
   std::cout << "Size of the payload in bytes. The default value is 100 bytes. Ignored when option --payload provided." << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--payload [data]" << std::endl;
   std::cout << "Payload to send. Defaults are \"0\" as text (0x30 hex)" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--exit_on_last_client [0-1]" << std::endl;
   std::cout << "Control if application shall be closed on last client disconnection. If no, application keeps listening for clients until manually closed. Default is true" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << std::endl;
   std::cout << "Example usage as server:" << std::endl;
   std::cout << std::endl;
   std::cout << "./SocketDataGenerator --mode=server --max_server_clients=1 --port=1111 --payload_rate=100 --payload=\"THIS IS EXAMPLE PAYLOAD\"" << std::endl;
   std::cout << std::endl;
   std::cout << "Example usage as client:" << std::endl;
   std::cout << std::endl;
   std::cout << "./SocketDataGenerator --mode=client --address=127.0.0.1 --port=1111 --payload_rate=100 --payload=\"THIS IS EXAMPLE PAYLOAD\"" << std::endl;
   std::cout << std::endl;
}
bool isCommand (const std::string& text)
{
   bool result = false;
   if (text.size() > 2)
   {
      if (text.find("--", 0) == 0)
      {
         result = true;
      }
   }
   return result;
}
bool parseModeCommand(const std::string& value)
{
   bool result = false;
   if (value == "server")
   {
      g_isClient = false;
      result = true;
   }
   else if (value == "client")
   {
      g_isClient = true;
      result = true;
   }
   return result;
}
bool parseMaxServerClients(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      uint32_t max_clients = std::stoi(value);
      if ((max_clients >= 1) && (max_clients <= 10))
      {
         result = true;
         g_maxServerClients = max_clients;
      }
   }
   return result;
}
bool parseAddress(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      result = true;
      g_ipAddress = value;
   }
   return result;
}
bool parsePort(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      uint32_t port = std::stoi(value);
      if (port != 0)
      {
         result = true;
         g_ipPort = port;
      }
   }
   return result;
}
bool parsePayloadRate(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      uint32_t payload_rate = std::stoi(value);
      if ((payload_rate >= 1) && (payload_rate <= 10000))
      {
         result = true;
         g_payloadRate = payload_rate;
      }
   }
   return result;
}
bool parsePayloadSize(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      uint32_t payload_size = std::stoi(value);
      if ((payload_size >= 1) && (payload_size <= 4096))
      {
         result = true;
         g_payloadSize = payload_size;
      }
   }
   return result;
}
bool parsePayload(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      result = true;
      g_payload = value;
      g_payloadSize = value.size();
   }
   return result;
}
bool parseExitOnLastClient(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      result = true;
      g_exitOnLastClient = (bool)std::stoi(value);
   }
   return result;
}
bool parseCommand(const std::string& command, const std::string& value)
{
   bool result = false;
   if (command == "--help")
   {
      printHelp();
   }
   if (command == "--mode")
   {
      result = parseModeCommand(value);
   }
   else if (command == "--max_server_clients")
   {
      result = parseMaxServerClients(value);
   }
   else if (command == "--address")
   {
      result = parseAddress(value);
   }
   else if (command == "--port")
   {
      result = parsePort(value);
   }
   else if (command == "--payload_rate")
   {
      result = parsePayloadRate(value);
   }
   else if (command == "--payload_size")
   {
      result = parsePayloadSize(value);
   }
   else if (command == "--payload")
   {
      result = parsePayload(value);
   }
   else if (command == "--exit_on_last_client")
   {
      result = parseExitOnLastClient(value);
   }
   return result;
}
bool processCommands(int argc, char** argv)
{
   bool result = false;

   for (uint8_t i = 1; i < argc; i++)
   {
      if (isCommand(argv[i]))
      {
         std::stringstream ss (argv[i]);
         std::string command = "";
         std::string value = "";
         std::getline(ss, command, '=');
         std::getline(ss, value, '=');
         result = parseCommand(command, value);
         if (!result)
         {
            break;
         }
      }
      else
      {
         std::cout << "[" << argv[i] << "] is not a valid option!" << std::endl;
         break;
      }
   }
   return result;
}
void runClientMode()
{
   std::unique_ptr<Drivers::SocketClient::ISocketClient> socket_client = Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER);
   if (socket_client)
   {
      if (socket_client->connect(g_ipAddress, g_ipPort))
      {
         while(1)
         {
            socket_client->write({g_payload.begin(), g_payload.end()}, g_payloadSize);
            std::this_thread::sleep_for(std::chrono::milliseconds(g_payloadRate));
         }
      }
      else
      {
         std::cout << "Cannot connect to server " << g_ipAddress << ":" << g_ipPort << std::endl;
      }
   }
   else
   {
      std::cout << "Cannot create ISocketClient interface" << std::endl;
   }
}

void runServerMode()
{
   std::unique_ptr<Drivers::SocketServer::ISocketServer> socket_server = Drivers::SocketServer::ISocketServer::create(Drivers::SocketServer::DataMode::NEW_LINE_DELIMITER);
   std::condition_variable cond_var;
   std::mutex mutex;
   std::atomic<uint8_t> clients_count = 0;

   if (socket_server)
   {
      class ServerCallback : public Drivers::SocketServer::ServerListener
      {
      public:
         ServerCallback(std::mutex& mutex, std::condition_variable& cond_var, std::atomic<uint8_t>& clients_count):
         cond_var(cond_var),
         mutex(mutex),
         clients_count(clients_count)
         {

         }
         void onServerEvent(int, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>&, size_t)
         {
            if (ev == Drivers::SocketServer::ServerEvent::CLIENT_CONNECTED)
            {
               std::unique_lock<std::mutex> lock(mutex);
               clients_count++;
               cond_var.notify_all();
            }
            else if (ev == Drivers::SocketServer::ServerEvent::CLIENT_DISCONNECTED)
            {
               std::unique_lock<std::mutex> lock(mutex);
               clients_count--;
            }
         }
      private:
         std::condition_variable& cond_var;
         std::mutex& mutex;
         std::atomic<uint8_t>& clients_count ;
      };

      ServerCallback callback (mutex, cond_var, clients_count);
      socket_server->addListener(&callback);
      if (socket_server->start(g_ipPort, g_maxServerClients))
      {
         do
         {
            {
               std::unique_lock<std::mutex> lock(mutex);
               std::cout << "Waiting for client on port " << g_ipPort << std::endl;
               cond_var.wait(lock, [&](){return clients_count.load();});
               std::cout << "First client connected, starting..." << std::endl;
            }
            while (clients_count)
            {
               socket_server->write({g_payload.begin(), g_payload.end()}, g_payloadSize);
               std::this_thread::sleep_for(std::chrono::milliseconds(g_payloadRate));
            }
            std::cout << "Last client disconnected, stopping" << std::endl;
         } while(!g_exitOnLastClient);
      }
      else
      {
         std::cout << "Cannot connect to server " << g_ipAddress << ":" << g_ipPort << std::endl;
      }
   }
   else
   {
      std::cout << "Cannot create ISocketClient interface" << std::endl;
   }
}

int main(int argc, char** argv)
{
   if (!processCommands(argc, argv))
   {
      std::cout << "Invalid options, type --help for usage guide" << std::endl;
      return -1;
   }
   if (g_payload.empty())
   {
      g_payload = std::string(g_payloadSize, '0');
   }
   g_payload += '\n';
   g_payloadSize ++;

   std::cout << "Starting program with parameters:" << std::endl;
   std::cout << "Server? "<< g_isClient << std::endl;
   std::cout << "Max server clients? " << g_maxServerClients << std::endl;
   std::cout << "IP Address? " << g_ipAddress << std::endl;
   std::cout << "IP Port? " << g_ipPort << std::endl;
   std::cout << "Payload rate? " << g_payloadRate << std::endl;
   std::cout << "Payload size? " << g_payloadSize  << std::endl;
   std::cout << "Payload? " << g_payload << std::endl;
   std::cout << "Exit on last client? " << g_exitOnLastClient << std::endl;

   if (g_isClient)
   {
      std::cout << "Starting socket client" << std::endl;
      runClientMode();
   }
   else
   {
      std::cout << "Starting socket server" << std::endl;
      runServerMode();
   }

}
