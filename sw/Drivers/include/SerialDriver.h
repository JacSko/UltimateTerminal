#pragma once

#include <mutex>
#include <atomic>

#include "ISerialDriver.h"
#include "ThreadWorker.h"

namespace Drivers
{
namespace Serial
{


class SerialDriver : public ISerialDriver
{
public:
   SerialDriver();
   ~SerialDriver();
private:
   enum class State
   {
      IDLE,
      CONNECTING,
      CONNECTED,
      DISCONNECTING,
      CLOSING,
   };

   void extracted();
   bool open(DataMode mode, const Settings &settings) override;
   void close() override;
   bool isOpened() override;
   void addListener(SerialListener* callback) override;
   void removeListener(SerialListener* callback) override;
   bool write(const std::vector<uint8_t>& data, ssize_t size = 0) override;

   void closeDescriptor();
   void receivingThread();
   void startReadingLoop();
   void notifyListeners(DriverEvent ev, const std::vector<uint8_t>& data, size_t size);
   void setCommonValues(struct termios& termios);
   void setBaudrates(BaudRate, struct termios& termios);
   void setDataBits(DataBitType, struct termios& termios);
   void setParityBits(ParityType, struct termios& termios);
   void setStopBits(StopBitType, struct termios& termios);
   Utilities::ThreadWorker m_worker;
   std::vector<uint8_t> m_buffer;
   uint32_t m_bufferIndex;
   DataMode m_mode;
   int m_fd;
   std::vector<SerialListener*> m_listeners;
   std::mutex m_listeners_mutex;
   std::atomic<State> m_state;
   std::mutex m_mutex;
   std::condition_variable m_conditionVariable;
};


}
}
