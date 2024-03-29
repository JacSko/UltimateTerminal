#include <map>
#include "QtSerialDriver.h"

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
T Utilities::EnumValue<T>::fromName(const std::string& name)
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

static const std::map<BaudRate, QSerialPort::BaudRate> g_baudrates_map = {{BaudRate::BR_1200, QSerialPort::BaudRate::Baud1200},
                                                                          {BaudRate::BR_2400, QSerialPort::BaudRate::Baud2400},
                                                                          {BaudRate::BR_4800, QSerialPort::BaudRate::Baud4800},
                                                                          {BaudRate::BR_9600, QSerialPort::BaudRate::Baud9600},
                                                                          {BaudRate::BR_19200, QSerialPort::BaudRate::Baud19200},
                                                                          {BaudRate::BR_38400, QSerialPort::BaudRate::Baud38400},
                                                                          {BaudRate::BR_57600, QSerialPort::BaudRate::Baud57600},
                                                                          {BaudRate::BR_115200, QSerialPort::BaudRate::Baud115200}};
static const std::map<DataBitType, QSerialPort::DataBits> g_databits_map = {{DataBitType::FIVE, QSerialPort::DataBits::Data5},
                                                                            {DataBitType::SIX, QSerialPort::DataBits::Data6},
                                                                            {DataBitType::SEVEN, QSerialPort::DataBits::Data7},
                                                                            {DataBitType::EIGHT, QSerialPort::DataBits::Data8}};
static const std::map<ParityType, QSerialPort::Parity> g_paritytype_map = {{ParityType::NONE, QSerialPort::Parity::NoParity},
                                                                           {ParityType::EVEN, QSerialPort::Parity::EvenParity},
                                                                           {ParityType::ODD, QSerialPort::Parity::OddParity}};
static const std::map<StopBitType, QSerialPort::StopBits> g_stopbits_map =  {{StopBitType::ONE, QSerialPort::StopBits::OneStop},
                                                                             {StopBitType::TWO, QSerialPort::StopBits::TwoStop}};

std::unique_ptr<ISerialDriver> ISerialDriver::create()
{
   return std::unique_ptr<ISerialDriver>(new QtSerialDriver());
}

QtSerialDriver::QtSerialDriver():
m_worker(std::bind(&QtSerialDriver::receivingThread, this), "SERIAL_WORKER"),
m_recv_buffer(SERIAL_MAX_PAYLOAD_LENGTH, 0x00),
m_recv_buffer_idx(0),
m_state(State::IDLE)
{
   m_worker.start(SERIAL_THREAD_START_TIMEOUT);
}
QtSerialDriver::~QtSerialDriver()
{
   close();
   {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::DESTROYING;
   }
   m_cond_var.notify_all();
   m_worker.stop();
}
bool QtSerialDriver::open(DataMode mode, const Settings& settings)
{
   bool result = false;

   std::unique_lock<std::mutex> lock(m_mutex);
   if (m_state == State::IDLE)
   {
      m_mode = mode;
      m_settings = settings;
      m_state = State::CONNECTION_REQUESTED;
      m_cond_var.notify_all();
      if (m_cond_var.wait_for(lock, std::chrono::milliseconds(SERIAL_THREAD_START_TIMEOUT), [&]()->bool{return (m_state == State::CONNECTED) || (m_state == State::IDLE);}))
      {
         result = (m_state == State::CONNECTED);
      }

   }
   return result;
}

void QtSerialDriver::close()
{
   std::unique_lock<std::mutex> lock(m_mutex);
   if(m_state == State::CONNECTED)
   {
      m_state = State::DISCONNECTING;
      m_cond_var.wait_for(lock, std::chrono::milliseconds(SERIAL_THREAD_START_TIMEOUT), [&]()->bool{return m_state == State::IDLE;});
   }
}
bool QtSerialDriver::isOpened()
{
   std::lock_guard<std::mutex> lock(m_mutex);
   return m_state == State::CONNECTED;
}
void QtSerialDriver::receivingThread()
{
   bool is_started = false;
   m_serial_port = new QSerialPort();

   while(m_worker.isRunning())
   {
      {
         std::unique_lock<std::mutex> lock(m_mutex);
         is_started = false;
         m_cond_var.wait(lock);
         if (m_state == State::CONNECTION_REQUESTED)
         {
            is_started = true;
         }
         else if (m_state == State::DESTROYING)
         {
            break;
         }
      }

      if (is_started)
      {
         if ((g_baudrates_map.find(m_settings.baudRate.value) != g_baudrates_map.end()) &&
               (g_databits_map.find(m_settings.dataBits.value) != g_databits_map.end()) &&
               (g_paritytype_map.find(m_settings.parityBits.value) != g_paritytype_map.end()) &&
               (g_stopbits_map.find(m_settings.stopBits.value) != g_stopbits_map.end()))
         {
            m_serial_port->setBaudRate(g_baudrates_map.at(m_settings.baudRate.value));
            m_serial_port->setDataBits(g_databits_map.at(m_settings.dataBits.value));
            m_serial_port->setParity(g_paritytype_map.at(m_settings.parityBits.value));
            m_serial_port->setStopBits(g_stopbits_map.at(m_settings.stopBits.value));
            m_serial_port->setPortName(QString(m_settings.device.c_str()));

            if (m_serial_port->open(QIODevice::ReadWrite))
            {
               {
                  std::unique_lock<std::mutex> lock(m_mutex);
                  m_state = State::CONNECTED;
                  m_cond_var.notify_all();
               }
               while(m_worker.isRunning())
               {
                  writePendingData();
                  if (m_serial_port->waitForReadyRead(500))
                  {
                     int recv_bytes = m_serial_port->read((char*)m_recv_buffer.data() + m_recv_buffer_idx, SERIAL_MAX_PAYLOAD_LENGTH);
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
                  }
                  else
                  {
                     std::lock_guard<std::mutex> lock (m_mutex);
                     if (m_state == State::DISCONNECTING)
                     {
                        m_state = State::IDLE;
                        m_serial_port->close();
                        m_cond_var.notify_all();
                        break;
                     }
                  }
               }
            }
         }
      }
      std::unique_lock<std::mutex> lock(m_mutex);
      m_state = State::IDLE;
      m_cond_var.notify_all();
   }

   m_serial_port->close();
   delete m_serial_port;
}
void QtSerialDriver::writePendingData()
{
   std::lock_guard<std::mutex> lock(m_write_buffer_mutex);
   if (!m_write_buffer.empty())
   {
      size_t bytes_to_write = m_write_buffer.size();
      size_t bytes_written = 0;
      size_t current_write = 0;

      if (m_serial_port && bytes_to_write <= SERIAL_MAX_PAYLOAD_LENGTH)
      {
         while (bytes_to_write > 0)
         {
            current_write = m_serial_port->write((char*)m_write_buffer.data() + bytes_written, bytes_to_write);
            if (current_write > 0)
            {
               bytes_written += current_write;
            }
            else
            {
               break;
            }
            bytes_to_write -= bytes_written;
         }
      }
      if (m_write_buffer.size() == bytes_written)
      {
         m_write_buffer.clear();
         m_write_buffer_condvar.notify_all();
      }
   }
}
void QtSerialDriver::addListener(SerialListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   m_listeners.push_back(callback);
}
void QtSerialDriver::removeListener(SerialListener* callback)
{
   std::lock_guard<std::mutex> lock(m_listeners_mutex);
   auto it = std::find(m_listeners.begin(), m_listeners.end(), callback);
   if (it != m_listeners.end())
   {
      m_listeners.erase(it);
   }
}
void QtSerialDriver::notifyListeners(DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
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
bool QtSerialDriver::write(const std::vector<uint8_t>& data, ssize_t size)
{
   bool result = false;

   std::unique_lock<std::mutex> lock(m_write_buffer_mutex);
   m_write_buffer.clear();
   m_write_buffer.insert(m_write_buffer.end(), data.begin(), data.begin() + size);
   if (m_write_buffer_condvar.wait_for(lock, std::chrono::milliseconds(2000), [&](){return m_write_buffer.empty();}))
   {
      result = true;
   }
   return result;
}

}
}
