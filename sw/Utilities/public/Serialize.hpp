#ifndef _SERIALIZE_HPP_
#define _SERIALIZE_HPP_

#include <vector>
#include <stdint.h>
#include <iostream>

template<typename ITEM_TYPE>
static void serialize(std::vector<uint8_t>& buffer, ITEM_TYPE item)
{
   printf("SERIALIZE ERROR!\n");
}

static void serialize(std::vector<uint8_t>& buffer, bool item)
{
   buffer.push_back((uint8_t)item);
}

static void serialize(std::vector<uint8_t>& buffer, uint8_t item)
{
   buffer.push_back(item);
}

static void serialize(std::vector<uint8_t>& buffer, uint16_t item)
{
   buffer.push_back((item >> 8) & 0xFF);
   buffer.push_back((item) & 0xFF);
}

static void serialize(std::vector<uint8_t>& buffer, uint32_t item)
{
   buffer.push_back((item >> 24) & 0xFF);
   buffer.push_back((item >> 16) & 0xFF);
   buffer.push_back((item >> 8) & 0xFF);
   buffer.push_back((item) & 0xFF);
}

static void serialize(std::vector<uint8_t>& buffer, int8_t item)
{
   serialize(buffer, (uint8_t)item);
}

static void serialize(std::vector<uint8_t>& buffer, int16_t item)
{
   serialize(buffer, (uint16_t)item);
}

static void serialize(std::vector<uint8_t>& buffer, int32_t item)
{
   serialize(buffer, (uint32_t)item);
}

static void serialize(std::vector<uint8_t>& buffer, const std::string& item)
{
   serialize(buffer, (uint32_t)item.size());
   for (const char& c : item)
   {
      serialize(buffer, (uint8_t)c);
   }
}


template<typename ITEM_TYPE>
static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, ITEM_TYPE& item)
{
   printf("DESERIALIZE ERROR!\n");
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, bool& item)
{
   item = (bool)buffer[offset++];
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, uint8_t& item)
{
   item = buffer[offset++];
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, uint16_t& item)
{
   item = (buffer[offset++] << 8);
   item |= buffer[offset++];
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, uint32_t& item)
{
   item = (buffer[offset++] << 24);
   item |= (buffer[offset++] << 16);
   item |= (buffer[offset++] << 8);
   item |= buffer[offset++];
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, int8_t& item)
{
   deserialize(buffer, offset, (uint8_t&)item);
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, int16_t& item)
{
   deserialize(buffer, offset, (uint16_t&)item);
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, int32_t& item)
{
   deserialize(buffer, offset, (uint32_t&)item);
}

static void deserialize(const std::vector<uint8_t>& buffer, uint32_t& offset, std::string& item)
{
   item.clear();
   uint32_t string_length = 0;
   char c;
   deserialize(buffer, offset, string_length);
   for (uint32_t i = 0; i < string_length; i++)
   {
      deserialize(buffer, offset, (uint8_t&)c);
      item += c;
   }
}


#endif
