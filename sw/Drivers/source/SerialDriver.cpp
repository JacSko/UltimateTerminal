#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

#include "SerialDriver.h"
#include "Logger.h"


namespace Drivers
{
namespace Serial
{
constexpr uint32_t SERIAL_THREAD_START_TIMEOUT = 1000;
constexpr uint32_t SERIAL_MAX_PAYLOAD_LENGTH = 4096;
constexpr char DATA_DELIMITER = '\n';

std::unique_ptr<ISerialDriver> ISerialDriver::create()
{
   return std::unique_ptr<ISerialDriver>(new SerialDriver());
}

SerialDriver::SerialDriver():
m_worker(std::bind(&SerialDriver::receivingThread, this), "SERIAL_WORKER"),
m_recv_buffer(SERIAL_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0)
{

}

bool SerialDriver::open(DataMode mode, const Settings& settings)
{
   bool result = false;
   m_mode = mode;

   m_fd = ::open(settings.device.c_str(), O_RDWR);
   if (m_fd >= 0)
   {
      struct termios tty;
      if (tcgetattr(m_fd, &tty) >= 0)
      {
         tty.c_ispeed = settings.baudRate;
         tty.c_ospeed = settings.baudRate;
         tty.c_cflag &= ~CSIZE;
         tty.c_cflag &= ~CRTSCTS;
         tty.c_cflag |= CREAD | CLOCAL;
         tty.c_lflag &= ~ICANON;
         tty.c_lflag &= ~ECHO;
         tty.c_lflag &= ~ECHOE;
         tty.c_lflag &= ~ECHONL;
         tty.c_lflag &= ~ISIG;
         tty.c_iflag &= ~(IXON | IXOFF | IXANY);
         tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
         tty.c_oflag &= ~OPOST;
         tty.c_oflag &= ~ONLCR;
         tty.c_cc[VMIN] = 1;
         tty.c_cc[VTIME] = 1;

         /* set parity */
         switch(settings.parityBits)
         {
         case ParityType::NONE:
            tty.c_cflag &= ~PARENB;
            break;
         case ParityType::EVEN:
            tty.c_cflag |= PARENB;
            tty.c_cflag &= ~PARODD;
            break;
         case ParityType::ODD:
            tty.c_cflag |= PARENB;
            tty.c_cflag |= PARODD;
            break;
         default:
            break;
         }

         /* set stopbits */
         switch(settings.stopBits)
         {
         case StopBitType::ONE:
            tty.c_cflag &= ~CSTOPB;
            break;
         case StopBitType::TWO:
            tty.c_cflag |= CSTOPB;
            break;
         default:
            break;
         }

         /* set databits */
         switch(settings.dataBits)
         {
         case DataBitType::_5BITS:
            tty.c_cflag |= CS5;
            break;
         case DataBitType::_6BITS:
            tty.c_cflag |= CS6;
            break;
         case DataBitType::_7BITS:
            tty.c_cflag |= CS7;
            break;
         case DataBitType::_8BITS:
            tty.c_cflag |= CS8;
            break;
         default:
            break;
         }


         if (tcsetattr(m_fd, TCSANOW, &tty) == 0)
         {
            result = true;
            m_worker.start(SERIAL_THREAD_START_TIMEOUT);
            UT_Log(MAIN, LOW, "Successfully opened!");
         }
         else
         {
            UT_Log(MAIN, LOW, "Cannot tcsetattr(), error %s(%d)", strerror(errno), errno);
         }
      }
      else
      {
         UT_Log(MAIN, LOW, "Cannot tcgetattr(), error %s(%d)", strerror(errno), errno);
      }
   }
   else
   {
      UT_Log(MAIN, LOW, "Cannot open %s, error %s(%d)", settings.device.c_str(), strerror(errno), errno);
   }

   return result;
}
void SerialDriver::close()
{
   ::close(m_fd);
   m_fd = -1;
}
bool SerialDriver::isOpened()
{
   return m_worker.isRunning();
}
void SerialDriver::receivingThread()
{
   while(m_worker.isRunning())
   {
      UT_Log(MAIN, LOW, "waiting for data");
      int recv_bytes = read(m_fd, m_recv_buffer.data() + m_recv_buffer_idx, SERIAL_MAX_PAYLOAD_LENGTH);
      UT_Log(MAIN, LOW, "got %d bytes", recv_bytes);
      if (recv_bytes > 0)
      {
         m_recv_buffer_idx += recv_bytes;
         bool is_next_new_line = true;
         do
         {
            auto it = std::find(m_recv_buffer.begin(), (m_recv_buffer.begin() + m_recv_buffer_idx), (uint8_t)DATA_DELIMITER);
            if (it != (m_recv_buffer.begin() + m_recv_buffer_idx))
            {
               it++; //include the found newline too
               notifyListeners(DriverEvent::DATA_RECV, std::vector<uint8_t>(m_recv_buffer.begin(), it), std::distance(m_recv_buffer.begin(), it));
               std::copy(it, m_recv_buffer.begin() + m_recv_buffer_idx, m_recv_buffer.begin());
               m_recv_buffer_idx = std::distance(it, m_recv_buffer.begin() + m_recv_buffer_idx);
            }
            else
            {
               is_next_new_line = false;
            }
         }
         while(is_next_new_line);
      }
      else
      {
         if(recv_bytes == -1)
         {
            UT_Log(MAIN, LOW, "Cannot read(), error %s(%d)", strerror(errno), errno);
            return;
         }
      }
   }
}
void SerialDriver::addListener(SerialListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void SerialDriver::removeListener(SerialListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}
void SerialDriver::notifyListeners(DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   UT_Log_If(ev == DriverEvent::DATA_RECV, MAIN, LOW, "got data %s", data.data());
   for (auto& listener : m_listeners)
   {
      if (listener)
      {
         listener->onSerialEvent(ev, data, size);
      }
   }
}
bool SerialDriver::write(const std::vector<uint8_t>& data, size_t size)
{
   return ::write(m_fd, data.data(), data.size()) == size;
}

}
}
