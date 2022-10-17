#pragma once

#include <mutex>

#include "ISerialDriver.h"
namespace Drivers
{
namespace Serial
{


class SerialDriver : public ISerialDriver
{
public:
private:
   bool open(DataMode mode, std::string device) override;
   void close() override;
   bool isOpened() override;
   void addListener(SerialListener* callback) override;
   void removeListener(SerialListener* callback) override;
   bool write(const std::vector<uint8_t>& data, size_t size = 0) override;

   DataMode m_mode;
   int m_fd;
   std::vector<SerialListener*> m_listeners;
   std::mutex m_listeners_mutex;
};


}
}
