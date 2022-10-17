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
   DEF_BAUD_RATE(BR_50)         \
   DEF_BAUD_RATE(BR_75)         \
   DEF_BAUD_RATE(BR_110)        \
   DEF_BAUD_RATE(BR_134)        \
   DEF_BAUD_RATE(BR_150)        \
   DEF_BAUD_RATE(BR_200)        \
   DEF_BAUD_RATE(BR_300)        \
   DEF_BAUD_RATE(BR_600)        \
   DEF_BAUD_RATE(BR_1200)       \
   DEF_BAUD_RATE(BR_1800)       \
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

template<typename T>
class EnumValue
{
public:
   EnumValue(T val):
   value(val)
   {}
   std::string toName() const;
   T fromName(const std::string& name);
   T value;
};


struct Settings
{
   std::string device = "/dev/ttyUSB1";
   EnumValue<BaudRate> baudRate = BaudRate::BR_115200;
   DataMode mode = DataMode::NEW_LINE_DELIMITER;
   EnumValue<ParityType> parityBits = ParityType::NONE;
   EnumValue<StopBitType> stopBits = StopBitType::ONE;
   EnumValue<DataBitType> dataBits = DataBitType::EIGHT;
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
   virtual bool write(const std::vector<uint8_t>& data, size_t size = 0) = 0;
   virtual ~ISerialDriver(){};
};

}
}

#endif
