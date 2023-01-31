#pragma once


#include <stdint.h>

namespace RPC
{

enum class MessageType
{
   Request,
   RequestResponse,
   Notification,
};

constexpr uint8_t RPC_MESSAGE_HEADER_SIZE = 2;
constexpr uint8_t RPC_MESSAGE_TYPE_BYTE_OFFSET = 0;
constexpr uint8_t RPC_COMMAND_BYTE_OFFSET = 1;
constexpr uint16_t SOCKET_TRANSACTION_TIMEOUT = 500;


template<typename T>
inline T convert(const std::vector<uint8_t>& data)
{
   T result = {};
   deserialize(data, result);
   return result;
}

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

}
