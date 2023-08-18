#include <memory>
#include <thread>
#include "gtest/gtest.h"
#include "TestFramework.h"
#include "TestFrameworkCommon.h"
#include "TestFrameworkAPI.h"
#include "RPCClient.h"
#include "Logger.h"
#include "Settings.h"
#include "ApplicationExecutor.hpp"
#include "ISocketClient.h"
#include "ISocketServer.h"

struct SerialPortLoopback
{
   std::string device1;
   std::string device2;
   TF::ApplicationExecutor executor;
};

class SocketServerHandler : public Drivers::SocketServer::ServerListener
{
public:
   SocketServerHandler(uint32_t port):
   m_port(port),
   m_server(Drivers::SocketServer::ISocketServer::create(Drivers::SocketServer::DataMode::NEW_LINE_DELIMITER))
   {
      UT_Log(TEST_FRAMEWORK, LOW, "starting server on port %u", port);
      m_server->addListener(this);
      m_server->start(port, 1);
   }
   ~SocketServerHandler()
   {
      UT_Log(TEST_FRAMEWORK, LOW, "stopping server on port %u", m_port);
      m_server->stop();
   }
   void clearBuffer()
   {
      m_buffer.clear();
   }
   bool write(const std::string& message)
   {
      return m_server->write({message.begin(), message.end()}, message.size());
   }
   bool wasMessageReceived(const std::string& message)
   {
      bool result = false;
      if (std::find(m_buffer.begin(), m_buffer.end(), message) != m_buffer.end())
      {
         result = true;
      }
      return result;
   }
   uint32_t getPort() const
   {
      return m_port;
   }
   size_t bufferSize() const
   {
      return m_buffer.size();
   }
private:

   void onServerEvent(int client_id, Drivers::SocketServer::ServerEvent ev, const std::vector<uint8_t>& data, size_t size) override
   {
      switch(ev)
      {
      case Drivers::SocketServer::ServerEvent::CLIENT_CONNECTED:
         UT_Log(TEST_FRAMEWORK, LOW, "client connected on port %u", m_port);
         break;
      case Drivers::SocketServer::ServerEvent::CLIENT_DISCONNECTED:
         UT_Log(TEST_FRAMEWORK, LOW, "client disconnected on port %u", m_port);
         break;
      case Drivers::SocketServer::ServerEvent::CLIENT_DATA_RECV:
         m_buffer.push_back(std::string(data.begin(), data.end()));
         break;
      default:
         UT_Assert(false && "Incorrect event");
         break;
      }
   }
   uint32_t m_port;
   std::unique_ptr<Drivers::SocketServer::ISocketServer> m_server;
   std::vector<std::string> m_buffer;
};

struct TraceForwarder : public Drivers::SocketClient::ClientListener
{
   void onClientEvent(Drivers::SocketClient::ClientEvent ev, const std::vector<uint8_t>& data, size_t)
   {
      switch (ev)
      {
      case Drivers::SocketClient::ClientEvent::SERVER_DISCONNECTED:
      {
         UT_Log(TEST_FRAMEWORK, ERROR, "application trace server disconnected!");
         break;
      }
      default:
      {
         UT_Log(TEST_FRAMEWORK, INFO, "FWD: %s", std::string({data.begin(), data.end()}).c_str());
         break;
      }
      }
   }
};

static std::unique_ptr<RPC::RPCClient> g_rpc_client;
static TF::ApplicationExecutor g_test_application;
static std::vector<SerialPortLoopback> g_serial_loopbacks;
static std::map<std::string, std::unique_ptr<Drivers::Serial::ISerialDriver>> g_serial_drivers;
static std::vector<std::unique_ptr<SocketServerHandler>> g_socket_servers;
static std::unique_ptr<Drivers::SocketClient::ISocketClient> g_trace_client;
static TraceForwarder g_trace_forwarder;

bool isLoopbackActive(const std::string& device)
{
   bool result = false;
   for (const auto& serial : g_serial_loopbacks)
   {
      if (serial.device1 == device ||
          serial.device2 == device)
      {
         result = true;
         break;
      }
   }
   return result;
}

std::string createTimestamp()
{
   auto currentTime = std::chrono::system_clock::now();
   std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
   auto ts = localtime (&tt);

   char buffer [50];
   std::snprintf(buffer, 50, "%.2u%.2u%.2u_%.2u%.2u%.2u", ts->tm_mday, ts->tm_mon, ts->tm_year, ts->tm_hour, ts->tm_min, ts->tm_sec);
   return buffer;
}

namespace TF
{


bool Connect()
{
   bool result = false;

   Settings::SettingsHandler::create();
   Settings::SettingsHandler::get()->start(CONFIG_FILE);

   SETTING_SET_BOOL(Logger_supportSocketLogging, false);
   SETTING_SET_BOOL(Logger_supportFileLogging, true);

   LoggerEngine::get()->startFrontends();
   LoggerEngine::get()->setLogFileName(std::string(RUNTIME_OUTPUT_DIR) + "/" +
                                       ::testing::UnitTest::GetInstance()->current_test_suite()->name() + "_" +
                                       createTimestamp());

   UT_Log(TEST_FRAMEWORK, LOW, "starting application %s", APPLICATION_PATH.c_str());
   UT_Assert(g_test_application.startApplication(APPLICATION_PATH, ""));
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   UT_Log(TEST_FRAMEWORK, LOW, "connecting TestFramework to RPC server");
   g_rpc_client = std::unique_ptr<RPC::RPCClient>(new RPC::RPCClient);
   UT_Assert(g_rpc_client);

   if (g_rpc_client->connect(std::string(SERVER_IP_ADDRESS), SERVER_PORT))
   {
      UT_Log(TEST_FRAMEWORK, LOW, "TestFramework connected, trying to connect to application trace server");
      g_trace_client = Drivers::SocketClient::ISocketClient::create(Drivers::SocketClient::DataMode::NEW_LINE_DELIMITER);
      UT_Assert(g_trace_client);
      result = g_trace_client->connect("127.0.0.1", SETTING_GET_U32(Logger_socketPort));
      g_trace_client->addListener(&g_trace_forwarder);
      UT_Log_If(!result, TEST_FRAMEWORK, LOW, "Cannot connect to trace server at 127.0.0.1:%u", SETTING_GET_U32(Logger_socketPort));
   }
   return result;
}
void Disconnect()
{
   UT_Assert(g_rpc_client);
   g_rpc_client->disconnect();
   g_trace_client->disconnect();
   g_trace_client->removeListener(&g_trace_forwarder);
   g_rpc_client.reset(nullptr);
   g_trace_client.reset(nullptr);
   UT_Assert(g_test_application.stopApplication());
   LoggerEngine::get()->stopFrontends();
   Settings::SettingsHandler::destroy();
}
void BeginTest()
{
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s", __func__, ::testing::UnitTest::GetInstance()->current_test_info()->name());
}
void FinishTest()
{
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s", __func__, ::testing::UnitTest::GetInstance()->current_test_info()->name());
}
void wait(uint32_t ms)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
double getCPUUsage()
{
   UT_Assert(g_test_application.isRunning());
   return g_test_application.getCPUUsage();
}
namespace Serial
{

bool startForwarding(const std::string& device1, const std::string& device2)
{
   bool result = false;
   if (!isLoopbackActive(device1) && !isLoopbackActive(device2))
   {
      SerialPortLoopback loopback;
      loopback.device1 = device1;
      loopback.device2 = device2;
      std::string argument = "pty,raw,link=" + device1 + " pty,raw,link=" + device2;
      result = loopback.executor.startApplication("socat", argument.c_str());
      if (result)
      {
         g_serial_loopbacks.emplace_back(loopback);
      }
   }
   return result;
}
bool stopForwarding(const std::string& device1, const std::string& device2)
{
   bool result = false;
   auto it = std::find_if(g_serial_loopbacks.begin(), g_serial_loopbacks.end(), [&](SerialPortLoopback& loopback){return (loopback.device1 == device1) || (loopback.device2 == device2);});
   if (it != g_serial_loopbacks.end())
   {
      (void)it->executor.stopApplication();
      g_serial_loopbacks.erase(it);
      result = true;
   }
   return result;
}
bool openSerialPort(const Drivers::Serial::Settings& settings)
{
   bool result = false;
   if (g_serial_drivers.find(settings.device) == g_serial_drivers.end())
   {
      UT_Log(TEST_FRAMEWORK, LOW, "%s device %s not found, creating driver", __func__, settings.device.c_str());
      auto driver = Drivers::Serial::ISerialDriver::create();
      if (driver->open(Drivers::Serial::DataMode::NEW_LINE_DELIMITER, settings))
      {
         UT_Log(TEST_FRAMEWORK, LOW, "%s device %s started", __func__, settings.device.c_str());
         g_serial_drivers[settings.device] = std::move(driver);
         result = true;
      }
   }
   return result;
}
bool closeSerialPort(const std::string& device)
{
   bool result = false;
   auto it = g_serial_drivers.find(device);
   if (it != g_serial_drivers.end())
   {
      UT_Log(TEST_FRAMEWORK, LOW, "%s device %s found, closing", __func__, device);
      g_serial_drivers[device]->close();
      g_serial_drivers[device].reset(nullptr);
      g_serial_drivers.erase(it);
      result = true;
   }
   return result;
}
bool sendMessage(const std::string& device, const std::string& message)
{
   bool result = false;
   if (g_serial_drivers.find(device) != g_serial_drivers.end())
   {
      result = g_serial_drivers[device]->write({message.begin(), message.end()}, message.size());
   }
   UT_Log_If(!result, TEST_FRAMEWORK, LOW, "%s device %s not found or not opened!", __func__, device);
   return result;
}

}

namespace Socket
{
bool startServer(uint32_t port)
{
   bool result = false;
   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it == g_socket_servers.end())
   {
      g_socket_servers.emplace_back(new SocketServerHandler(port));
      result = true;
   }
   return result;
}
bool stopServer(uint32_t port)
{
   bool result = false;
   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it != g_socket_servers.end())
   {
      g_socket_servers.erase(it);
      result = true;
   }
   return result;
}
bool sendMessage(uint32_t port, const std::string& message)
{
   bool result = false;
   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it != g_socket_servers.end())
   {
      result = (*it)->write(message);
   }
   return result;
}
bool checkMessageReceived(uint32_t port, const std::string& message)
{
   bool result = false;
   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it != g_socket_servers.end())
   {
      result = (*it)->wasMessageReceived(message);
   }
   return result;
}
uint32_t bufferSize(uint32_t port)
{
   uint32_t result = 0;

   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it != g_socket_servers.end())
   {
      result = static_cast<uint32_t>((*it)->bufferSize());
   }
   return result;
}
void clearMessageBuffer(uint32_t port)
{
   auto it = std::find_if(g_socket_servers.begin(), g_socket_servers.end(), [&](std::unique_ptr<SocketServerHandler>& handler){return handler->getPort() == port;});
   if (it != g_socket_servers.end())
   {
      (*it)->clearBuffer();
   }
}

}  // Socket

namespace Buttons
{

uint32_t getBackgroundColor(const std::string& name)
{
   uint32_t result = 0;
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.background_color;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %.6x", __func__, name.c_str(), result);
   return result;
}
uint32_t getFontColor(const std::string& name)
{
   uint32_t result = 0;
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.font_color;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %.6x", __func__, name.c_str(), result);
   return result;
}
bool isEnabled(const std::string& name)
{
   bool result = 0;
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.enabled;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
   return result;
}
bool isChecked(const std::string& name)
{
   bool result = 0;
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.checked;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
   return result;
}
bool isCheckable(const std::string& name)
{
   bool result = 0;
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.checkable;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
   return result;
}
std::string getText(const std::string& name)
{
   std::string result = "";
   RPC::GetButtonStateRequest request {};
   request.button_name = name;

   RPC::result<RPC::GetButtonStateReply> reply = g_rpc_client->invoke<RPC::GetButtonStateReply, RPC::GetButtonStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.text;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s [%s]", __func__, name.c_str(), result.c_str());
   return result;
}
bool simulateButtonClick(const std::string& name)
{
   bool result = 0;
   RPC::ButtonClickRequest request {};
   request.button_name = name;

   RPC::result<RPC::ButtonClickReply> reply = g_rpc_client->invoke<RPC::ButtonClickReply, RPC::ButtonClickRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.clicked;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
   return result;
}
bool simulateContextMenuClick(const std::string& name)
{
   bool result = 0;
   RPC::ButtonContextMenuClickRequest request {};
   request.button_name = name;

   RPC::result<RPC::ButtonContextMenuClickReply> reply = g_rpc_client->invoke<RPC::ButtonContextMenuClickReply, RPC::ButtonContextMenuClickRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.clicked;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
   return result;
}

} // Buttons

namespace Common
{
std::string getCommand()
{
   std::string result = "";
   RPC::GetCommandRequest request {};

   RPC::result<RPC::GetCommandReply> reply = g_rpc_client->invoke<RPC::GetCommandReply, RPC::GetCommandRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.command;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %s", __func__, reply.ready(), result.c_str());
   return result;
}
bool setCommand(const std::string& command)
{
   bool result = false;
   RPC::SetCommandRequest request {};
   request.command = command;

   RPC::result<RPC::SetCommandReply> reply = g_rpc_client->invoke<RPC::SetCommandReply, RPC::SetCommandRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.reply;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}
bool isLineEndingVisible(const std::string ending)
{
   bool result = false;
   RPC::GetAllLineEndingsRequest request {};

   RPC::result<RPC::GetAllLineEndingsReply> reply = g_rpc_client->invoke<RPC::GetAllLineEndingsReply, RPC::GetAllLineEndingsRequest>(request);
   if (reply.ready())
   {
      result = std::find(reply.reply.lineendings.begin(), reply.reply.lineendings.end(), ending) != reply.reply.lineendings.end();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s [%s] %u %u", __func__, ending.c_str(), reply.ready(), result);
   return result;
}
bool setLineEnding(const std::string ending)
{
   bool result = false;
   RPC::SetLineEndingRequest request {};
   request.lineending = ending;

   RPC::result<RPC::SetLineEndingReply> reply = g_rpc_client->invoke<RPC::SetLineEndingReply, RPC::SetLineEndingRequest>(request);
   if (reply.ready() && reply.reply.result)
   {
      result = true;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}
std::string getLineEnding()
{
   std::string result = "";
   RPC::GetLineEndingRequest request {};

   RPC::result<RPC::GetLineEndingReply> reply = g_rpc_client->invoke<RPC::GetLineEndingReply, RPC::GetLineEndingRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.lineending;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u [%s]", __func__, reply.ready(), result.c_str());
   return result;
}
bool isTargetPortVisible(const std::string port_name)
{
   bool result = false;
   RPC::GetAllTargetPortsRequest request {};

   RPC::result<RPC::GetAllTargetPortsReply> reply = g_rpc_client->invoke<RPC::GetAllTargetPortsReply, RPC::GetAllTargetPortsRequest>(request);
   if (reply.ready())
   {
      result = std::find(reply.reply.port_names.begin(), reply.reply.port_names.end(), port_name) != reply.reply.port_names.end();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s [%s] %u %u", __func__, port_name.c_str(), reply.ready(), result);
   return result;
}
bool setTargetPort(const std::string port_name)
{
   bool result = false;
   RPC::SetTargetPortRequest request {};
   request.port_name = port_name;

   RPC::result<RPC::SetTargetPortReply> reply = g_rpc_client->invoke<RPC::SetTargetPortReply, RPC::SetTargetPortRequest>(request);
   if (reply.ready() && reply.reply.result)
   {
      result = true;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}
std::string getTargetPort()
{
   std::string result = "";
   RPC::GetTargetPortRequest request {};

   RPC::result<RPC::GetTargetPortReply> reply = g_rpc_client->invoke<RPC::GetTargetPortReply, RPC::GetTargetPortRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.port_name;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u [%s]", __func__, reply.ready(), result.c_str());
   return result;
}
uint32_t countTargetPorts()
{
   uint32_t result = 0;
   RPC::GetAllTargetPortsRequest request {};

   RPC::result<RPC::GetAllTargetPortsReply> reply = g_rpc_client->invoke<RPC::GetAllTargetPortsReply, RPC::GetAllTargetPortsRequest>(request);
   if (reply.ready())
   {
      result = (uint32_t)reply.reply.port_names.size();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}
uint32_t countCommandHistory()
{
   uint32_t result = 0;
   RPC::GetCommandHistoryRequest request {};

   RPC::result<RPC::GetCommandHistoryReply> reply = g_rpc_client->invoke<RPC::GetCommandHistoryReply, RPC::GetCommandHistoryRequest>(request);
   if (reply.ready())
   {
      result = (uint32_t)reply.reply.history.size();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}
bool isCommandInHistory(const std::string& command, uint32_t index)
{
   bool result = false;
   RPC::GetCommandHistoryRequest request {};

   RPC::result<RPC::GetCommandHistoryReply> reply = g_rpc_client->invoke<RPC::GetCommandHistoryReply, RPC::GetCommandHistoryRequest>(request);
   if (reply.ready())
   {
      auto it = std::find(reply.reply.history.begin(), reply.reply.history.end(), command);
      result = it != reply.reply.history.end();
      if (result)
      {
         uint32_t position = std::distance(reply.reply.history.begin(), it);
         UT_Log(TEST_FRAMEWORK, HIGH, "text %s found @ pos %u", command.c_str(), position);
         result = (position == index);
      }
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
   return result;
}

std::string getMessageBoxText()
{
   std::string result = "";
   RPC::GetMessageBoxRequest request {};

   RPC::result<RPC::GetMessageBoxReply> reply = g_rpc_client->invoke<RPC::GetMessageBoxReply, RPC::GetMessageBoxRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.text;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u text %s", __func__, reply.ready(), result.c_str());
   return result;
}
std::string getMessageBoxTitle()
{
   std::string result = "";
   RPC::GetMessageBoxRequest request {};

   RPC::result<RPC::GetMessageBoxReply> reply = g_rpc_client->invoke<RPC::GetMessageBoxReply, RPC::GetMessageBoxRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.title;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u text %s", __func__, reply.ready(), result.c_str());
   return result;
}
Dialogs::MessageDialog::Icon getMessageBoxIcon()
{
   Dialogs::MessageDialog::Icon result = Dialogs::MessageDialog::Icon::Warning;
   RPC::GetMessageBoxRequest request {};

   RPC::result<RPC::GetMessageBoxReply> reply = g_rpc_client->invoke<RPC::GetMessageBoxReply, RPC::GetMessageBoxRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.icon;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u icon %u", __func__, reply.ready(), (uint8_t)reply.reply.icon);
   return result;
}
bool resetMessageBox()
{
   bool result = false;
   RPC::ResetMessageBoxRequest request {};

   RPC::result<RPC::ResetMessageBoxReply> reply = g_rpc_client->invoke<RPC::ResetMessageBoxReply, RPC::ResetMessageBoxRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.result;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u reply %u", __func__, reply.ready(), reply.reply.result);
   return result;
}
bool setLoggingPath(const std::string& path)
{
   bool result = "";
   RPC::SetLoggingPathRequest request {};
   request.path = path;

   RPC::result<RPC::SetLoggingPathReply> reply = g_rpc_client->invoke<RPC::SetLoggingPathReply, RPC::SetLoggingPathRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.result;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %s", __func__, reply.ready(), path.c_str());
   return result;
}
std::string getLoggingPath()
{
   std::string result = "";
   RPC::GetLoggingPathRequest request {};

   RPC::result<RPC::GetLoggingPathReply> reply = g_rpc_client->invoke<RPC::GetLoggingPathReply, RPC::GetLoggingPathRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.path;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %s", __func__, reply.ready(), result.c_str());
   return result;
}

} // Common

namespace Ports
{
std::string getLabelText(uint8_t id)
{
   std::string result = "";
   RPC::GetPortLabelRequest request {};
   request.id = id;

   RPC::result<RPC::GetPortLabelReply> reply = g_rpc_client->invoke<RPC::GetPortLabelReply, RPC::GetPortLabelRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.text;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s id %u %u %s", __func__, id, reply.ready(), result.c_str());
   return result;
}
std::string getLabelStylesheet(uint8_t id)
{
   std::string result = "";
   RPC::GetPortLabelRequest request {};
   request.id = id;

   RPC::result<RPC::GetPortLabelReply> reply = g_rpc_client->invoke<RPC::GetPortLabelReply, RPC::GetPortLabelRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.stylesheet;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s id %u %u %s", __func__, id, reply.ready(), result.c_str());
   return result;
}
Dialogs::PortSettingDialog::Settings getPortSettings(uint8_t port_id)
{
   Dialogs::PortSettingDialog::Settings result = {};
   RPC::GetPortSettingsRequest request {};
   request.port_id = port_id;

   RPC::result<RPC::GetPortSettingsReply> reply = g_rpc_client->invoke<RPC::GetPortSettingsReply, RPC::GetPortSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.settings;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u settings %s", __func__, reply.ready(), port_id, reply.reply.settings.shortSettingsString().c_str());
   return result;
}
bool setPortSettings(uint8_t port_id, const Dialogs::PortSettingDialog::Settings& settings)
{
   bool result = false;
   RPC::SetPortSettingsRequest request {};
   request.port_id = port_id;
   request.settings = settings;

   RPC::result<RPC::SetPortSettingsReply> reply = g_rpc_client->invoke<RPC::SetPortSettingsReply, RPC::SetPortSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.result;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u result %u settings %s", __func__, reply.ready(), port_id, reply.reply.result, settings.shortSettingsString().c_str());
   return result;
}

} // Ports

namespace TraceFilters
{

std::string getText(const std::string& filter_name)
{
   std::string result = "";
   RPC::GetTraceFilterStateRequest request {};
   request.filter_name = filter_name;

   RPC::result<RPC::GetTraceFilterStateReply> reply = g_rpc_client->invoke<RPC::GetTraceFilterStateReply, RPC::GetTraceFilterStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.text;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s name %s text %s", __func__, reply.reply.filter_name.c_str(), result.c_str());
   return result;
}
uint32_t getBackgroundColor(const std::string& filter_name)
{
   uint32_t result = 0;
   RPC::GetTraceFilterStateRequest request {};
   request.filter_name = filter_name;

   RPC::result<RPC::GetTraceFilterStateReply> reply = g_rpc_client->invoke<RPC::GetTraceFilterStateReply, RPC::GetTraceFilterStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.background_color;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s name %s color %.8x", __func__, reply.reply.filter_name.c_str(), result);
   return result;
}
uint32_t getFontColor(const std::string& filter_name)
{
   uint32_t result = 0;
   RPC::GetTraceFilterStateRequest request {};
   request.filter_name = filter_name;

   RPC::result<RPC::GetTraceFilterStateReply> reply = g_rpc_client->invoke<RPC::GetTraceFilterStateReply, RPC::GetTraceFilterStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.background_color;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s name %s color %.8x", __func__, reply.reply.filter_name.c_str(), result);
   return result;
}
bool isEditable(const std::string& filter_name)
{
   bool result = false;
   RPC::GetTraceFilterStateRequest request {};
   request.filter_name = filter_name;

   RPC::result<RPC::GetTraceFilterStateReply> reply = g_rpc_client->invoke<RPC::GetTraceFilterStateReply, RPC::GetTraceFilterStateRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.enabled;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s name %s editable %u", __func__, reply.reply.filter_name.c_str(), result);
   return result;
}
bool setText(const std::string& filter_name, const std::string& filter)
{
   bool result = false;
   RPC::SetTraceFilterRequest request {};
   request.filter_name = filter_name;
   request.text = filter;

   RPC::result<RPC::SetTraceFilterReply> reply = g_rpc_client->invoke<RPC::SetTraceFilterReply, RPC::SetTraceFilterRequest>(request);
   if (reply.ready() && reply.reply.result)
   {
      result = true;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s name %s text %s result %u", __func__, filter_name.c_str(), filter.c_str(), result);
   return result;
}
bool setSettings(uint8_t id, const Dialogs::TraceFilterSettingDialog::Settings& settings)
{
   bool result = false;
   RPC::SetTraceFilterSettingsRequest request {};
   request.id = id;
   request.settings = settings;

   RPC::result<RPC::SetTraceFilterSettingsReply> reply = g_rpc_client->invoke<RPC::SetTraceFilterSettingsReply, RPC::SetTraceFilterSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.result;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u regex [%s] bg %.6x fg %.6x", __func__, reply.ready(), id, request.settings.regex.c_str(),
         request.settings.background,
         request.settings.font);
   return result;
}
Dialogs::TraceFilterSettingDialog::Settings getSettings(uint8_t id)
{
   Dialogs::TraceFilterSettingDialog::Settings result = {};
   RPC::GetTraceFilterSettingsRequest request {};
   request.id = id;

   RPC::result<RPC::GetTraceFilterSettingsReply> reply = g_rpc_client->invoke<RPC::GetTraceFilterSettingsReply, RPC::GetTraceFilterSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.settings;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u regex [%s] bg %.6x fg %.6x", __func__, reply.ready(), id, reply.reply.settings.regex.c_str(),
         reply.reply.settings.background,
         reply.reply.settings.font);
   return result;
}


} // TraceFilters

namespace TerminalView
{
uint32_t countItems()
{
   uint32_t result = 0;
   RPC::GetTerminalViewCountRequest request {};

   RPC::result<RPC::GetTerminalViewCountReply> reply = g_rpc_client->invoke<RPC::GetTerminalViewCountReply, RPC::GetTerminalViewCountRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.count;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s count %u", __func__, result);
   return result;
}
uint32_t countItemsWithBackgroundColor(uint32_t color)
{
   uint32_t result = 0;
   RPC::GetTerminalViewContentRequest request {};

   RPC::result<RPC::GetTerminalViewContentReply> reply = g_rpc_client->invoke<RPC::GetTerminalViewContentReply, RPC::GetTerminalViewContentRequest>(request);


   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.background_color == color;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s color %.8x items %u", __func__, color, result);
   return result;
}
uint32_t countItemsWithFontColor(uint32_t color)
{
   uint32_t result = 0;
   RPC::GetTerminalViewContentRequest request {};

   RPC::result<RPC::GetTerminalViewContentReply> reply = g_rpc_client->invoke<RPC::GetTerminalViewContentReply, RPC::GetTerminalViewContentRequest>(request);
   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.font_color == color;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s color %.8x items %u", __func__, color, result);
   return result;
}
uint32_t countItemsWithText(const std::string& text)
{
   uint32_t result = 0;
   RPC::GetTerminalViewContentRequest request {};

   RPC::result<RPC::GetTerminalViewContentReply> reply = g_rpc_client->invoke<RPC::GetTerminalViewContentReply, RPC::GetTerminalViewContentRequest>(request);
   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.text == text;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s text %s items %u", __func__, text.c_str(), result);
   return result;
}
int getScrollPosition()
{
   int result = -1;
   RPC::GetTerminalScrollPositionRequest request {};

   RPC::result<RPC::GetTerminalScrollPositionReply> reply = g_rpc_client->invoke<RPC::GetTerminalScrollPositionReply, RPC::GetTerminalScrollPositionRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.position;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s position %d", __func__, result);
   return result;
}
bool setPosition(int position)
{
   RPC::SetTerminalScrollPositionRequest request {};
   request.position = position;

   RPC::result<RPC::SetTerminalScrollPositionReply> reply = g_rpc_client->invoke<RPC::SetTerminalScrollPositionReply, RPC::SetTerminalScrollPositionRequest>(request);
   UT_Log(TEST_FRAMEWORK, LOW, "%s position %d ready %u", __func__, position, reply.ready());
   return reply.ready();
}

} // TerminalView

namespace TraceView
{
uint32_t countItems()
{
   uint32_t result = 0;
   RPC::GetTraceViewCountRequest request {};

   RPC::result<RPC::GetTraceViewCountReply> reply = g_rpc_client->invoke<RPC::GetTraceViewCountReply, RPC::GetTraceViewCountRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.count;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s count %u", __func__, result);
   return result;
}
uint32_t countItemsWithBackgroundColor(uint32_t color)
{
   uint32_t result = 0;
   RPC::GetTraceViewContentRequest request {};

   RPC::result<RPC::GetTraceViewContentReply> reply = g_rpc_client->invoke<RPC::GetTraceViewContentReply, RPC::GetTraceViewContentRequest>(request);
   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.background_color == color;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s color %.8x items %u", __func__, color, result);
   return result;
}
uint32_t countItemsWithFontColor(uint32_t color)
{
   uint32_t result = 0;
   RPC::GetTraceViewContentRequest request {};

   RPC::result<RPC::GetTraceViewContentReply> reply = g_rpc_client->invoke<RPC::GetTraceViewContentReply, RPC::GetTraceViewContentRequest>(request);
   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.font_color == color;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s color %.8x items %u", __func__, color, result);
   return result;
}
uint32_t countItemsWithText(const std::string& text)
{
   uint32_t result = 0;
   RPC::GetTraceViewContentRequest request {};

   RPC::result<RPC::GetTraceViewContentReply> reply = g_rpc_client->invoke<RPC::GetTraceViewContentReply, RPC::GetTraceViewContentRequest>(request);
   if (reply.ready())
   {
      result = std::count_if(reply.reply.content.begin(), reply.reply.content.end(), [&](const RPC::ViewItem& item){return item.text == text;});
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s text %s items %u", __func__, text.c_str(), result);
   return result;
}
int getScrollPosition()
{
   int result = -1;
   RPC::GetTraceViewScrollPositionRequest request {};

   RPC::result<RPC::GetTraceViewScrollPositionReply> reply = g_rpc_client->invoke<RPC::GetTraceViewScrollPositionReply, RPC::GetTraceViewScrollPositionRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.position;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s position %d", __func__, result);
   return result;
}
bool setPosition(int position)
{
   RPC::SetTraceViewScrollPositionRequest request {};
   request.position = position;

   RPC::result<RPC::SetTraceViewScrollPositionReply> reply = g_rpc_client->invoke<RPC::SetTraceViewScrollPositionReply, RPC::SetTraceViewScrollPositionRequest>(request);
   UT_Log(TEST_FRAMEWORK, LOW, "%s position %d ready %u", __func__, position, reply.ready());
   return reply.ready();
}

} // TraceView

namespace UserButtons
{
bool setSettings(uint8_t id, const Dialogs::UserButtonDialog::Settings& settings)
{
   bool result = false;
   RPC::SetUserButtonSettingsRequest request {};
   request.id = id;
   request.settings = settings;

   RPC::result<RPC::SetUserButtonSettingsReply> reply = g_rpc_client->invoke<RPC::SetUserButtonSettingsReply, RPC::SetUserButtonSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.result;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u name %s", __func__, reply.ready(), id, request.settings.button_name.c_str());
   return result;

}
Dialogs::UserButtonDialog::Settings getSettings(uint8_t id)
{
   Dialogs::UserButtonDialog::Settings result = {};
   RPC::GetUserButtonSettingsRequest request {};
   request.id = id;

   RPC::result<RPC::GetUserButtonSettingsReply> reply = g_rpc_client->invoke<RPC::GetUserButtonSettingsReply, RPC::GetUserButtonSettingsRequest>(request);
   if (reply.ready())
   {
      result = reply.reply.settings;
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s ready %u id %u name %s", __func__, reply.ready(), id, result.button_name.c_str());
   return result;
}
} // UserButtons


} // TestFramework
