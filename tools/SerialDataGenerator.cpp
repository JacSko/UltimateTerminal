#include "ISerialDriver.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <atomic>

#define APP_VERSION "1.0"

Drivers::Serial::Settings g_settings;
uint32_t g_payloadRate = 100;
uint32_t g_payloadSize = 100;
std::string g_payload = "";

void printVersion()
{
   std::cout << "SerialDataGenerator v" << std::string(APP_VERSION) << std::endl;
   std::cout << "Author: Jacek Skowronek (jacekskowronekk@gmail.com)" << std::endl;
   std::cout << "Application designed to generate adjustable traffic on serial port" << std::endl;
}
void printHelp()
{
   printVersion();

   std::cout << "options:" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--device " <<std::endl;
   std::cout << "Serial device to use. The default value is  " << g_settings.device << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--baud " <<std::endl;
   std::cout << "Connection baud rate. Default is " << g_settings.baudRate.toName() << std::endl;
   std::cout << "Possible values:"<< std::endl;
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::BaudRate::BAUDRATE_MAX; i++)
   {
      EnumValue<Drivers::Serial::BaudRate> value ((Drivers::Serial::BaudRate)i);
      std::cout << value.toName() << std::endl;
   }
   std::cout << "-----" << std::endl;
   std::cout << "--parity " <<std::endl;
   std::cout << "Parity type. Default is " << g_settings.parityBits.toName() << std::endl;
   std::cout << "Possible values:"<< std::endl;
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::ParityType::PARITY_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::ParityType> value ((Drivers::Serial::ParityType)i);
      std::cout << value.toName() << std::endl;
   }
   std::cout << "-----" << std::endl;
   std::cout << "--stop " <<std::endl;
   std::cout << "Stop bits. Default is " << g_settings.stopBits.toName() << std::endl;
   std::cout << "Possible values:"<< std::endl;
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::StopBitType::STOP_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::StopBitType> value ((Drivers::Serial::StopBitType)i);
      std::cout << value.toName() << std::endl;
   }
   std::cout << "-----" << std::endl;
   std::cout << "--data " <<std::endl;
   std::cout << "Data bits. Default is " << g_settings.dataBits.toName() << std::endl;
   std::cout << "Possible values:"<< std::endl;
   for (uint8_t i = 0; i < (uint8_t)Drivers::Serial::DataBitType::DATA_BIT_MAX; i++)
   {
      EnumValue<Drivers::Serial::DataBitType> value ((Drivers::Serial::DataBitType)i);
      std::cout << value.toName() << std::endl;
   }
   std::cout << "-----" << std::endl;
   std::cout << "--payload_rate [1-10000]" << std::endl;
   std::cout << "Data send period in ms. The default value is 100ms, it means that payload will be send every 100ms" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--payload_size [1-4096]" << std::endl;
   std::cout << "Size of the payload in bytes. The default value is 100 bytes. Ignored when option --payload provided." << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << "--payload [data]" << std::endl;
   std::cout << "Payload to send. Defaults are \"0\" as text (0x30 hex)" << std::endl;
   std::cout << "-----" << std::endl;
   std::cout << std::endl;
   std::cout << "Example usage:" << std::endl;
   std::cout << std::endl;
   std::cout << "./SerialDataGenerator --device=/dev/ttyUSB0 --baud=BR_115200 --data=SIX --payload_rate=100 --payload=\"THIS IS EXAMPLE PAYLOAD\"" << std::endl;
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
bool parseDeviceCommand(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      g_settings.device = value;
      result = true;
   }
   return result;
}
bool parseBaudrateCommand(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      g_settings.baudRate.fromName(value);
      if (g_settings.baudRate != Drivers::Serial::BaudRate::BAUDRATE_MAX)
      {
         result = true;
      }
      else
      {
         std::cout << "Invalid baudrate " << value << std::endl;
      }
   }
   return result;
}
bool parseParityBitsCommand(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      g_settings.parityBits.fromName(value);
      if (g_settings.parityBits != Drivers::Serial::ParityType::PARITY_BIT_MAX)
      {
         result = true;
      }
      else
      {
         std::cout << "Invalid parity bits " << value << std::endl;
      }
   }
   return result;
}
bool parseStopBitsCommand(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      g_settings.stopBits.fromName(value);
      if (g_settings.stopBits != Drivers::Serial::StopBitType::STOP_BIT_MAX)
      {
         result = true;
      }
      else
      {
         std::cout << "Invalid stop bits " << value << std::endl;
      }
   }
   return result;
}
bool parseDataBitsCommand(const std::string& value)
{
   bool result = false;
   if (value.size())
   {
      g_settings.dataBits.fromName(value);
      if (g_settings.dataBits != Drivers::Serial::DataBitType::DATA_BIT_MAX)
      {
         result = true;
      }
      else
      {
         std::cout << "Invalid data bits " << value << std::endl;
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
bool parseCommand(const std::string& command, const std::string& value)
{
   bool result = false;
   if (command == "--help")
   {
      printHelp();
   }
   if (command == "--device")
   {
      result = parseDeviceCommand(value);
   }
   else if (command == "--baud")
   {
      result = parseBaudrateCommand(value);
   }
   else if (command == "--parity")
   {
      result = parseParityBitsCommand(value);
   }
   else if (command == "--stop")
   {
      result = parseStopBitsCommand(value);
   }
   else if (command == "--data")
   {
      result = parseDataBitsCommand(value);
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
void runApplication()
{
   std::unique_ptr<Drivers::Serial::ISerialDriver> serial_driver = Drivers::Serial::ISerialDriver::create();
   if (serial_driver)
   {
      if (serial_driver->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, g_settings))
      {
         while(1)
         {
            serial_driver->write({g_payload.begin(), g_payload.end()}, g_payloadSize);
            std::this_thread::sleep_for(std::chrono::milliseconds(g_payloadRate));
         }
      }
      else
      {
         std::cout << "Cannot connect to serial device " << g_settings.device << std::endl;
         std::cout << "Check connection settings first! " << std::endl;
      }
   }
   else
   {
      std::cout << "Cannot create ISerialDriver interface" << std::endl;
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
   std::cout << "Device? "<< g_settings.device << std::endl;
   std::cout << "baudrate? " << g_settings.baudRate.toName() << std::endl;
   std::cout << "paritytype? " << g_settings.parityBits.toName() << std::endl;
   std::cout << "stopbits? " << g_settings.stopBits.toName() << std::endl;
   std::cout << "databits? " << g_settings.dataBits.toName() << std::endl;
   std::cout << "Payload rate? " << g_payloadRate << std::endl;
   std::cout << "Payload size? " << g_payloadSize  << std::endl;
   std::cout << "Payload? " << g_payload << std::endl;
   std::cout << "Starting serial driver" << std::endl;
   runApplication();

}
