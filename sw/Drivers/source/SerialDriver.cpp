#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <map>
#include "SerialDriver.h"
#include "Logger.h"

/* namespace wrapper around system function to allow replace in unit tests */
namespace system_call
{
__attribute__((weak)) int open(const char* file, int flags)
{
   return ::open(file, flags);
}
__attribute__((weak)) int tcgetattr(int fd, struct termios* termios)
{
   return ::tcgetattr(fd, termios);
}
__attribute__((weak)) int tcsetattr(int fd, int actions, const struct termios* termios)
{
   return ::tcsetattr(fd, actions, termios);
}
__attribute__((weak)) int close (int fd)
{
   return ::close(fd);
}
__attribute__((weak)) int cfsetispeed (struct termios* termios, speed_t speed)
{
   return ::cfsetispeed(termios, speed);
}
__attribute__((weak)) int cfsetospeed (struct termios* termios, speed_t speed)
{
   return ::cfsetospeed(termios, speed);
}
__attribute__((weak)) ssize_t read(int fd, void *buffer, size_t length)
{
   return ::read(fd, buffer, length);
}
__attribute__((weak)) ssize_t write(int fd, const void *buffer, size_t length)
{
   return ::write(fd, buffer, length);
}
}

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
std::string Utilities::EnumValue<T>::toName() const
{
   return "";
}
template<>
std::string Utilities::EnumValue<Drivers::Serial::BaudRate>::toName() const
{
   return Drivers::Serial::g_baudrates_names[(size_t)value];
}
template<>
std::string Utilities::EnumValue<Drivers::Serial::ParityType>::toName() const
{
   return Drivers::Serial::g_paritybits_names[(size_t)value];
}
template<>
std::string Utilities::EnumValue<Drivers::Serial::StopBitType>::toName() const
{
   return Drivers::Serial::g_stopbits_names[(size_t)value];
}
template<>
std::string Utilities::EnumValue<Drivers::Serial::DataBitType>::toName() const
{
   return Drivers::Serial::g_databits_names[(size_t)value];
}

template<typename T>
T Utilities::EnumValue<T>::fromName(const std::string&)
{
   return {};
}
template<>
Drivers::Serial::BaudRate Utilities::EnumValue<Drivers::Serial::BaudRate>::fromName(const std::string& name)
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
Drivers::Serial::ParityType Utilities::EnumValue<Drivers::Serial::ParityType>::fromName(const std::string& name)
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
Drivers::Serial::StopBitType Utilities::EnumValue<Drivers::Serial::StopBitType>::fromName(const std::string& name)
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
Drivers::Serial::DataBitType Utilities::EnumValue<Drivers::Serial::DataBitType>::fromName(const std::string& name)
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

const std::chrono::milliseconds COND_VAR_WAIT_MS = std::chrono::milliseconds(500);

SerialDriver::SerialDriver():
m_worker(std::bind(&SerialDriver::receivingThread, this), "SERIAL_WORKER"),
m_buffer(SERIAL_MAX_PAYLOAD_LENGTH, 0x00),
m_bufferIndex(0),
m_fd(-1),
m_state(State::IDLE)
{
   m_worker.start(SERIAL_THREAD_START_TIMEOUT);
}
SerialDriver::~SerialDriver()
{
   if (isOpened())
   {
      close();
   }
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::CLOSING;
      m_conditionVariable.notify_all();
   }
   if (m_worker.isRunning())
   {
      m_worker.stop();
   }
}
bool SerialDriver::open(DataMode mode, const Settings& settings)
{
   bool result = false;
   if (!isOpened())
   {
      m_mode = mode;
      UT_Log(SERIAL_DRV, LOW, "[%s] open - mode %u br %s sb %s pb %s db %s", settings.device.c_str(),
                                                                             static_cast<uint8_t>(mode),
                                                                             settings.baudRate.toName().c_str(),
                                                                             settings.stopBits.toName().c_str(),
                                                                             settings.parityBits.toName().c_str(),
                                                                             settings.dataBits.toName().c_str());
      m_fd = system_call::open(settings.device.c_str(), O_RDWR);
      if (m_fd >= 0)
      {
         UT_Log(SERIAL_DRV, LOW, "[%s] getting tc attributes", settings.device.c_str());
         struct termios tty;
         if (system_call::tcgetattr(m_fd, &tty) >= 0)
         {
            UT_Log(SERIAL_DRV, LOW, "[%s] setting connection settings", settings.device.c_str());
            setCommonValues(tty);
            setBaudrates(settings.baudRate.value, tty);
            setDataBits(settings.dataBits.value, tty);
            setParityBits(settings.parityBits.value, tty);
            setStopBits(settings.stopBits.value, tty);
            if (system_call::tcsetattr(m_fd, TCSANOW, &tty) == 0)
            {
               UT_Log(SERIAL_DRV, LOW, "[%s] starting receiving thread", settings.device.c_str());
               std::unique_lock<std::mutex> lock(m_mutex);
               m_state = State::CONNECTING;
               m_conditionVariable.notify_all();
               if (m_conditionVariable.wait_for(lock, COND_VAR_WAIT_MS, [&](){return m_state != State::CONNECTING;}))
               {
                  UT_Log(SERIAL_DRV, HIGH, "[%s] thread started", settings.device.c_str());
                  result = true;
               }
            }
            else
            {
               closeDescriptor();
               UT_Log(SERIAL_DRV, ERROR, "[%s] cannot set tc attributes!", settings.device.c_str());
            }
         }
         else
         {
            closeDescriptor();
            UT_Log(SERIAL_DRV, ERROR, "[%s] cannot get tc attributes!", settings.device.c_str());
         }
      }
   }
   else
   {
      UT_Log(SERIAL_DRV, ERROR, "already opened!");
   }
   return result;
}
void SerialDriver::closeDescriptor()
{
   system_call::close(m_fd);
   m_fd = -1;
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
   system_call::cfsetispeed(&tty, g_baudrates_map.at(baudrate));
   system_call::cfsetospeed(&tty, g_baudrates_map.at(baudrate));
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
   UT_Log(SERIAL_DRV, LOW, "closing port");
   if (isOpened())
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::DISCONNECTING;
      m_conditionVariable.notify_all();
      m_conditionVariable.wait_for(lock, COND_VAR_WAIT_MS, [&](){return m_state == State::IDLE;});
   }
   UT_Log(SERIAL_DRV, LOW, "close exit!");
}
bool SerialDriver::isOpened()
{
   return m_state == State::CONNECTING || m_state == State::CONNECTED;
}
void SerialDriver::receivingThread()
{
   UT_Log(SERIAL_DRV, LOW, "starting receiving thread");
   while(m_worker.isRunning())
   {
       {
         std::unique_lock<std::mutex> lock(m_mutex);
         UT_Log(SERIAL_DRV, HIGH, "thread paused");
         m_conditionVariable.wait(lock, [&](){return m_state != State::IDLE;});
         UT_Log(SERIAL_DRV, HIGH, "thread unlocked");
         if (m_state == State::CONNECTING)
         {
            m_state = State::CONNECTED;
            m_conditionVariable.notify_all();
         }
         else if (m_state == State::CLOSING)
         {
            break;
         }
      }
      if(m_state == State::CONNECTED)
      {
         startReadingLoop();
      }
      m_state = State::IDLE;
      closeDescriptor();
      m_conditionVariable.notify_all();
   }
   UT_Log(SERIAL_DRV, LOW, "closing receiving thread");
}
void SerialDriver::startReadingLoop()
{
   UT_Log(SERIAL_DRV, LOW, "%s", __func__);
   while(m_state == State::CONNECTED)
   {
      if (m_bufferIndex == m_buffer.size())
      {
         m_bufferIndex = 0;
      }
      int bytesReceived = system_call::read(m_fd, m_buffer.data() + m_bufferIndex,
                                               m_buffer.size() - m_bufferIndex);
      if (bytesReceived > 0)
      {
         m_bufferIndex += bytesReceived;
         while(1)
         {
            auto it = std::find(m_buffer.begin(),
                               (m_buffer.begin() + m_bufferIndex),
                               (uint8_t)DATA_DELIMITER);
            if (it != (m_buffer.begin() + m_bufferIndex))
            {
               it++; //include the found newline too
               notifyListeners(DriverEvent::DATA_RECV,
                               std::vector<uint8_t>(m_buffer.begin(), it),
                               std::distance(m_buffer.begin(), it));
               std::copy(it, m_buffer.begin() + m_bufferIndex, m_buffer.begin());
               m_bufferIndex = std::distance(it, m_buffer.begin() + m_bufferIndex);
            }
            else
            {
               break;
            }
         }
      }
      else if(bytesReceived == 0)
      {
         UT_Log(SERIAL_DRV, HIGH, "No bytes received, querying device");
         struct termios tty;
         if (system_call::tcgetattr(m_fd, &tty) != 0)
         {
            UT_Log(SERIAL_DRV, ERROR, "device disconnected!", bytesReceived);
            notifyListeners(DriverEvent::COMMUNICATION_ERROR, {}, 0);
            break;
         }
      }
      else
      {
         UT_Log(SERIAL_DRV, ERROR, "read returned %d, aborting", bytesReceived);
         notifyListeners(DriverEvent::COMMUNICATION_ERROR, {}, 0);
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
   UT_Log(SERIAL_DRV, HIGH, "writing %u bytes", size);
   return system_call::write(m_fd, data.data(), data.size()) == size;
}

}
}
