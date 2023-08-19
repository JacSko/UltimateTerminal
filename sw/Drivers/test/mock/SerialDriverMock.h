#pragma once
#include <gmock/gmock.h>
#include "ISerialDriver.h"


namespace Drivers
{
namespace Serial
{
constexpr uint32_t SERIAL_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t SERIAL_MAX_PAYLOAD_LENGTH = 4096;
constexpr char DATA_DELIMITER = '\n';

#undef DEF_BAUD_RATE
#define DEF_BAUD_RATE(a) #a,
const std::vector<std::string> g_baudrates_names = { DEF_BAUD_RATES };
#undef DEF_BAUD_RATE

#undef DEF_DATA_BIT
#define DEF_DATA_BIT(a) #a,
const std::vector<std::string> g_databits_names = { DEF_DATA_BITS };
#undef DEF_DATA_BIT

#undef DEF_PARITY_BIT
#define DEF_PARITY_BIT(a) #a,
const std::vector<std::string> g_paritybits_names = { DEF_PARITY_BITS };
#undef DEF_PARITY_BIT

#undef DEF_STOP_BIT
#define DEF_STOP_BIT(a) #a,
const std::vector<std::string> g_stopbits_names = { DEF_STOP_BITS };
#undef DEF_STOP_BIT
}
}

template<typename T>
std::string EnumValue<T>::toName() const
{
   return "";
}
template<>
std::string EnumValue<Drivers::Serial::BaudRate>::toName() const
{
   return Drivers::Serial::g_baudrates_names[(size_t)value];
}
template<>
std::string EnumValue<Drivers::Serial::ParityType>::toName() const
{
   return Drivers::Serial::g_paritybits_names[(size_t)value];
}
template<>
std::string EnumValue<Drivers::Serial::StopBitType>::toName() const
{
   return Drivers::Serial::g_stopbits_names[(size_t)value];
}
template<>
std::string EnumValue<Drivers::Serial::DataBitType>::toName() const
{
   return Drivers::Serial::g_databits_names[(size_t)value];
}

template<typename T>
T EnumValue<T>::fromName(const std::string& name)
{
   return {};
}
template<>
Drivers::Serial::BaudRate EnumValue<Drivers::Serial::BaudRate>::fromName(const std::string& name)
{
   value = Drivers::Serial::BaudRate::BAUDRATE_MAX;
   auto it = std::find(Drivers::Serial::g_baudrates_names.begin(), Drivers::Serial::g_baudrates_names.end(), name);
   if (it != Drivers::Serial::g_baudrates_names.end())
   {
      value = (Drivers::Serial::BaudRate)(std::distance(Drivers::Serial::g_baudrates_names.begin(), it));
   }
   return value;
}
template<>
Drivers::Serial::ParityType EnumValue<Drivers::Serial::ParityType>::fromName(const std::string& name)
{
   value = Drivers::Serial::ParityType::PARITY_BIT_MAX;
   auto it = std::find(Drivers::Serial::g_paritybits_names.begin(), Drivers::Serial::g_paritybits_names.end(), name);
   if (it != Drivers::Serial::g_paritybits_names.end())
   {
      value = (Drivers::Serial::ParityType)(std::distance(Drivers::Serial::g_paritybits_names.begin(), it));
   }
   return value;
}
template<>
Drivers::Serial::StopBitType EnumValue<Drivers::Serial::StopBitType>::fromName(const std::string& name)
{
   value = Drivers::Serial::StopBitType::STOP_BIT_MAX;
   auto it = std::find(Drivers::Serial::g_stopbits_names.begin(), Drivers::Serial::g_stopbits_names.end(), name);
   if (it != Drivers::Serial::g_stopbits_names.end())
   {
      value = (Drivers::Serial::StopBitType)(std::distance(Drivers::Serial::g_stopbits_names.begin(), it));
   }
   return value;
}
template<>
Drivers::Serial::DataBitType EnumValue<Drivers::Serial::DataBitType>::fromName(const std::string& name)
{
   value = Drivers::Serial::DataBitType::DATA_BIT_MAX;
   auto it = std::find(Drivers::Serial::g_databits_names.begin(), Drivers::Serial::g_databits_names.end(), name);
   if (it != Drivers::Serial::g_databits_names.end())
   {
      value =  (Drivers::Serial::DataBitType)(std::distance(Drivers::Serial::g_databits_names.begin(), it));
   }
   return value;
}


namespace Drivers
{
namespace Serial
{

struct ISerialDriverMock : public ISerialDriver
{
   MOCK_METHOD2(open, bool(DataMode, const Settings&));
   MOCK_METHOD0(close, void());
   MOCK_METHOD0(isOpened, bool());
   MOCK_METHOD1(addListener, void(SerialListener*));
   MOCK_METHOD1(removeListener, void(SerialListener*));
   MOCK_METHOD2(write, bool(const std::vector<uint8_t>&, ssize_t));
};

}
}
