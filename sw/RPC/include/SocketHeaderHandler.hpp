#ifndef __SOCKET_HEADER_HANDLER_HPP_
#define __SOCKET_HEADER_HANDLER_HPP_
#include <vector>
#include <stdint.h>
#include "Logger.h"
#include "Serialize.hpp"

namespace Drivers
{

class HeaderHandler
{
public:

   /* the default header size */
   static constexpr uint32_t HEADER_SIZE = 4;

   /**
    * @brief Generates the header basing on payload.
    * @param[in] output_buffer - place, where new header shall be written
    * @param[in] payload - data intended for header calculation.
    * @return None.
    */
   void preapreHeader(std::vector<uint8_t>& output_buffer, uint32_t data_size)
   {
      serialize(output_buffer, data_size);
   }

   /**
    * @brief Decodes the header basing on payload.
    * @param[in] data - received header.
    * @return Message length.
    */
   uint32_t decodeMessageLength (std::vector<uint8_t>& data)
   {
      uint32_t result = 0;
      uint32_t offset = 0;
      deserialize(data, offset, result);

      return result;
   }

   /**
    * @brief Returns the default header size
    * @return Header size.
    */
   uint32_t getHeaderSize()
   {
      return HEADER_SIZE;
   }
};

}

#endif
