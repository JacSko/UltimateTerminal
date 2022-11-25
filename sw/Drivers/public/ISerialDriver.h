#ifndef _ISERIAL_DRIVER_H_
#define _ISERIAL_DRIVER_H_

/* =============================
 *   Includes of common headers
 * =============================*/
#include "stdint.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "EnumValue.hpp"

/**
 * @file ISerialDriver.h
 *
 * @brief
 *    Interface for serial port communication.
 *
 * @details
 *
 * @author Jacek Skowronek
 * @date   08/04/2022
 *
 */

namespace Drivers
{
namespace Serial
{

enum class DriverEvent
{
   DATA_RECV,       /**< New data received on serial port */
};

enum class DataMode
{
   NEW_LINE_DELIMITER,  /**< Client waits for newline char, then notifies listeners about new data */
};

#define DEF_BAUD_RATES          \
   DEF_BAUD_RATE(BR_1200)       \
   DEF_BAUD_RATE(BR_2400)       \
   DEF_BAUD_RATE(BR_4800)       \
   DEF_BAUD_RATE(BR_9600)       \
   DEF_BAUD_RATE(BR_19200)      \
   DEF_BAUD_RATE(BR_38400)      \
   DEF_BAUD_RATE(BR_57600)      \
   DEF_BAUD_RATE(BR_115200)     \

#define DEF_DATA_BITS      \
   DEF_DATA_BIT(FIVE)      \
   DEF_DATA_BIT(SIX)       \
   DEF_DATA_BIT(SEVEN)     \
   DEF_DATA_BIT(EIGHT)     \

#define DEF_PARITY_BITS    \
   DEF_PARITY_BIT(NONE)    \
   DEF_PARITY_BIT(EVEN)    \
   DEF_PARITY_BIT(ODD)     \

#define DEF_STOP_BITS      \
   DEF_STOP_BIT(ONE)       \
   DEF_STOP_BIT(TWO)       \

#undef DEF_BAUD_RATE
#define DEF_BAUD_RATE(name) name,
enum class BaudRate
{
   DEF_BAUD_RATES
   BAUDRATE_MAX
};
#undef DEF_BAUD_RATE

#undef DEF_PARITY_BIT
#define DEF_PARITY_BIT(name) name,
enum class ParityType
{
   DEF_PARITY_BITS
   PARITY_BIT_MAX
};
#undef DEF_PARITY_BIT

#undef DEF_STOP_BIT
#define DEF_STOP_BIT(name) name,
enum class StopBitType
{
   DEF_STOP_BITS
   STOP_BIT_MAX
};
#undef DEF_STOP_BIT

#define DEF_DATA_BIT(name) name,
enum class DataBitType
{
   DEF_DATA_BITS
   DATA_BIT_MAX
};
#undef DEF_DATA_BIT

}
}

template<>
std::string EnumValue<Drivers::Serial::BaudRate>::toName() const;
template<>
std::string EnumValue<Drivers::Serial::ParityType>::toName() const;
template<>
std::string EnumValue<Drivers::Serial::StopBitType>::toName() const;
template<>
std::string EnumValue<Drivers::Serial::DataBitType>::toName() const;
template<>
Drivers::Serial::BaudRate EnumValue<Drivers::Serial::BaudRate>::fromName(const std::string& name);
template<>
Drivers::Serial::ParityType EnumValue<Drivers::Serial::ParityType>::fromName(const std::string& name);
template<>
Drivers::Serial::StopBitType EnumValue<Drivers::Serial::StopBitType>::fromName(const std::string& name);
template<>
Drivers::Serial::DataBitType EnumValue<Drivers::Serial::DataBitType>::fromName(const std::string& name);

namespace Drivers
{
namespace Serial
{

struct Settings
{
   Settings():
   device("/dev/ttyUSB1"),
   baudRate(BaudRate::BR_115200),
   mode(DataMode::NEW_LINE_DELIMITER),
   parityBits(ParityType::NONE),
   stopBits(StopBitType::ONE),
   dataBits(DataBitType::EIGHT)
   {

   }
   Settings(const std::string& device_path,
            Drivers::Serial::DataMode mode,
            Drivers::Serial::BaudRate baud,
            Drivers::Serial::ParityType paritybits,
            Drivers::Serial::StopBitType stopbits,
            Drivers::Serial::DataBitType databits):
   device(device_path),
   baudRate(baud),
   mode(mode),
   parityBits(paritybits),
   stopBits(stopbits),
   dataBits(databits)
   {

   }
   std::string device;
   EnumValue<BaudRate> baudRate;
   DataMode mode;
   EnumValue<ParityType> parityBits;
   EnumValue<StopBitType> stopBits;
   EnumValue<DataBitType> dataBits;

   bool operator==(const Settings& lhs)
   {
      return (device == lhs.device)        &&
             (baudRate == lhs.baudRate)    &&
             (mode == lhs.mode)            &&
             (parityBits == lhs.parityBits)&&
             (stopBits == lhs.stopBits)    &&
             (dataBits == lhs.dataBits);
   }
};

class SerialListener
{
public:
   virtual ~SerialListener(){};
   /**
    * @brief Callback called on new event.
    * @param[in] ev - Event received from server
    * @param[in] data - Reference to vector with new data, can be empty, depends on event
    * @param[in] size - Size of the data, can be 0, depends on event.
    * @return None.
    */
   virtual void onSerialEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t size) = 0;
};

class ISerialDriver
{
public:
   static std::unique_ptr<ISerialDriver> create();
   /**
    * @brief Opens a serial port.
    * @param[in] mode - server mode
    * @param[in] device - path to device
    * @return true if connected successfully, otherwise false.
    */
   virtual bool open(DataMode mode, const Settings& settings) = 0;
   /**
    * @brief Disconnects client from server.
    * @return void.
    */
   virtual void close() = 0;
   /**
    * @brief Returns information if port is opened.
    * @return True if opened.
    */
   virtual bool isOpened() = 0;
   /**
    * @brief Adds listener to receive notifications about driver status or data received.
    * @return None.
    */
   virtual void addListener(SerialListener* callback) = 0;
   /**
    * @brief Removes listener.
    * @return None.
    */
   virtual void removeListener(SerialListener* callback) = 0;
   /**
    * @brief Writes data to serial port.
    * @param[in] data - bytes to write.
    * @param[in] size - number of bytes to write - if not provided, data.size() bytes will be written.
    */
   virtual bool write(const std::vector<uint8_t>& data, ssize_t size = 0) = 0;
   virtual ~ISerialDriver(){};
};

}
}

#endif
