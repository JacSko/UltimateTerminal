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

enum class ParityType
{
   NONE,
   EVEN,
   ODD
};

enum class StopBitType
{
   ONE = 1,
   TWO
};

enum class DataBitType
{
   _5BITS = 5,
   _6BITS,
   _7BITS,
   _8BITS
};

struct Settings
{
   std::string device;
   uint32_t baudRate;
   DataMode mode;
   ParityType parityBits;
   StopBitType stopBits;
   DataBitType dataBits;
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
