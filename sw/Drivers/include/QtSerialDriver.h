#pragma once

#include <mutex>

#include "ISerialDriver.h"
#include "ThreadWorker.h"
#include "QtSerialPort/QSerialPort"


namespace Drivers
{
namespace Serial
{


class QtSerialDriver : public ISerialDriver
{
public:
   QtSerialDriver();
private:
   bool open(DataMode mode, const Settings& settings) override;
   void close() override;
   bool isOpened() override;
   void addListener(SerialListener* callback) override;
   void removeListener(SerialListener* callback) override;
   bool write(const std::vector<uint8_t>& data, ssize_t size = 0) override;

   void receivingThread();
   void notifyListeners(DriverEvent ev, const std::vector<uint8_t>& data, size_t size);
   Utilities::ThreadWorker m_worker;
   std::vector<uint8_t> m_recv_buffer;
   uint32_t m_recv_buffer_idx;
   DataMode m_mode;
   std::vector<SerialListener*> m_listeners;
   std::mutex m_listeners_mutex;

   QSerialPort m_serial_port;
};


}
}