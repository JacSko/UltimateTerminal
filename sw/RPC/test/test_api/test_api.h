#ifndef _TEST_RPC_API_H
#define _TEST_RPC_API_H

#include <stdint.h>
#include "Serialize.hpp"

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
   std::string result;
};

}

static void serialize(std::vector<uint8_t>& buffer, RPC::TestCommand1Request item)
{
   serialize(buffer, (uint8_t)item.cmd);
   serialize(buffer, item.data1);
   serialize(buffer, item.data2);
   serialize(buffer, item.data3);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::TestCommand1Reply item)
{
   serialize(buffer, (uint8_t)item.result);
}

static void serialize(std::vector<uint8_t>& buffer, RPC::TestCommand2Request item)
{
   serialize(buffer, (uint8_t)item.cmd);
   serialize(buffer, item.data1);
   serialize(buffer, item.data2);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::TestCommand2Reply item)
{
   serialize(buffer, item.result);
}

static void deserialize(const std::vector<uint8_t>& buffer, RPC::TestCommand1Request& item)
{
   uint32_t offset = 0;
   deserialize(buffer, offset, (uint8_t&)item.cmd);
   deserialize(buffer, offset, item.data1);
   deserialize(buffer, offset, item.data2);
   deserialize(buffer, offset, item.data3);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::TestCommand1Reply& item)
{
   uint32_t offset = 0;
   deserialize(buffer, offset, (uint8_t&)item.result);
}

static void deserialize(const std::vector<uint8_t>& buffer, RPC::TestCommand2Request& item)
{
   uint32_t offset = 0;
   deserialize(buffer, offset, (uint8_t&)item.cmd);
   deserialize(buffer, offset, item.data1);
   deserialize(buffer, offset, item.data2);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::TestCommand2Reply& item)
{
   uint32_t offset = 0;
   deserialize(buffer, offset, item.result);
}

#endif
