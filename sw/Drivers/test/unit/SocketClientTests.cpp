#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SocketClient.h"
#include <sys/socket.h>
#include <cstring>
/* ============================= */
/**
 * @file SocketClientTests.cpp
 *
 * @brief Unit tests to verify behavior of Socket Client.
 *
 * @author Jacek Skowronek
 * @date 09/04/2021
 */
/* ============================= */

using namespace testing;

struct SystemCallMock
{
   MOCK_METHOD3(socket, int(int, int, int));
   MOCK_METHOD5(setsockopt, int(int, int, int, const void *, socklen_t));
   MOCK_METHOD1(htons, uint32_t(uint32_t));
   MOCK_METHOD3(inet_pton, int(int af, const char * src, void * dst));
   MOCK_METHOD3(connect, int(int sockfd, const struct sockaddr *addr, socklen_t addrlen));
   MOCK_METHOD4(send, ssize_t(int socket, const void *message, size_t length, int flags));
   MOCK_METHOD4(recv, ssize_t(int socket, void *buffer, size_t length, int flags));
   MOCK_METHOD1(close, int(int));
};

struct ClientListenerMock
{
   MOCK_METHOD3(onClientEvent, void(Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>&, size_t));
};

SystemCallMock* sys_call_mock;
ClientListenerMock* listener_mock;

namespace system_call
{
__attribute__((weak)) int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
   return sys_call_mock->connect(socket, address, address_len);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return sys_call_mock->recv(socket, buffer, length, flags);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return sys_call_mock->send(socket, message, length, flags);
}
__attribute__((weak)) int socket(int domain, int type, int protocol)
{
   return sys_call_mock->socket(domain, type, protocol);
}
__attribute__((weak)) int close (int fd)
{
   return sys_call_mock->close(fd);
}
__attribute__((weak)) int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
   return sys_call_mock->setsockopt(socket, level, option_name, option_value, option_len);
}
__attribute__((weak)) uint32_t htons(uint32_t id)
{
   return sys_call_mock->htons(id);
}
__attribute__((weak)) int inet_pton(int af, const char * src, void * dst)
{
   return sys_call_mock->inet_pton(af, src, dst);
}

}

constexpr int TEST_RETURN_OK = 1;
constexpr int TEST_RETURN_NOK = -1;
constexpr uint32_t TEST_PORT = 5555;
const std::string TEST_IP_ADDRESS("127.0.0.1");
constexpr int TEST_SOCKET_FD = 2;
constexpr uint32_t LESS_CPU_TIMEOUT = 10;

namespace Drivers
{
namespace SocketClient
{

class ClientListenerMockImpl : public ClientListener
{
public:
   void onClientEvent(ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
   {
      listener_mock->onClientEvent(ev, data, size);
   }
};


struct SocketClientFixture : public testing::Test
{
   void SetUp()
   {
      sys_call_mock = new SystemCallMock;
      listener_mock = new ClientListenerMock;
      m_test_subject.reset(new SocketClient(DataMode::NEW_LINE_DELIMITER));
   }
   void TearDown()
   {
      m_test_subject.reset(nullptr);
      delete sys_call_mock;
      delete listener_mock;
   }
   std::unique_ptr<ISocketClient> m_test_subject;
};

TEST_F(SocketClientFixture, client_connect_when_already_connected)
{
   /**
    * <b>scenario</b>: Client connected to server, but next connect request received.<br>
    * <b>expected</b>: False returned, previous connection still operable.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, recv(TEST_SOCKET_FD, _,_, _))
         .WillRepeatedly([](int, void *, size_t, int)->ssize_t
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
            return TEST_RETURN_NOK;
         });
   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_TRUE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_test_subject->isConnected());
   EXPECT_FALSE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));
   m_test_subject->disconnect();
   EXPECT_FALSE(m_test_subject->isConnected());
}

TEST_F(SocketClientFixture, cannot_create_socket)
{
   /**
    * <b>scenario</b>: Client cannot create socket.<br>
    * <b>expected</b>: False returned <br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_RETURN_NOK));
   EXPECT_FALSE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));

}

TEST_F(SocketClientFixture, client_cannot_set_sockopts)
{
   /**
    * <b>scenario</b>: Client cannot set sockopt's.<br>
    * <b>expected</b>: False returned.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_NOK));

   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_FALSE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));
}

TEST_F(SocketClientFixture, client_convert_ip_address_from_text_to_binary)
{
   /**
    * <b>scenario</b>: Client cannot converts the ip address.<br>
    * <b>expected</b>: False returned.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_NOK));

   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_FALSE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));
}

TEST_F(SocketClientFixture, client_cannot_connect)
{
   /**
    * <b>scenario</b>: Client cannot connect to server.<br>
    * <b>expected</b>: False returned.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(TEST_RETURN_NOK));

   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_FALSE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));
}

TEST_F(SocketClientFixture, client_write_to_server_delimiter_mode)
{
   std::vector<uint8_t> TEST_SEND_DATA = {'e','d','c','b','a','\n'};
   std::vector<uint8_t> TEST_EXPECTED_DATA = {'e','d','c','b','a', '\n'};
   /**
    * <b>scenario</b>: Sending data to server.<br>
    * <b>expected</b>: No header added, correct data sent.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, recv(TEST_SOCKET_FD, _,_, _))
         .WillRepeatedly([](int, void *, size_t, int)->ssize_t
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
            return TEST_RETURN_NOK;
         });
   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_CALL(*sys_call_mock, send(TEST_SOCKET_FD,_,_,_)).WillOnce(Invoke([&](int, const void * data, size_t length, int) -> ssize_t
         {
            std::vector<uint8_t> send_data ((uint8_t*) data, ((uint8_t*) data + length));
            EXPECT_THAT(send_data, ContainerEq(TEST_EXPECTED_DATA));
            return length;
         }));

   EXPECT_TRUE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   EXPECT_TRUE(m_test_subject->write(TEST_SEND_DATA, TEST_SEND_DATA.size()));

   m_test_subject->disconnect();
}

TEST_F(SocketClientFixture, client_read_from_server_delimiter_mode)
{

   std::vector<uint8_t> TEST_RECEIVED_DATA = {'a','b','c','d','e','\n'};
   std::vector<uint8_t> TEST_SEND_DATA = {'e','d','c','b','a','\n'};
   /**
    * <b>scenario</b>: Reading data from server.<br>
    * <b>expected</b>: Listener notified about new data only, when newline received.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, recv(TEST_SOCKET_FD, _,_, _))
   .WillOnce([&](int, void * buffer, size_t, int)->ssize_t
   {
      /* simulate received data */
      if (buffer)
      {
         std::memcpy(buffer, TEST_RECEIVED_DATA.data(), TEST_RECEIVED_DATA.size());
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
      return TEST_RECEIVED_DATA.size();
   })
   .WillRepeatedly([](int, void *, size_t, int)->ssize_t
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
      return TEST_RETURN_NOK;
   });
   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_CALL(*listener_mock, onClientEvent(ClientEvent::SERVER_DATA_RECV,_,_))
         .WillOnce(Invoke([&](Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>& buffer, size_t size)
         {
            EXPECT_THAT(buffer, ContainerEq(TEST_RECEIVED_DATA));
            EXPECT_EQ(size, TEST_RECEIVED_DATA.size());
         }));

   ClientListenerMockImpl listener;
   m_test_subject->addListener(&listener);
   EXPECT_TRUE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_test_subject->disconnect();

}

TEST_F(SocketClientFixture, client_read_empty_message_from_server)
{

   std::vector<uint8_t> TEST_RECEIVED_DATA = {'\n'};
   std::vector<uint8_t> TEST_SEND_DATA = {'\n'};
   /**
    * <b>scenario</b>: Reading data from server.<br>
    * <b>expected</b>: Listener notified about new data only, when newline received.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, recv(TEST_SOCKET_FD, _,_, _))
   .WillOnce([&](int, void * buffer, size_t, int)->ssize_t
   {
      /* simulate received data */
      if (buffer)
      {
         std::memcpy(buffer, TEST_RECEIVED_DATA.data(), TEST_RECEIVED_DATA.size());
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
      return TEST_RECEIVED_DATA.size();
   })
   .WillRepeatedly([](int, void *, size_t, int)->ssize_t
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(LESS_CPU_TIMEOUT));
      return TEST_RETURN_NOK;
   });
   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));

   EXPECT_CALL(*listener_mock, onClientEvent(ClientEvent::SERVER_DATA_RECV,_,_))
         .WillOnce(Invoke([&](Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>& buffer, size_t size)
         {
            EXPECT_THAT(buffer, ContainerEq(TEST_RECEIVED_DATA));
            EXPECT_EQ(size, TEST_RECEIVED_DATA.size());
         }));

   ClientListenerMockImpl listener;
   m_test_subject->addListener(&listener);
   EXPECT_TRUE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_test_subject->disconnect();

}

TEST_F(SocketClientFixture, server_disconnected)
{

   /**
    * <b>scenario</b>: Reading data from server.<br>
    * <b>expected</b>: Listener notified about new data only, when newline received.<br>
    * ************************************************
    */
   EXPECT_CALL(*sys_call_mock, socket(AF_INET, SOCK_STREAM, _)).WillOnce(Return(TEST_SOCKET_FD));
   EXPECT_CALL(*sys_call_mock, setsockopt(_,_,_,_,_)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, htons(_)).WillOnce(Return(TEST_PORT));
   EXPECT_CALL(*sys_call_mock, inet_pton(AF_INET, _, _)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*sys_call_mock, connect(TEST_SOCKET_FD, _, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, recv(TEST_SOCKET_FD, _,_, _)).WillOnce(Return(0));
   EXPECT_CALL(*sys_call_mock, close(TEST_SOCKET_FD)).WillOnce(Return(TEST_RETURN_OK));
   EXPECT_CALL(*listener_mock, onClientEvent(ClientEvent::SERVER_DISCONNECTED,_,_));

   ClientListenerMockImpl listener;
   m_test_subject->addListener(&listener);
   EXPECT_TRUE(m_test_subject->connect(TEST_IP_ADDRESS, TEST_PORT));

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_test_subject->disconnect();

}



}
}
