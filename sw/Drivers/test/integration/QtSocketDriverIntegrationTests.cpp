#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>

#include "Logger.h"
#include "ISocketServer.h"
#include "ISocketClient.h"
/* ============================= */
/**
 * @file SocketDriverIntegrationTests.cpp
 *
 * @brief Integration tests to check communication between client and server drivers
 *
 * @author Jacek Skowronek
 * @date 08/04/2022
 */
/* ============================= */

using namespace testing;
using namespace Drivers;

constexpr uint16_t TEST_PORT = 6666;

struct ServerListenerMock
{
   MOCK_METHOD4(onServerEvent, void(int, Drivers::SocketServer::ServerEvent, const std::vector<uint8_t>&, size_t));
};
struct ClientListenerMock
{
   MOCK_METHOD3(onClientEvent, void(Drivers::SocketClient::ClientEvent, const std::vector<uint8_t>&, size_t));
};
ServerListenerMock* server_listener_mock;
ClientListenerMock* client_listener_mock;

class ClientListenerMockImpl : public SocketClient::ClientListener
{
public:
   void onClientEvent(SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t size)
   {
      client_listener_mock->onClientEvent(ev, data, size);
   }
};

class ServerListenerMockImpl : public SocketServer::ServerListener
{
public:
   void onServerEvent(int client_id, SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size)
   {
      server_listener_mock->onServerEvent(client_id, ev, data, size);
   }
};

struct QtSocketDriveFixture : public ::testing::Test
{
   void SetUp()
   {
      server_listener_mock = new ServerListenerMock;
      client_listener_mock = new ClientListenerMock;
   }
   void prepareDrivers(bool is_delimiter_mode)
   {
      if (is_delimiter_mode)
      {
         m_server = Drivers::SocketServer::ISocketServer::create(SocketServer::DataMode::NEW_LINE_DELIMITER);
         m_client = Drivers::SocketClient::ISocketClient::create(SocketClient::DataMode::NEW_LINE_DELIMITER);
      }
      else
      {
         m_server = Drivers::SocketServer::ISocketServer::create(SocketServer::DataMode::PAYLOAD_HEADER);
         m_client = Drivers::SocketClient::ISocketClient::create(SocketClient::DataMode::PAYLOAD_HEADER);
      }
   }
   void TearDown()
   {
      m_server.reset(nullptr);
      m_client.reset(nullptr);
      delete server_listener_mock;
      delete client_listener_mock;
   }
   std::unique_ptr<SocketServer::ISocketServer> m_server;
   std::unique_ptr<SocketClient::ISocketClient> m_client;
};

TEST_F(QtSocketDriveFixture, server_to_client_data_exchange_in_delimiter_mode)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};

   /**
    * <b>scenario</b>: Server sends data to client.<br>
    * <b>expected</b>: Client notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA),TEST_DATA.size()));
   m_server->write(TEST_DATA, TEST_DATA.size());
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, server_to_client_multiple_data_exchange_in_delimiter_mode)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1 = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_2 = {'s','e','c','o','n','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_3 = {'t','h','i','r','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};

   /**
    * <b>scenario</b>: Server sends data to client.<br>
    * <b>expected</b>: Client notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_3),TEST_DATA_3.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_2, TEST_DATA_2.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_3, TEST_DATA_3.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, client_to_server_multiple_data_exchange_in_delimiter_mode)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1 = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_2 = {'s','e','c','o','n','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_3 = {'t','h','i','r','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};

   /**
    * <b>scenario</b>: Client sends data to server.<br>
    * <b>expected</b>: Server notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_3),TEST_DATA_3.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_3, TEST_DATA_3.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, client_to_server_big_data_exchange_in_delimiter_mode)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x01);
   std::vector<uint8_t> TEST_DATA_2(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x02);

   /* add newline at the end of data */
   TEST_DATA_1[SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 11] = '\n';
   TEST_DATA_2[SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 11] = '\n';

   /**
    * <b>scenario</b>: Client sends data to server.<br>
    * <b>expected</b>: Server notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, server_unexpectedly_closed)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x01);

   /* add newline at the end of data */
   TEST_DATA_1[SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 11] = '\n';

   /**
    * <b>scenario</b>: Client sends data to server, then disconnects.<br>
    * <b>expected</b>: Server notifies the listener about new data, then about client disconnection.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* server unexpectedly closed */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DISCONNECTED,_,_));
   m_server->stop();

   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);
   m_client->disconnect();
}

TEST_F(QtSocketDriveFixture, client_reconnection)
{
   prepareDrivers(true);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x01);
   std::vector<uint8_t> TEST_DATA_2(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x02);

   /* add newline at the end of data */
   TEST_DATA_1[SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 11] = '\n';
   TEST_DATA_2[SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 11] = '\n';

   /**
    * <b>scenario</b>: Client sends data to server.<br>
    * <b>expected</b>: Server notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* reconnect client */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, server_to_client_data_exchange_in_header_mode)
{
   prepareDrivers(false);

   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!'};

   /**
    * <b>scenario</b>: Server sends data to client.<br>
    * <b>expected</b>: Client notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA),TEST_DATA.size()));
   m_server->write(TEST_DATA, TEST_DATA.size());
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(300));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, server_to_client_multiple_data_exchange_in_header_mode)
{
   prepareDrivers(false);
   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1 = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_2 = {'s','e','c','o','n','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_3 = {'t','h','i','r','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};

   /**
    * <b>scenario</b>: Server sends data to client.<br>
    * <b>expected</b>: Client notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA_3),TEST_DATA_3.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_2, TEST_DATA_2.size()));
   ASSERT_TRUE(m_server->write(TEST_DATA_3, TEST_DATA_3.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, client_to_server_multiple_data_exchange_in_header_mode)
{
   prepareDrivers(false);
   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1 = {'h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!','\n'};
   std::vector<uint8_t> TEST_DATA_2 = {'s','e','c','o','n','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};
   std::vector<uint8_t> TEST_DATA_3 = {'t','h','i','r','d',' ','h','e','l','l','o',' ','f','r','o','m',' ','s','e','r','v','e','r','!', '\n'};

   /**
    * <b>scenario</b>: Client sends data to server.<br>
    * <b>expected</b>: Server notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_3),TEST_DATA_3.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_3, TEST_DATA_3.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, client_to_server_big_data_exchange_in_header_mode)
{
   prepareDrivers(false);
   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   std::vector<uint8_t> TEST_DATA_1(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x01);
   std::vector<uint8_t> TEST_DATA_2(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH - 10, 0x02);

   /**
    * <b>scenario</b>: Client sends data to server.<br>
    * <b>expected</b>: Server notifies the listener about new data.<br>
    * ************************************************
    */

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(QtSocketDriveFixture, recv_buffer_overflow_tests_delimiter_mode)
{
   prepareDrivers(true);
   ServerListenerMockImpl server_listener;
   ClientListenerMockImpl client_listener;

   /**
   * <b>scenario</b>: Data exchange in two directions. First message of size SOCKET_MAX_PAYLOAD_LENGTH without delimiter,
   *                  than next SOCKET_MAX_PAYLOAD_LENGTH message with newline delimiter in the middle of payload. <br>
   *                  then just few bytes with newline delimiter sent. <br>
   * <b>expected</b>: First buffer shall be abandoned, due to lack of delimiter.<br>
   *                  Second buffer shall be processed, first part of data shall be notified via listener, second part shall be moved to the beginning of the buffer.<br>
   *                  Third message shall trigger callback with second part of second buffer concatenated with third message.<br>
   * ************************************************
   */
   constexpr uint32_t THIRD_MESSAGE_SIZE = 1024;
   std::vector<uint8_t> FIRST_MESSAGE (SocketServer::SOCKET_MAX_PAYLOAD_LENGTH, 0xAA);
   std::vector<uint8_t> SECOND_MESSAGE (SocketServer::SOCKET_MAX_PAYLOAD_LENGTH, 0xBB);
   std::vector<uint8_t> THIRD_MESSAGE (THIRD_MESSAGE_SIZE, 0xBB);

   // place newline in the middle of second buffer
   SECOND_MESSAGE[(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH / 2) - 1] = '\n';
   // place newline at the end of third buffer
   THIRD_MESSAGE[THIRD_MESSAGE_SIZE - 1] = '\n';

   // prepare expected messages - First callback shall contain the data from first half of SECOND_MESSAGE (including newline)
   std::vector<uint8_t> FIRST_CALLBACK = {SECOND_MESSAGE.begin(), SECOND_MESSAGE.begin() + (SocketServer::SOCKET_MAX_PAYLOAD_LENGTH / 2)};
   // prepare expected messages - Second callback shall contain the data from second half of SECOND_MESSAGE + THIRD_MESSAGE
   std::vector<uint8_t> SECOND_CALLBACK = {SECOND_MESSAGE.begin() + (size_t)(SocketServer::SOCKET_MAX_PAYLOAD_LENGTH / 2), SECOND_MESSAGE.end()};
   SECOND_CALLBACK.insert(SECOND_CALLBACK.end(), THIRD_MESSAGE.begin(), THIRD_MESSAGE.end());

   /* prepare connection between server and client */
   m_server->addListener(&server_listener);
   m_client->addListener(&client_listener);

   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));

   ASSERT_TRUE(m_server->start(TEST_PORT, 1));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(FIRST_CALLBACK),FIRST_CALLBACK.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(SECOND_CALLBACK),SECOND_CALLBACK.size()));
   ASSERT_TRUE(m_client->write(FIRST_MESSAGE, FIRST_MESSAGE.size()));
   ASSERT_TRUE(m_client->write(SECOND_MESSAGE, SECOND_MESSAGE.size()));
   ASSERT_TRUE(m_client->write(THIRD_MESSAGE, THIRD_MESSAGE.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(FIRST_CALLBACK),FIRST_CALLBACK.size()));
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(SECOND_CALLBACK),SECOND_CALLBACK.size()));
   ASSERT_TRUE(m_server->write(FIRST_MESSAGE, FIRST_MESSAGE.size()));
   ASSERT_TRUE(m_server->write(SECOND_MESSAGE, SECOND_MESSAGE.size()));
   ASSERT_TRUE(m_server->write(THIRD_MESSAGE, THIRD_MESSAGE.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

