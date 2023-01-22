#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <thread>

#include "Logger.h"
#include "test_api.h"
#include "RPCClient.h"
#include "RPCServer.h"

namespace RPC
{

constexpr uint16_t TEST_PORT = 4321;

struct RPCFixture : public testing::Test
{
   void SetUp()
   {
      m_server = std::unique_ptr<RPC::RPCServer>(new RPC::RPCServer);
      m_client = std::unique_ptr<RPC::RPCClient>(new RPC::RPCClient);
   }
   void TearDown()
   {
      m_server.reset(nullptr);
      m_client.reset(nullptr);
   }
   std::unique_ptr<RPC::RPCServer> m_server;
   std::unique_ptr<RPC::RPCClient> m_client;
};



TEST_F(RPCFixture, remote_invoke_uint_types)
{
   /**
    * <b>scenario</b>: Connection established, first method invoked with basic uint types.<br>
    * <b>expected</b>: - Message serialized and sent to client
    *                  - Client correctly parsed the message
    *                  - Client serialized the response and send it back to server
    *                  - Server correctly parsed the response.<br>
    * ************************************************
    */
   RPC::TestCommand1Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = 0x11223344;
   test_request.data2 = 0x55;
   test_request.data3 = 0x6677;

   RPC::TestCommand1Reply test_reply = {};
   test_reply.result = true;

   auto FirstCommandExecutor = [&](const std::vector<uint8_t>& data) -> bool
         {
            RPC::TestCommand1Request request {};
            deserialize(data, request);
            EXPECT_EQ(request.cmd, test_request.cmd);
            EXPECT_EQ(request.data1, test_request.data1);
            EXPECT_EQ(request.data2, test_request.data2);
            EXPECT_EQ(request.data3, test_request.data3);

            std::vector<uint8_t> buffer;
            serialize(buffer, test_reply);
            return m_server->respond(buffer);
         };


   m_server->addCommandExecutor(Command::TestCommand1, FirstCommandExecutor);

   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_client->connect("127.0.0.1", TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   RPC::result<RPC::TestCommand1Reply> response = m_client->invoke<RPC::TestCommand1Reply, RPC::TestCommand1Request>(test_request);

   EXPECT_TRUE(response.ready());
   EXPECT_TRUE(response.reply.result);

   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}

TEST_F(RPCFixture, remote_invoke_string_and_int_types)
{
   /**
    * <b>scenario</b>: Connection established, first method invoked with string and int types.<br>
    * <b>expected</b>: - Message serialized and sent to client
    *                  - Client correctly parsed the message
    *                  - Client serialized the response and send it back to server
    *                  - Server correctly parsed the response.<br>
    * ************************************************
    */

   RPC::TestCommand2Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = -50;
   test_request.data2 = "TEST_REQUEST";

   RPC::TestCommand2Reply test_reply = {};
   test_reply.result = "TEST_RESPONSE";

   auto FirstCommandExecutor = [&](const std::vector<uint8_t>& data) -> bool
         {
            RPC::TestCommand2Request request {};
            deserialize(data, request);
            EXPECT_EQ(request.cmd, test_request.cmd);
            EXPECT_EQ(request.data1, test_request.data1);
            EXPECT_EQ(request.data2, test_request.data2);

            std::vector<uint8_t> buffer;
            serialize(buffer, test_reply);
            return m_server->respond(buffer);
         };


   m_server->addCommandExecutor(Command::TestCommand1, FirstCommandExecutor);

   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_client->connect("127.0.0.1", TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   RPC::result<RPC::TestCommand2Reply> response = m_client->invoke<RPC::TestCommand2Reply, RPC::TestCommand2Request>(test_request);

   EXPECT_TRUE(response.ready());
   EXPECT_EQ(response.reply.result, test_reply.result);

   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}


TEST_F(RPCFixture, no_response_from_client)
{
   /**
    * <b>scenario</b>: Connection established, first method invoked with basic uint types.<br>
    * <b>expected</b>: - Message serialized and sent to client
    *                  - Client does not parse the message
    *                  - Server timeout expired, result marked as not ready.<br>
    * ************************************************
    */

   RPC::TestCommand2Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = -50;
   test_request.data2 = "TEST_REQUEST";

   RPC::TestCommand2Reply test_reply = {};
   test_reply.result = "TEST_RESPONSE";

   auto FirstCommandExecutor = [&](const std::vector<uint8_t>& data) -> bool
         {
            RPC::TestCommand2Request request {};
            deserialize(data, request);
            EXPECT_EQ(request.cmd, test_request.cmd);
            EXPECT_EQ(request.data1, test_request.data1);
            EXPECT_EQ(request.data2, test_request.data2);
            /* no response for received request */
            return false;
         };


   m_server->addCommandExecutor(Command::TestCommand1, FirstCommandExecutor);

   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_client->connect("127.0.0.1", TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   RPC::result<RPC::TestCommand2Reply> response = m_client->invoke<RPC::TestCommand2Reply, RPC::TestCommand2Request>(test_request);

   EXPECT_FALSE(response.ready());

   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}

TEST_F(RPCFixture, invoking_when_client_not_connected)
{
   /**
    * <b>scenario</b>: Invoking method while client is not connected.<br>
    * <b>expected</b>: - Reponse should be marked as not ready.<br>
    * ************************************************
    */

   RPC::TestCommand2Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = -50;
   test_request.data2 = "TEST_REQUEST";

   RPC::TestCommand2Reply test_reply = {};
   test_reply.result = "TEST_RESPONSE";

   auto FirstCommandExecutor = [&](const std::vector<uint8_t>& data) -> bool
         {
            RPC::TestCommand2Request request {};
            deserialize(data, request);
            EXPECT_EQ(request.cmd, test_request.cmd);
            EXPECT_EQ(request.data1, test_request.data1);
            EXPECT_EQ(request.data2, test_request.data2);
            /* no response for received request */
            return false;
         };


   m_server->addCommandExecutor(Command::TestCommand1, FirstCommandExecutor);

   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   /* client is not connecting here */

   RPC::result<RPC::TestCommand2Reply> response = m_client->invoke<RPC::TestCommand2Reply, RPC::TestCommand2Request>(test_request);

   EXPECT_FALSE(response.ready());

   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}

TEST_F(RPCFixture, invoking_when_client_suddenly_disconnecs)
{
   /**
    * <b>scenario</b>: Invoking method while suddenly disconnects.<br>
    * <b>expected</b>: - Reponse should be marked as not ready.<br>
    * ************************************************
    */

   RPC::TestCommand2Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = -50;
   test_request.data2 = "TEST_REQUEST";

   RPC::TestCommand2Reply test_reply = {};
   test_reply.result = "TEST_RESPONSE";

   auto FirstCommandExecutor = [&](const std::vector<uint8_t>& data) -> bool
         {
            RPC::TestCommand2Request request {};
            deserialize(data, request);
            EXPECT_EQ(request.cmd, test_request.cmd);
            EXPECT_EQ(request.data1, test_request.data1);
            EXPECT_EQ(request.data2, test_request.data2);
            /* no response for received request */
            return false;
         };


   m_server->addCommandExecutor(Command::TestCommand1, FirstCommandExecutor);

   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   m_client->connect("127.0.0.1", TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   /* client disconnects here */
   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   RPC::result<RPC::TestCommand2Reply> response = m_client->invoke<RPC::TestCommand2Reply, RPC::TestCommand2Request>(test_request);

   EXPECT_FALSE(response.ready());

   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}

TEST_F(RPCFixture, no_executor_registered_on_client_side)
{
   /**
    * <b>scenario</b>: Connection established, first method invoked with basic uint types, but no executor registered on client side.<br>
    * <b>expected</b>: - Message serialized and sent to client
    *                  - Client does not parse the message
    *                  - Server timeout expired, response marked as not ready.<br>
    * ************************************************
    */
   RPC::TestCommand1Request test_request = {};
   test_request.cmd = Command::TestCommand1;
   test_request.data1 = 0x11223344;
   test_request.data2 = 0x55;
   test_request.data3 = 0x6677;

   RPC::TestCommand1Reply test_reply = {};
   test_reply.result = true;


   m_server->start(TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_client->connect("127.0.0.1", TEST_PORT);
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

   RPC::result<RPC::TestCommand1Reply> response = m_client->invoke<RPC::TestCommand1Reply, RPC::TestCommand1Request>(test_request);

   EXPECT_FALSE(response.ready());

   m_client->disconnect();
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   m_server->stop();
}


}

