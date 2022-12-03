#include "ISocketDriverFactory.h"
#include <iostream>

#define APP_VERSION "1.0"

bool processCommands()
{
   return false;
}

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
   std::cout << "--mode [server|client] - selects the socket type. In server mode application waits for first client, then start data generation. Default is CLIENT" << std::endl;
   std::cout << "--max_server_clients [1-10] - specifies how many parallel connections can be made to server. Option ignored when --mode is CLIENT. Default is 1" << std::endl;
   std::cout << "--address - IP address (IPv4) of remote server. Option ignored when --mode is SERVER. Default is 127.0.0.1" << std::endl;
   std::cout << "--port - socket port. Default is 1234" << std::endl;
   std::cout << "--payload_rate [1-10000] - data send period in ms. The default value is 100ms, it means that payload will be end every 100ms" << std::endl;
   std::cout << "--payload_size [1-4096] - size of the payload in bytes. The default value is 100 bytes. Ignored when option --payload provided." << std::endl;
   std::cout << "--payload [data] - Payload to send. Defaults are \"0\" as text (0x30 hex)" << std::endl;
   std::cout << "--exit_on_last_client [0-1] - Control if application shall be closed on last client disconnection. If no, application keeps listening for clients until manually closed." << std::endl;
   std::cout << std::endl;
   std::cout << "Example usage as server:" << std::endl;
   std::cout << std::endl;
   std::cout << "./SocketDataGenerator --mode server --max_server_clients 1 --port 1111 --payload_rate 100 --payload \"THIS IS EXAMPLE PAYLOAD\\n\"" << std::endl;
   std::cout << std::endl;
   std::cout << "Example usage as client:" << std::endl;
   std::cout << std::endl;
   std::cout << "./SocketDataGenerator --mode client --address 127.0.0.1 --port 1111 --payload_rate 100 --payload \"THIS IS EXAMPLE PAYLOAD\\n\"" << std::endl;
   std::cout << std::endl;
}

int main(int argc, char** argv)
{
   if (!processCommands())
   {
      printHelp();
   }
}
