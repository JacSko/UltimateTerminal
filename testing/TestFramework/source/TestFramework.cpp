#include <memory>
#include <thread>

#include "TestFramework.h"
#include "TestFrameworkCommon.h"
#include "TestFrameworkAPI.h"
#include "RPCClient.h"
#include "Logger.h"
#include "Settings.h"
#include "ApplicationExecutor.hpp"

static std::unique_ptr<RPC::RPCClient> g_rpc_client;
static TF::ApplicationExecutor g_test_application;
namespace TF
{


bool Connect()
{
   Settings::SettingsHandler::create();
   Settings::SettingsHandler::get()->start(CONFIG_FILE);
   LoggerEngine::get()->startFrontends();

   UT_Log(TEST_FRAMEWORK, LOW, "starting application %s", APPLICATION_PATH.c_str());
   UT_Assert(g_test_application.startApplication(APPLICATION_PATH, ""));
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

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

   UT_Assert(g_test_application.stopApplication());
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
   UT_Log(TEST_FRAMEWORK, LOW, "%s %s %u", __func__, name.c_str(), result);
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
      if (result && index != UINT32_MAX)
      {
         result = (std::distance(reply.reply.history.begin(), it) == index);
      }
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s %u %u", __func__, reply.ready(), result);
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

} // TraceFilters

namespace TerminalView
{
uint32_t countItems()
{
   uint32_t result = 0;
   RPC::GetTerminalViewContentRequest request {};

   RPC::result<RPC::GetTerminalViewContentReply> reply = g_rpc_client->invoke<RPC::GetTerminalViewContentReply, RPC::GetTerminalViewContentRequest>(request);
   if (reply.ready())
   {
      result = (uint32_t)reply.reply.content.size();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s size %u", __func__, result);
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
} // TerminalView

namespace TraceView
{
uint32_t countItems()
{
   uint32_t result = 0;
   RPC::GetTraceViewContentRequest request {};

   RPC::result<RPC::GetTraceViewContentReply> reply = g_rpc_client->invoke<RPC::GetTraceViewContentReply, RPC::GetTraceViewContentRequest>(request);
   if (reply.ready())
   {
      result = (uint32_t)reply.reply.content.size();
   }
   UT_Log(TEST_FRAMEWORK, LOW, "%s size %u", __func__, result);
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
} // TraceView


} // TestFramework
