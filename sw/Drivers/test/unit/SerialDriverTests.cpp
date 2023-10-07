#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <termios.h>
#include <fcntl.h>
#include <cstring>

#include "ISerialDriver.h"
/* ============================= */
/**
 * @file SerialDriverTests.cpp
 *
 * @brief Unit tests to verify behavior of SerialDriver.
 *
 * @author Jacek Skowronek
 * @date 08/10/2023
 */
/* ============================= */

using namespace testing;

struct SystemCallMock
{
   MOCK_METHOD2(open, int(const char*, int));
   MOCK_METHOD2(tcgetattr, int(int, struct termios*));
   MOCK_METHOD3(tcsetattr, int(int, int, const struct termios*));
   MOCK_METHOD1(close, int(int));
   MOCK_METHOD2(cfsetispeed, int(const struct termios*, speed_t));
   MOCK_METHOD2(cfsetospeed, int(const struct termios*, speed_t));
   MOCK_METHOD3(read, ssize_t(int, void *message, size_t length));
   MOCK_METHOD3(write, ssize_t(int, const void *message, size_t length));
};

struct SerialListenerMock : public Drivers::Serial::SerialListener
{
   MOCK_METHOD3(onSerialEvent, void(Drivers::Serial::DriverEvent, const std::vector<uint8_t>&, size_t));
};

SystemCallMock* sys_call_mock;

namespace system_call
{
__attribute__((weak)) int open(const char* file, int flags)
{
   return sys_call_mock->open(file, flags);
}
__attribute__((weak)) int tcgetattr(int fd, struct termios* termios)
{
   return sys_call_mock->tcgetattr(fd, termios);
}
__attribute__((weak)) int tcsetattr(int fd, int actions, const struct termios* termios)
{
   return sys_call_mock->tcsetattr(fd, actions, termios);
}
__attribute__((weak)) int close (int fd)
{
   return sys_call_mock->close(fd);
}
__attribute__((weak)) int cfsetispeed (struct termios* termios, speed_t speed)
{
   return sys_call_mock->cfsetispeed(termios, speed);
}
__attribute__((weak)) int cfsetospeed (struct termios* termios, speed_t speed)
{
   return sys_call_mock->cfsetospeed(termios, speed);
}
__attribute__((weak)) ssize_t read(int fd, void *buffer, size_t length)
{
   return sys_call_mock->read(fd, buffer, length);
}
__attribute__((weak)) ssize_t write(int fd, const void *buffer, size_t length)
{
   return sys_call_mock->write(fd, buffer, length);
}

}

namespace Drivers
{
namespace Serial
{

struct SerialDriverFixture : public testing::Test
{
   void SetUp()
   {
      sys_call_mock = new SystemCallMock;
      m_test_subject = ISerialDriver::create();
      m_test_subject->addListener(&listener);
      event_ready = false;
   }
   void TearDown()
   {
      m_test_subject->removeListener(&listener);
      m_test_subject.reset(nullptr);
      delete sys_call_mock;
   }
   void executePortOpenSequence()
   {
      EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
      EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(0));
      EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
      EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
      EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
      EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
      EXPECT_TRUE(m_test_subject->isOpened());
      Mock::VerifyAndClearExpectations(sys_call_mock);
   }
   void waitForEvent()
   {
      std::unique_lock<std::mutex> lock(mutex);
      cond_var.wait_for(lock, std::chrono::seconds(30), [&](){return event_ready;});
      event_ready = false;
   }
   void notifyEvent()
   {
      std::lock_guard<std::mutex> lock(mutex);
      event_ready = true;
      cond_var.notify_all();
   }
   std::unique_ptr<ISerialDriver> m_test_subject;
   SerialListenerMock listener;
   Settings serial_settings = {};
   int test_fd = 1;
   std::condition_variable cond_var;
   std::mutex mutex;
   bool event_ready;
};

TEST_F(SerialDriverFixture, closingPortWhenNotOpened_closeShallFinishedWithoutError)
{
   EXPECT_FALSE(m_test_subject->isOpened());
   m_test_subject->close();
   EXPECT_FALSE(m_test_subject->isOpened());
}
TEST_F(SerialDriverFixture, cannotCannotBeOpened_falseReturned)
{
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(-1));
   EXPECT_FALSE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_FALSE(m_test_subject->isOpened());
}
TEST_F(SerialDriverFixture, cannotGetTermiosAttributes_portCannotBeOpened)
{
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(-1));
   EXPECT_FALSE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_FALSE(m_test_subject->isOpened());
}
TEST_F(SerialDriverFixture, cannotSetTermiosAttributes_portCannotBeOpened)
{
   serial_settings.baudRate.value = BaudRate::BR_9600;
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, B9600)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, B9600)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(-1));
   EXPECT_FALSE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_FALSE(m_test_subject->isOpened());
}
TEST_F(SerialDriverFixture, portOpenedCorrectly_trueReturned)
{
   serial_settings.baudRate.value = BaudRate::BR_9600;
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, B9600)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, B9600)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, read(test_fd,_,_)).WillRepeatedly(Return(1));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   EXPECT_CALL(*sys_call_mock, close(test_fd));
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, noDataIncoming_periodicDeviceCheckExecuted)
{
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(0))
                                                     .WillRepeatedly(Invoke([&]()->int
            {
               notifyEvent();
               return 0;
            }));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   waitForEvent();

   EXPECT_CALL(*sys_call_mock, close(test_fd));
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, deviceDisconnected_ListenerNotified)
{
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillOnce(Return(0))
                                                     .WillRepeatedly(Invoke([&]()->int
            {
               notifyEvent();
               return -1;
            }));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::COMMUNICATION_ERROR,_,_));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   waitForEvent();

   m_test_subject->close();
}
TEST_F(SerialDriverFixture, readError_ListenerNotified)
{
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillOnce(Invoke([&]()
            {
               notifyEvent();
               return -1;
            }));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::COMMUNICATION_ERROR,_,_));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   waitForEvent();

   m_test_subject->close();
}
TEST_F(SerialDriverFixture, writeDataError_falseReturned)
{
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   const std::vector<uint8_t> data_to_send = {0,1,2,3,4,5,6,7,8,9};
   EXPECT_CALL(*sys_call_mock, write(test_fd, data_to_send.data(), data_to_send.size())).WillOnce(Return(0));
   EXPECT_FALSE(m_test_subject->write(data_to_send, data_to_send.size()));
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, writeDataSuccess_trueReturned)
{
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());

   const std::vector<uint8_t> data_to_send = {0,1,2,3,4,5,6,7,8,9};
   EXPECT_CALL(*sys_call_mock, write(test_fd, data_to_send.data(), data_to_send.size()))
                                                      .WillOnce(Return(data_to_send.size()));
   EXPECT_TRUE(m_test_subject->write(data_to_send, data_to_send.size()));
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, oneLineReceived_listenerNotified)
{
   const std::vector<uint8_t> data_received = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', '\n'};
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillOnce(
                                          Invoke([&](int, void* buffer, ssize_t)->int
                                             {
                                                std::memcpy(buffer, data_received.data(), data_received.size());
                                                notifyEvent();
                                                return data_received.size();
                                             })).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(data_received),data_received.size()));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, twoLinesReceived_listenerNotifiedTwice)
{
   const std::vector<uint8_t> data_received = {'a', 'b', 'c', 'd', '\n', 'f', 'g', 'h', 'i', 'j', '\n'};
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillOnce(
                                        Invoke([&](int, void* buffer, ssize_t)->int
                                           {
                                              std::memcpy(buffer, data_received.data(), data_received.size());
                                              notifyEvent();
                                              return data_received.size();
                                           })).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'a', 'b', 'c', 'd', '\n'}),5));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'f', 'g', 'h', 'i', 'j', '\n'}),6));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, manySpecialCharsReceived_dataSplittedByNewline)
{
   const std::vector<uint8_t> data_received = {'\t', ';', ':', '~', '!', '\n', ',', '.', '<', '?', '\n'};
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_)).WillOnce(
                                        Invoke([&](int, void* buffer, ssize_t)->int
                                           {
                                              std::memcpy(buffer, data_received.data(), data_received.size());
                                              notifyEvent();
                                              return data_received.size();
                                           })).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'\t', ';', ':', '~', '!', '\n'}),6));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{',', '.', '<', '?', '\n'}),5));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, lineReceivedWithTrailingData_dataAppendedToNextLine)
{
   const std::vector<uint8_t> data_received = {'a', 'b', 'c', 'd', '\n', 'f', 'g', 'h', 'i'};
   const std::vector<uint8_t> second_data_received = {'j', '\n'};
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_))
                        .WillOnce(
                         Invoke([&](int, void* buffer, ssize_t)->int
                            {
                               std::memcpy(buffer, data_received.data(), data_received.size());
                               return data_received.size();
                            }))
                        .WillOnce(
                         Invoke([&](int, void* buffer, ssize_t)->int
                            {
                               std::memcpy(buffer, second_data_received.data(), second_data_received.size());
                               notifyEvent();
                               return second_data_received.size();
                            }))
                        .WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'a', 'b', 'c', 'd', '\n'}),5));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'f', 'g', 'h', 'i', 'j', '\n'}),6));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, emptyStringsReceived_emptyStringsReported)
{
   const std::vector<uint8_t> data_received = {'\n','\n','\n','\n','\n','a', 'b', 'c', '\n'};
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,_))
                                       .WillOnce(
                                        Invoke([&](int, void* buffer, ssize_t)->int
                                           {
                                              std::memcpy(buffer, data_received.data(), data_received.size());
                                              notifyEvent();
                                              return data_received.size();
                                           }))
                                       .WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'\n'}),1)).Times(5);
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(
                              std::vector<uint8_t>{'a', 'b', 'c','\n'}),4));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}
TEST_F(SerialDriverFixture, entireBufferFilledTwice_dataCorrectlySplitted)
{
   constexpr uint32_t BUFFER_SIZE = 4096;
   std::vector<uint8_t> data_received (BUFFER_SIZE, 0x01);
   std::vector<uint8_t> second_data_received (BUFFER_SIZE, 0x02);
   data_received[BUFFER_SIZE - 1] = '\n';
   second_data_received[BUFFER_SIZE - 1] = '\n';
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,BUFFER_SIZE))
                  .WillOnce(
                   Invoke([&](int, void* buffer, ssize_t)->int
                      {
                         std::memcpy(buffer, data_received.data(), data_received.size());
                         return data_received.size();
                      }))
                  .WillOnce(
                   Invoke([&](int, void* buffer, ssize_t)->int
                      {
                         std::memcpy(buffer, second_data_received.data(), second_data_received.size());
                         notifyEvent();
                         return second_data_received.size();
                      }));
   EXPECT_CALL(*sys_call_mock, read(test_fd, _,Ne(BUFFER_SIZE))).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, tcgetattr(test_fd, _)).WillRepeatedly(Return(0));
   EXPECT_CALL(*sys_call_mock, open(serial_settings.device.c_str(),O_RDWR)).WillOnce(Return(test_fd));
   EXPECT_CALL(*sys_call_mock, cfsetispeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, cfsetospeed(_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, tcsetattr(test_fd, TCSANOW, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(test_fd));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(data_received),data_received.size()));
   EXPECT_CALL(listener, onSerialEvent(DriverEvent::DATA_RECV,ContainerEq(second_data_received),second_data_received.size()));
   EXPECT_TRUE(m_test_subject->open(DataMode::NEW_LINE_DELIMITER, serial_settings));
   EXPECT_TRUE(m_test_subject->isOpened());
   waitForEvent();
   m_test_subject->close();
}


}
}

