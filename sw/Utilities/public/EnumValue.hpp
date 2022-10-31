#pragma once

#include <string>

template<typename T>
class EnumValue
{
public:
   EnumValue(T val):
   value(val)
   {}
   EnumValue(const std::string& name):
   value(fromName(name))
   {}
   std::string toName() const;
   T fromName(const std::string& name);
   T value;
   bool operator==(const EnumValue<T>& lhs) const
   {
      return value == lhs.value;
   }
   bool operator!=(const EnumValue<T>& lhs) const
   {
      return value != lhs.value;
   }
};
