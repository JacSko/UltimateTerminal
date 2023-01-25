#include <memory>

#include "TestFramework.h"
#include "TestFrameworkCommon.h"
#include "TestFrameworkAPI.h"
#include "RPCClient.h"
#include "Logger.h"
#include "Settings.h"

static std::unique_ptr<RPC::RPCClient> g_rpc_client;

namespace TF
{


bool Connect()
{
   Settings::SettingsHandler::create();
   Settings::SettingsHandler::get()->start(CONFIG_FILE);
   LoggerEngine::get()->startFrontends();

   UT_Log(TEST_FRAMEWORK, LOW, "connecting TestFramework to RPC server");
   g_rpc_client = std::unique_ptr<RPC::RPCClient>(new RPC::RPCClient);
   UT_Assert(g_rpc_client);

   return g_rpc_client->connect(std::string(SERVER_IP_ADDRESS), SERVER_PORT);
}
void Disconnect()
{
   UT_Assert(g_rpc_client);
   g_rpc_client->disconnect();
   g_rpc_client.reset(nullptr);
}

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
bool checkFontColor(const std::string& name, uint32_t color)
{

}
bool checkEnabled(const std::string& name, bool enabled)
{

}
bool checkChecked(const std::string& name, bool checked)
{

}
bool checkCheckable(const std::string& name, bool checkable)
{

}
bool checkText(const std::string& name, const std::string& text)
{

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


}

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

}

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

}

}
