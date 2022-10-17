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

bool SerialDriver::open(DataMode mode, std::string device)
{
   m_mode = mode;

   m_fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
   if (m_fd >= 0)
   {
      struct termios tty;
      if (tcgetattr(m_fd, &tty) >= 0)
      {
         cfsetospeed(&tty, B115200);
         cfsetispeed(&tty, B115200);

         tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
         tty.c_cflag &= ~CSIZE;
         tty.c_cflag |= CS8;         /* 8-bit characters */
         tty.c_cflag &= ~PARENB;     /* no parity bit */
         tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
         tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

         tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
         tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
         tty.c_oflag &= ~OPOST;

         /* fetch bytes as they become available */
         tty.c_cc[VMIN] = 1;
         tty.c_cc[VTIME] = 1;

         if (tcsetattr(m_fd, TCSANOW, &tty) == 0)
         {
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
      UT_Log(MAIN, LOW, "Cannot open %s, error %s(%d)", device.c_str(), strerror(errno), errno);
   }


}
void SerialDriver::close()
{
   ::close(m_fd);
   m_fd = -1;
}
bool SerialDriver::isOpened()
{

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
bool SerialDriver::write(const std::vector<uint8_t>& data, size_t size)
{
   return ::write(m_fd, data.data(), data.size()) == size;
}

}
}
