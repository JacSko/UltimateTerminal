#pragma once

#include <GenericListener.hpp>

namespace Persistence
{

class PersistenceListener
{
public:
   PersistenceListener(const std::string& name);
   virtual ~PersistenceListener(){};

   virtual void onPersistenceRead(const std::vector<uint8_t>& data) = 0;
   virtual void onPersistenceWrite(std::vector<uint8_t>& data) = 0;
   const std::string& getName() {return name;};
private:
   std::string name;
};

class PersistenceHandler : public GenericListener<PersistenceListener>
{
public:
   PersistenceHandler(const std::string& file):
   m_file(file)
   {

   }
   ~PersistenceHandler()
   {

   }
   bool restore()
   {
      std::ifstream file;
      file.open(m_file, std::ios::in);
      if (file)
      {

      }
   }
   bool save()
   {
      bool result = false;
      std::ofstream file;
      file.open(m_file, std::ios::out);
      if (file)
      {
         std::vector<uint8_t> buffer;
         GenericListener::notifyChange([&](PersistenceListener* l)
               {
                  /* write module name at the front of the data */
                  std::string name = l->getName();
                  buffer.insert(buffer.end(), name.begin(), name.end());
                  l->onPersistenceWrite(buffer);
               });
         file << buffer;
         file.flush();
         file.close();
         result = true;
      }
      return result;
   }

private:
   std::string m_file;

};



}
