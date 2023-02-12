#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>

#include "ISocketServer.h"
#include "ISocketClient.h"
#include "Logger.h"
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

constexpr uint16_t TEST_PORT = 5555;

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

struct SocketDriveFixture : public ::testing::Test
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
         m_server = Drivers::SocketFactory::createServer(SocketServer::DataMode::NEW_LINE_DELIMITER);
         m_client = Drivers::SocketFactory::createClient(SocketClient::DataMode::NEW_LINE_DELIMITER);
      }
      else
      {
         m_server = Drivers::SocketFactory::createServer(SocketServer::DataMode::PAYLOAD_HEADER);
         m_client = Drivers::SocketFactory::createClient(SocketClient::DataMode::PAYLOAD_HEADER);
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

TEST_F(SocketDriveFixture, server_to_client_data_exchange_in_header_mode)
{
   prepareDrivers(false);
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
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA),TEST_DATA.size()));
   m_server->write(TEST_DATA, TEST_DATA.size());
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(SocketDriveFixture, server_to_client_data_exchange_in_delimiter_mode)
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
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from server to client */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DATA_RECV, ContainerEq(TEST_DATA),TEST_DATA.size()));
   m_server->write(TEST_DATA, TEST_DATA.size());
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client, stop server */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}

TEST_F(SocketDriveFixture, server_to_client_multiple_data_exchange_in_header_mode)
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

TEST_F(SocketDriveFixture, server_to_client_multiple_data_exchange_in_delimiter_mode)
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

TEST_F(SocketDriveFixture, client_to_server_multiple_data_exchange_in_header_mode)
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

TEST_F(SocketDriveFixture, client_to_server_multiple_data_exchange_in_delimiter_mode)
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

TEST_F(SocketDriveFixture, client_to_server_big_data_exchange_in_header_mode)
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


TEST_F(SocketDriveFixture, client_to_server_big_data_exchange_in_delimiter_mode)
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

TEST_F(SocketDriveFixture, server_unexpectedly_closed)
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
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* server unexpectedly closed */
   EXPECT_CALL(*client_listener_mock, onClientEvent(SocketClient::ClientEvent::SERVER_DISCONNECTED,_,_));
   m_server->stop();

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);
   m_client->disconnect();
}

TEST_F(SocketDriveFixture, client_reconnection)
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
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* reconnect client */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_CONNECTED, _, _));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DISCONNECTED, _, _));
   EXPECT_TRUE(m_client->connect("127.0.0.1", TEST_PORT));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* send data from client to server */
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_1),TEST_DATA_1.size()));
   EXPECT_CALL(*server_listener_mock, onServerEvent(_, SocketServer::ServerEvent::CLIENT_DATA_RECV, ContainerEq(TEST_DATA_2),TEST_DATA_2.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_1, TEST_DATA_1.size()));
   ASSERT_TRUE(m_client->write(TEST_DATA_2, TEST_DATA_2.size()));
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   /* disconnect client */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   m_server->removeListener(&server_listener);
   m_client->removeListener(&client_listener);

   m_server->stop();
}
