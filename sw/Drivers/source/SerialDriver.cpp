#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <map>
#include "SerialDriver.h"


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

static const std::map<BaudRate, int> g_baudrates_map = {{BaudRate::BR_1200, B1200},
                                                        {BaudRate::BR_2400, B2400},
                                                        {BaudRate::BR_4800, B4800},
                                                        {BaudRate::BR_9600, B9600},
                                                        {BaudRate::BR_19200, B19200},
                                                        {BaudRate::BR_38400, B38400},
                                                        {BaudRate::BR_57600, B57600},
                                                        {BaudRate::BR_115200, B115200}};

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
         setCommonValues(tty);
         setBaudrates(settings.baudRate.value, tty);
         setDataBits(settings.dataBits.value, tty);
         setParityBits(settings.parityBits.value, tty);
         setStopBits(settings.stopBits.value, tty);

         if (tcsetattr(m_fd, TCSANOW, &tty) == 0)
         {
            result = true;
            m_worker.start(SERIAL_THREAD_START_TIMEOUT);
         }
      }
   }

   return result;
}
void SerialDriver::setCommonValues(struct termios& tty)
{
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
   tty.c_cc[VMIN] = 0;
   tty.c_cc[VTIME] = 5; /* block for max 500ms*/
}
void SerialDriver::setBaudrates(BaudRate baudrate, struct termios& tty)
{
   cfsetispeed(&tty, g_baudrates_map.at(baudrate));
   cfsetospeed(&tty, g_baudrates_map.at(baudrate));
}
void SerialDriver::setDataBits(DataBitType databits, struct termios& tty)
{
   switch(databits)
   {
   case DataBitType::FIVE:
      tty.c_cflag |= CS5;
      break;
   case DataBitType::SIX:
      tty.c_cflag |= CS6;
      break;
   case DataBitType::SEVEN:
      tty.c_cflag |= CS7;
      break;
   case DataBitType::EIGHT:
      tty.c_cflag |= CS8;
      break;
   default:
      break;
   }
}
void SerialDriver::setParityBits(ParityType parity, struct termios& tty)
{
   switch(parity)
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
}
void SerialDriver::setStopBits(StopBitType stopbits, struct termios& tty)
{
   switch(stopbits)
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
}

void SerialDriver::close()
{
   if (isOpened())
   {
      ::close(m_fd);
      m_fd = -1;
      m_worker.stop();
   }
}
bool SerialDriver::isOpened()
{
   return m_worker.isRunning();
}
void SerialDriver::receivingThread()
{
   while(m_worker.isRunning())
   {
      int recv_bytes = read(m_fd, m_recv_buffer.data() + m_recv_buffer_idx, m_recv_buffer.size() - m_recv_buffer_idx);
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
      else if(recv_bytes == -1)
      {
         break;
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
   for (auto& listener : m_listeners)
   {
      if (listener)
      {
         listener->onSerialEvent(ev, data, size);
      }
   }
}
bool SerialDriver::write(const std::vector<uint8_t>& data, ssize_t size)
{
   return ::write(m_fd, data.data(), data.size()) == size;
}

}
}
