#ifndef _TEST_RPC_API_H
#define _TEST_RPC_API_H

#include <stdint.h>

namespace RPC
{
/**
 * This header file contains all entities required to serve the RPC communication.
 * To add new transaction, you should:
 * - Define a unique command <CMD> inside Command enum class
 * - Define the <CMD>Request structure
 * - Define the <CMD>Reply structure
 * - Overload the serialize/deserialize methods in RPCSerializers.h for newly defined structures
 */



/**
 * @brief Enum represents commands supported by RPC framework
 */
enum class Command : uint8_t
{

   /* define command below */
   /* Below Commands are used in integration tests - TODO rework to avoid using real API commands in tests */
   TestCommand1,
   TestCommand2,

};



struct TestCommand1Request
{
   Command cmd = Command::TestCommand1;
   uint32_t data1;
   uint8_t data2;
   uint16_t data3;
};

struct TestCommand1Reply
{
   Command cmd = Command::TestCommand1;
   bool result;
};

struct TestCommand2Request
{
   Command cmd = Command::TestCommand1;
   int8_t data1;
   std::string data2;
};

struct TestCommand2Reply
{
   Command cmd = Command::TestCommand1;
   std::string result;
};

}

#endif
