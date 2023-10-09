#pragma once
#include <vector>
#include <GenericListener.hpp>


namespace Utilities
{

namespace Persistence
{

class PersistenceListener
{
public:

   struct PersistenceItem
   {
      std::string key;
      std::string value;

      bool operator== (const PersistenceItem& lhs)
      {
         return (lhs.key == key) && (lhs.value == value);
      }
   };

   typedef std::vector<PersistenceItem> PersistenceItems;

   PersistenceListener(){};
   virtual ~PersistenceListener(){};

   virtual void onPersistenceRead(const PersistenceItems&) = 0;
   virtual void onPersistenceWrite(PersistenceItems&) = 0;
   const std::string& getName() {return m_name;};
   void setName(const std::string& name){ m_name = name;};

private:
   std::string m_name;
};

class Persistence : public GenericListener<PersistenceListener>
{
public:
   bool loadFile(const std::string& file_name);
   void restore();
   bool save(const std::string& file_name);
   void restoreModule(PersistenceListener& listener);
   void clearFile();
};

template<typename T>
inline void writeItem(PersistenceListener::PersistenceItems& items, const std::string& key, const T& value)
{
   items.push_back(PersistenceListener::PersistenceItem{key, std::to_string(value)});
}
template<>
inline void writeItem<std::string>(PersistenceListener::PersistenceItems& items, const std::string& key, const std::string& value)
{
   items.push_back(PersistenceListener::PersistenceItem{key, value});
}
template<typename T>
inline bool readItem(const PersistenceListener::PersistenceItems& items, const std::string& key, T& result)
{
   for (const auto& item : items)
   {
      if (item.key == key)
      {
         result = static_cast<T>(std::stoll(item.value));
         return true;
      }
   }
   return false;
}
template<>
inline bool readItem(const PersistenceListener::PersistenceItems& items, const std::string& key, std::string& result)
{
   for (const auto& item : items)
   {
      if (item.key == key)
      {
         result = item.value;
         return true;
      }
   }
   return false;
}
}

}
