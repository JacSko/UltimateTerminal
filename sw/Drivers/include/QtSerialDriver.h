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
   ~QtSerialDriver();
private:

   enum class State
   {
      IDLE,
      CONNECTION_REQUESTED,
      CONNECTED,
      DISCONNECTING,
      DESTROYING,
   };

   bool open(DataMode mode, const Settings& settings) override;
   void close() override;
   bool isOpened() override;
   void addListener(SerialListener* callback) override;
   void removeListener(SerialListener* callback) override;
   bool write(const std::vector<uint8_t>& data, ssize_t size = 0) override;

   void receivingThread();
   void writePendingData();
   void notifyListeners(DriverEvent ev, const std::vector<uint8_t>& data, size_t size);
   Utilities::ThreadWorker m_worker;
   std::vector<uint8_t> m_recv_buffer;
   uint32_t m_recv_buffer_idx;
   DataMode m_mode;
   std::vector<SerialListener*> m_listeners;
   std::mutex m_listeners_mutex;
   std::mutex m_write_buffer_mutex;
   std::condition_variable m_write_buffer_condvar;
   std::vector<uint8_t> m_write_buffer;

   QSerialPort* m_serial_port;
   Settings m_settings;
   std::condition_variable m_cond_var;
   std::mutex m_mutex;
   State m_state;
};


}
}
