#include <memory>

#include "TestFramework.h"
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

   return g_rpc_client->connect("127.0.0.1", 8888);
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
}
