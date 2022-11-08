#pragma once
#include <vector>
#include <GenericListener.hpp>

namespace Persistence
{

class PersistenceListener
{
public:
   PersistenceListener(){};
   virtual ~PersistenceListener(){};

   virtual void onPersistenceRead(const std::vector<uint8_t>& data) = 0;
   virtual void onPersistenceWrite(std::vector<uint8_t>& data) = 0;
   const std::string& getName() {return m_name;};
   void setName(const std::string& name){ m_name = name;};
private:
   std::string m_name;
};

class PersistenceHandler : public GenericListener<PersistenceListener>
{
public:
   bool restore(const std::string& file_name);
   bool save(const std::string& file_name);
private:
   uint32_t getFileSize(std::istream& file);
   void addChecksum(std::vector<uint8_t>& data);
   bool validateChecksum(const std::vector<uint8_t>& data);
};



}
