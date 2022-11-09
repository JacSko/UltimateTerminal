#include <sstream>
#include <fstream>
#include <numeric>
#include "PersistenceHandler.h"
#include "Serialize.hpp"
#include "Logger.h"

namespace Persistence
{

/* size of each persistence block is stored on 4 bytes */
constexpr uint8_t PERSISTENCE_DATA_BLOCK_SIZE = 4;
/* persistence checksum size */
constexpr uint8_t PERSISTENCE_CHECKSUM_SIZE = 4;

bool PersistenceHandler::restore(const std::string& file_name)
{
   bool result = false;
   std::ifstream file(file_name, std::ios::binary);

   if (file)
   {
      uint32_t file_size = getFileSize(file);
      if (file_size)
      {
         std::vector<uint8_t> buffer;
         buffer.resize(file_size);
         file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
         if (validateChecksum(buffer))
         {
            auto it = buffer.begin();
            while (it != (buffer.end() - PERSISTENCE_CHECKSUM_SIZE))
            {
               /* get block name */
               std::string name ((char*)&(*it));
               it += name.size() + 1;
               /* get block size */
               uint32_t offset = std::distance(buffer.begin(), it);
               uint32_t data_size = 0;
               ::deserialize(buffer, offset, data_size);
               it += PERSISTENCE_DATA_BLOCK_SIZE;
               /* get block data */
               std::vector<uint8_t> data (it, it + data_size);
               it += data_size;
               UT_Log(PERSISTENCE, HIGH, "got %u bytes for %s", data.size(), name.c_str());
               GenericListener::notifyChange([&](PersistenceListener* l)
                     {
                        if (l->getName() == name)
                        {
                           l->onPersistenceRead(data);
                        }
                     });
            }
            result = true;
         }
      }
   }
   return result;
}
bool PersistenceHandler::save(const std::string& file_name)
{
   bool result = false;
   std::ofstream file (file_name, std::ios::binary);
   if (file)
   {
      std::vector<uint8_t> out_buffer;
      std::vector<uint8_t> temp_buffer;
      GenericListener::notifyChange([&](PersistenceListener* l)
            {
               temp_buffer.clear();
               l->onPersistenceWrite(temp_buffer);
               /* write block name */
               std::string name = l->getName();
               UT_Log(PERSISTENCE, HIGH, "got %u bytes from %s", temp_buffer.size(), name.c_str());
               out_buffer.insert(out_buffer.end(), name.begin(), name.end());
               /* put NULL char to indicate string end */
               out_buffer.push_back(0x00);
               /* put block size on 4 bytes */
               ::serialize(out_buffer, (uint32_t)temp_buffer.size());
               /* insert serialized block data */
               out_buffer.insert(out_buffer.end(), temp_buffer.begin(), temp_buffer.end());
               UT_Log(PERSISTENCE, HIGH, "Persistence file size %u", out_buffer.size());
            });
      addChecksum(out_buffer);
      file.write(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size());
      file.flush();
      file.close();
      result = true;
   }
   return result;
}
uint32_t PersistenceHandler::getFileSize(std::istream& file)
{
   file.seekg(0, std::ios::end);
   uint32_t result = file.tellg();
   file.seekg(0, std::ios::beg);
   UT_Log(PERSISTENCE, LOW, "file size $u", result);
   return result;
}
void PersistenceHandler::addChecksum(std::vector<uint8_t>& data)
{
   uint32_t checksum = std::accumulate(data.begin(), data.end(), 0);
   UT_Log(MAIN, HIGH, "writing checksum %.4x", checksum);
   ::serialize(data, checksum);
}
bool PersistenceHandler::validateChecksum(const std::vector<uint8_t>& data)
{
   uint32_t offset = data.size() - PERSISTENCE_CHECKSUM_SIZE;
   uint32_t read_checksum = 0;
   uint32_t real_checksum = std::accumulate(data.begin(), data.end() - PERSISTENCE_CHECKSUM_SIZE, 0);
   ::deserialize(data, offset, read_checksum);
   UT_Log(PERSISTENCE, INFO, "checksum read %.4x, checksum calculated %.4x", read_checksum, real_checksum);
   return read_checksum == real_checksum;
}


}
