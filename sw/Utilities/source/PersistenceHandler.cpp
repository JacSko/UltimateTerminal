#include <sstream>
#include <fstream>
#include "PersistenceHandler.h"

namespace Persistence
{


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
         auto it = buffer.begin();
         while (it != buffer.end())
         {
            /* get block name */
            std::string name ((char*)&(*it));
            it += name.size() + 1;
            /* get block size */
            uint32_t data_size = decodeSize(it);
            it += PERSISTENCE_DATA_BLOCK_SIZE;
            /* get block data */
            std::vector<uint8_t> data (it, it + data_size);
            it += data_size;
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
               out_buffer.insert(out_buffer.end(), name.begin(), name.end());
               /* put NULL char to indicate string end */
               out_buffer.push_back(0x00);
               /* put block size on 4 bytes */
               encodeSize(out_buffer, (uint32_t)temp_buffer.size());
               /* insert serialized block data */
               out_buffer.insert(out_buffer.end(), temp_buffer.begin(), temp_buffer.end());
            });
      file.write(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size());
      file.flush();
      file.close();
      result = true;
   }
   return result;
}
uint32_t PersistenceHandler::decodeSize(std::vector<uint8_t>::iterator block)
{
   uint32_t result = 0;
   result = (0xFF000000 & (static_cast<uint32_t>(*(block+3) << 24))) |
            (0x00FF0000 & (static_cast<uint32_t>(*(block+2) << 16))) |
            (0x0000FF00 & (static_cast<uint32_t>(*(block+1) << 8))) |
            (0x000000FF & static_cast<uint32_t>(*(block)));
   return result;
}
void PersistenceHandler::encodeSize(std::vector<uint8_t>& buffer, uint32_t size)
{
   buffer.push_back(static_cast<uint8_t>(0x000000FF & size));
   buffer.push_back(static_cast<uint8_t>(0x0000FF00 & size));
   buffer.push_back(static_cast<uint8_t>(0x00FF0000 & size));
   buffer.push_back(static_cast<uint8_t>(0xFF000000 & size));
}
uint32_t PersistenceHandler::getFileSize(std::istream& file)
{
   file.seekg(0, std::ios::end);
   uint32_t result = file.tellg();
   file.seekg(0, std::ios::beg);
   return result;
}
}
