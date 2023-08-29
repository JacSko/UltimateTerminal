#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "Serialize.hpp"
#include "PortSettingDialog.h"
#include "TraceFilterSettingDialog.h"
#include "UserButtonDialog.h"
#include "MessageDialog.h"

namespace RPC
{

enum class Command : uint8_t
{
   GetButtonState,
   ButtonClick,
   ButtonContextMenuClick,
   SetCommand,
   GetCommand,
   GetPortLabel,
   SetLineEnding,
   GetLineEnding,
   GetAllLineEndings,
   SetTargetPort,
   GetTargetPort,
   GetAllTargetPorts,
   SetTraceFilter,
   GetTraceFilterState,
   GetCommandHistory,
   GetTerminalViewContent,
   GetTerminalViewCount,
   GetTraceViewContent,
   GetTraceViewCount,
   GetPortSettings,
   SetPortSettings,
   SetTraceFilterSettings,
   GetTraceFilterSettings,
   SetUserButtonSettings,
   GetUserButtonSettings,
   GetMessageBox,
   ResetMessageBox,
   GetLoggingPath,
   SetLoggingPath,
   GetTerminalScrollPosition,
   GetTraceViewScrollPosition,
   SetTerminalScrollPosition,
   SetTraceViewScrollPosition,
   CloseApplication,
};

struct GetButtonStateRequest
{
   Command cmd = Command::GetButtonState;
   std::string button_name;
};

struct GetButtonStateReply
{
   Command cmd = Command::GetButtonState;
   std::string button_name;
   bool checked;
   bool checkable;
   bool enabled;
   std::string text;
   uint32_t background_color;
   uint32_t font_color;
};

struct ButtonClickRequest
{
   Command cmd = Command::ButtonClick;
   std::string button_name;
};

struct ButtonClickReply
{
   Command cmd = Command::ButtonClick;
   bool clicked;
};

struct ButtonContextMenuClickRequest
{
   Command cmd = Command::ButtonContextMenuClick;
   std::string button_name;
};

struct ButtonContextMenuClickReply
{
   Command cmd = Command::ButtonContextMenuClick;
   bool clicked;
};

struct SetCommandRequest
{
   Command cmd = Command::SetCommand;
   std::string command;
};
struct SetCommandReply
{
   Command cmd = Command::SetCommand;
   bool reply;
};

struct GetCommandRequest
{
   Command cmd = Command::GetCommand;
};

struct GetCommandReply
{
   Command cmd = Command::GetCommand;
   std::string command;
};

struct GetPortLabelRequest
{
   Command cmd = Command::GetPortLabel;
   uint8_t id;
};
struct GetPortLabelReply
{
   Command cmd = Command::GetPortLabel;
   uint8_t id;
   std::string stylesheet;
   std::string text;
   std::string throughput;
};
struct SetLineEndingRequest
{
   Command cmd = Command::SetLineEnding;
   std::string lineending;
};
struct SetLineEndingReply
{
   Command cmd = Command::SetLineEnding;
   bool result;
};
struct GetLineEndingRequest
{
   Command cmd = Command::GetLineEnding;
};
struct GetLineEndingReply
{
   Command cmd = Command::GetLineEnding;
   std::string lineending;
};
struct GetAllLineEndingsRequest
{
   Command cmd = Command::GetAllLineEndings;
};
struct GetAllLineEndingsReply
{
   Command cmd = Command::GetAllLineEndings;
   std::vector<std::string> lineendings;
};
struct SetTargetPortRequest
{
   Command cmd = Command::SetTargetPort;
   std::string port_name;
};
struct SetTargetPortReply
{
   Command cmd = Command::SetTargetPort;
   bool result;
};
struct GetTargetPortRequest
{
   Command cmd = Command::GetTargetPort;
};
struct GetTargetPortReply
{
   Command cmd = Command::GetTargetPort;
   std::string port_name;
};
struct GetAllTargetPortsRequest
{
   Command cmd = Command::GetAllTargetPorts;
};
struct GetAllTargetPortsReply
{
   Command cmd = Command::GetAllTargetPorts;
   std::vector<std::string> port_names;
};
struct GetTraceFilterStateRequest
{
   Command cmd = Command::GetTraceFilterState;
   std::string filter_name;
};
struct GetTraceFilterStateReply
{
   Command cmd = Command::GetTraceFilterState;
   std::string filter_name;
   std::string text;
   uint32_t background_color;
   uint32_t font_color;
   bool enabled;
};
struct SetTraceFilterRequest
{
   Command cmd = Command::SetTraceFilter;
   std::string filter_name;
   std::string text;
};
struct SetTraceFilterReply
{
   Command cmd = Command::SetTraceFilter;
   bool result;
};
struct GetCommandHistoryRequest
{
   Command cmd = Command::GetCommandHistory;
};
struct GetCommandHistoryReply
{
   Command cmd = Command::GetCommandHistory;
   std::vector<std::string> history;
};
struct GetTerminalViewContentRequest
{
   Command cmd = Command::GetTerminalViewContent;
};
struct GetTerminalViewCountRequest
{
   Command cmd = Command::GetTerminalViewCount;
};
struct GetTraceViewContentRequest
{
   Command cmd = Command::GetTraceViewContent;
};
struct GetTraceViewCountRequest
{
   Command cmd = Command::GetTraceViewCount;
};

struct ViewItem
{
   std::string text;
   uint32_t background_color;
   uint32_t font_color;
};
struct GetTerminalViewContentReply
{
   Command cmd = Command::GetTerminalViewContent;
   std::vector<ViewItem> content;
};
struct GetTerminalViewCountReply
{
   Command cmd = Command::GetTerminalViewCount;
   uint32_t count;
};
struct GetTraceViewContentReply
{
   Command cmd = Command::GetTraceViewContent;
   std::vector<ViewItem> content;
};
struct GetTraceViewCountReply
{
   Command cmd = Command::GetTraceViewCount;
   uint32_t count;
};
struct GetPortSettingsRequest
{
   Command cmd = Command::GetPortSettings;
   uint8_t port_id;
};
struct GetPortSettingsReply
{
   Command cmd = Command::GetPortSettings;
   uint8_t port_id;
   Dialogs::PortSettingDialog::Settings settings;
};
struct SetPortSettingsRequest
{
   Command cmd = Command::SetPortSettings;
   uint8_t port_id;
   Dialogs::PortSettingDialog::Settings settings;
};
struct SetPortSettingsReply
{
   Command cmd = Command::SetPortSettings;
   bool result;
};
struct GetTraceFilterSettingsRequest
{
   Command cmd = Command::GetTraceFilterSettings;
   uint8_t id;
};
struct GetTraceFilterSettingsReply
{
   Command cmd = Command::GetTraceFilterSettings;
   uint8_t id;
   Dialogs::TraceFilterSettingDialog::Settings settings;
};
struct SetTraceFilterSettingsRequest
{
   Command cmd = Command::SetTraceFilterSettings;
   uint8_t id;
   Dialogs::TraceFilterSettingDialog::Settings settings;
};
struct SetTraceFilterSettingsReply
{
   Command cmd = Command::GetTraceFilterSettings;
   bool result;
};
struct GetUserButtonSettingsRequest
{
   Command cmd = Command::GetUserButtonSettings;
   uint8_t id;
};
struct GetUserButtonSettingsReply
{
   Command cmd = Command::GetUserButtonSettings;
   uint8_t id;
   Dialogs::UserButtonDialog::Settings settings;
};
struct SetUserButtonSettingsRequest
{
   Command cmd = Command::SetUserButtonSettings;
   uint8_t id;
   Dialogs::UserButtonDialog::Settings settings;
};
struct SetUserButtonSettingsReply
{
   Command cmd = Command::SetUserButtonSettings;
   bool result;
};
struct GetMessageBoxRequest
{
   Command cmd = Command::GetMessageBox;
};
struct GetMessageBoxReply
{
   Command cmd = Command::GetMessageBox;
   std::string title;
   Dialogs::MessageDialog::Icon icon;
   std::string text;
};
struct ResetMessageBoxRequest
{
   Command cmd = Command::ResetMessageBox;
};
struct ResetMessageBoxReply
{
   Command cmd = Command::ResetMessageBox;
   bool result;
};
struct GetLoggingPathRequest
{
   Command cmd = Command::GetLoggingPath;
};
struct GetLoggingPathReply
{
   Command cmd = Command::GetLoggingPath;
   std::string path;
};
struct SetLoggingPathRequest
{
   Command cmd = Command::SetLoggingPath;
   std::string path;
};
struct SetLoggingPathReply
{
   Command cmd = Command::SetLoggingPath;
   bool result;
};
struct GetTerminalScrollPositionRequest
{
   Command cmd = Command::GetTerminalScrollPosition;
};
struct GetTerminalScrollPositionReply
{
   Command cmd = Command::GetTerminalScrollPosition;
   int position;
};
struct GetTraceViewScrollPositionRequest
{
   Command cmd = Command::GetTraceViewScrollPosition;
};
struct GetTraceViewScrollPositionReply
{
   Command cmd = Command::GetTraceViewScrollPosition;
   int position;
};
struct SetTerminalScrollPositionRequest
{
   Command cmd = Command::SetTerminalScrollPosition;
   int position;
};
struct SetTerminalScrollPositionReply
{
   Command cmd = Command::SetTerminalScrollPosition;
   bool result;
};
struct SetTraceViewScrollPositionRequest
{
   Command cmd = Command::SetTraceViewScrollPosition;
   int position;
};
struct SetTraceViewScrollPositionReply
{
   Command cmd = Command::SetTraceViewScrollPosition;
   bool result;
};
struct CloseApplicationRequest
{
   Command cmd = Command::CloseApplication;
};
struct CloseApplicationReply
{
   Command cmd = Command::CloseApplication;
};
}

static void serialize(std::vector<uint8_t>& buffer, RPC::GetButtonStateRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.button_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetButtonStateReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.button_name);
   ::serialize(buffer, item.checked);
   ::serialize(buffer, item.checkable);
   ::serialize(buffer, item.enabled);
   ::serialize(buffer, item.text);
   ::serialize(buffer, item.background_color);
   ::serialize(buffer, item.font_color);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ButtonClickRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.button_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ButtonClickReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.clicked);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ButtonContextMenuClickRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.button_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ButtonContextMenuClickReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.clicked);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetCommandRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.command);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetCommandReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.reply);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetCommandRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetCommandReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.command);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetPortLabelRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetPortLabelReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
   ::serialize(buffer, item.stylesheet);
   ::serialize(buffer, item.text);
   ::serialize(buffer, item.throughput);

}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetLineEndingRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.lineending);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetLineEndingReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetLineEndingRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetLineEndingReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.lineending);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetAllLineEndingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetAllLineEndingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (uint32_t)item.lineendings.size());
   for (auto& ending : item.lineendings)
   {
      ::serialize(buffer, ending);
   }
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTargetPortRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.port_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTargetPortReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTargetPortRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTargetPortReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.port_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetAllTargetPortsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetAllTargetPortsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (uint32_t)item.port_names.size());
   for (auto& port_name : item.port_names)
   {
      ::serialize(buffer, port_name);
   }
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceFilterRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.filter_name);
   ::serialize(buffer, item.text);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceFilterReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceFilterStateRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.filter_name);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceFilterStateReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.filter_name);
   ::serialize(buffer, item.text);
   ::serialize(buffer, item.background_color);
   ::serialize(buffer, item.font_color);
   ::serialize(buffer, item.enabled);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetCommandHistoryRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetCommandHistoryReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (uint32_t)item.history.size());
   for (auto& history_item : item.history)
   {
      ::serialize(buffer, history_item);
   }
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalViewContentRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalViewCountRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalViewCountReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.count);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewContentRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewCountRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewCountReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.count);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalViewContentReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (uint32_t)item.content.size());
   for (auto& content_item : item.content)
   {
      ::serialize(buffer, content_item.text);
      ::serialize(buffer, content_item.background_color);
      ::serialize(buffer, content_item.font_color);
   }
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewContentReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (uint32_t)item.content.size());
   for (auto& content_item : item.content)
   {
      ::serialize(buffer, content_item.text);
      ::serialize(buffer, content_item.background_color);
      ::serialize(buffer, content_item.font_color);
   }
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetPortSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.port_id);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetPortSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.port_id);
   ::serialize(buffer, item.settings.font_color);
   ::serialize(buffer, item.settings.ip_address);
   ::serialize(buffer, item.settings.port);
   ::serialize(buffer, item.settings.port_id);
   ::serialize(buffer, item.settings.port_name);
   ::serialize(buffer, item.settings.serialSettings.baudRate.toName());
   ::serialize(buffer, item.settings.serialSettings.dataBits.toName());
   ::serialize(buffer, item.settings.serialSettings.device);
   ::serialize(buffer, (uint32_t)item.settings.serialSettings.mode);
   ::serialize(buffer, item.settings.serialSettings.parityBits.toName());
   ::serialize(buffer, item.settings.serialSettings.stopBits.toName());
   ::serialize(buffer, item.settings.trace_color);
   ::serialize(buffer, item.settings.type.toName());
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetPortSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.port_id);
   ::serialize(buffer, item.settings.font_color);
   ::serialize(buffer, item.settings.ip_address);
   ::serialize(buffer, item.settings.port);
   ::serialize(buffer, item.settings.port_id);
   ::serialize(buffer, item.settings.port_name);
   ::serialize(buffer, item.settings.serialSettings.baudRate.toName());
   ::serialize(buffer, item.settings.serialSettings.dataBits.toName());
   ::serialize(buffer, item.settings.serialSettings.device);
   ::serialize(buffer, (uint32_t)item.settings.serialSettings.mode);
   ::serialize(buffer, item.settings.serialSettings.parityBits.toName());
   ::serialize(buffer, item.settings.serialSettings.stopBits.toName());
   ::serialize(buffer, item.settings.trace_color);
   ::serialize(buffer, item.settings.type.toName());
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetPortSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceFilterSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceFilterSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
   ::serialize(buffer, item.settings.background);
   ::serialize(buffer, item.settings.font);
   ::serialize(buffer, item.settings.id);
   ::serialize(buffer, item.settings.regex);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceFilterSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
   ::serialize(buffer, item.settings.background);
   ::serialize(buffer, item.settings.font);
   ::serialize(buffer, item.settings.id);
   ::serialize(buffer, item.settings.regex);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceFilterSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetUserButtonSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetUserButtonSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
   ::serialize(buffer, item.settings.button_name);
   ::serialize(buffer, item.settings.id);
   ::serialize(buffer, item.settings.raw_commands);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetUserButtonSettingsRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.id);
   ::serialize(buffer, item.settings.button_name);
   ::serialize(buffer, item.settings.id);
   ::serialize(buffer, item.settings.raw_commands);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetUserButtonSettingsReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetMessageBoxRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetMessageBoxReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.title);
   ::serialize(buffer, (uint8_t)item.icon);
   ::serialize(buffer, item.text);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ResetMessageBoxRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::ResetMessageBoxReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetLoggingPathRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetLoggingPathReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.path);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetLoggingPathRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.path);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetLoggingPathReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalScrollPositionRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTerminalScrollPositionReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.position);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewScrollPositionRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::GetTraceViewScrollPositionReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.position);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTerminalScrollPositionRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (int)item.position);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTerminalScrollPositionReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceViewScrollPositionRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, (int)item.position);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::SetTraceViewScrollPositionReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
   ::serialize(buffer, item.result);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::CloseApplicationRequest item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void serialize(std::vector<uint8_t>& buffer, RPC::CloseApplicationReply item)
{
   ::serialize(buffer, (uint8_t)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetButtonStateRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (std::string&)item.button_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetButtonStateReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.button_name);
   ::deserialize(buffer, offset, item.checked);
   ::deserialize(buffer, offset, item.checkable);
   ::deserialize(buffer, offset, item.enabled);
   ::deserialize(buffer, offset, item.text);
   ::deserialize(buffer, offset, item.background_color);
   ::deserialize(buffer, offset, item.font_color);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ButtonClickRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.button_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ButtonClickReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.clicked);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ButtonContextMenuClickRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.button_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ButtonContextMenuClickReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.clicked);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetCommandRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.command);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetCommandReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.reply);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetCommandRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetCommandReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.command);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetPortLabelRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetPortLabelReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
   ::deserialize(buffer, offset, item.stylesheet);
   ::deserialize(buffer, offset, item.text);
   ::deserialize(buffer, offset, item.throughput);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetLineEndingRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetLineEndingReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.lineending);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetLineEndingRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.lineending);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetLineEndingReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetAllLineEndingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetAllLineEndingsReply& item)
{
   uint32_t offset = 0;
   uint32_t lineendings_count = 0;

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (uint32_t&)lineendings_count);
   for (uint32_t i = 0; i < lineendings_count; i++)
   {
      std::string line_ending = "";
      ::deserialize(buffer, offset, line_ending);
      item.lineendings.push_back(line_ending);
   }
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTargetPortRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTargetPortReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.port_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTargetPortRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.port_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTargetPortReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetAllTargetPortsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetAllTargetPortsReply& item)
{
   uint32_t offset = 0;
   uint32_t ports_count = 0;

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (uint32_t&)ports_count);
   for (uint32_t i = 0; i < ports_count; i++)
   {
      std::string name = "";
      ::deserialize(buffer, offset, name);
      item.port_names.push_back(name);
   }
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceFilterRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.filter_name);
   ::deserialize(buffer, offset, item.text);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceFilterReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceFilterStateRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.filter_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceFilterStateReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.filter_name);
   ::deserialize(buffer, offset, item.text);
   ::deserialize(buffer, offset, item.background_color);
   ::deserialize(buffer, offset, item.font_color);
   ::deserialize(buffer, offset, item.enabled);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetCommandHistoryRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetCommandHistoryReply& item)
{
   uint32_t offset = 0;
   uint32_t commands_count = 0;

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (uint32_t&)commands_count);
   for (uint32_t i = 0; i < commands_count; i++)
   {
      std::string command = "";
      ::deserialize(buffer, offset, command);
      item.history.push_back(command);
   }
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalViewContentRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalViewCountRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalViewCountReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.count);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewContentRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewCountRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewCountReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.count);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalViewContentReply& item)
{
   uint32_t offset = 0;
   uint32_t terminal_count = 0;

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (uint32_t&)terminal_count);
   for (uint32_t i = 0; i < terminal_count; i++)
   {
      RPC::ViewItem terminal_item = {};
      ::deserialize(buffer, offset, terminal_item.text);
      ::deserialize(buffer, offset, terminal_item.background_color);
      ::deserialize(buffer, offset, terminal_item.font_color);
      item.content.push_back(terminal_item);
   }
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewContentReply& item)
{
   uint32_t offset = 0;
   uint32_t terminal_count = 0;

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (uint32_t&)terminal_count);
   for (uint32_t i = 0; i < terminal_count; i++)
   {
      RPC::ViewItem terminal_item = {};
      ::deserialize(buffer, offset, terminal_item.text);
      ::deserialize(buffer, offset, terminal_item.background_color);
      ::deserialize(buffer, offset, terminal_item.font_color);
      item.content.push_back(terminal_item);
   }
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetPortSettingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.port_id);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetPortSettingsReply& item)
{
   uint32_t offset = 0;

   std::string baudrate_name = "";
   std::string databits_name = "";
   std::string paritybits_name = "";
   std::string stopbits_name = "";
   std::string type_name = "";

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.port_id);
   ::deserialize(buffer, offset, item.settings.font_color);
   ::deserialize(buffer, offset, item.settings.ip_address);
   ::deserialize(buffer, offset, item.settings.port);
   ::deserialize(buffer, offset, item.settings.port_id);
   ::deserialize(buffer, offset, item.settings.port_name);
   ::deserialize(buffer, offset, baudrate_name);
   ::deserialize(buffer, offset, databits_name);
   ::deserialize(buffer, offset, item.settings.serialSettings.device);
   ::deserialize(buffer, offset, (uint32_t&)item.settings.serialSettings.mode);
   ::deserialize(buffer, offset, paritybits_name);
   ::deserialize(buffer, offset, stopbits_name);
   ::deserialize(buffer, offset, item.settings.trace_color);
   ::deserialize(buffer, offset, type_name);

   item.settings.serialSettings.baudRate.fromName(baudrate_name);
   item.settings.serialSettings.dataBits.fromName(databits_name);
   item.settings.serialSettings.parityBits.fromName(paritybits_name);
   item.settings.serialSettings.stopBits.fromName(stopbits_name);
   item.settings.type.fromName(type_name);
}

static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetPortSettingsRequest& item)
{
   uint32_t offset = 0;
   std::string baudrate_name = "";
   std::string databits_name = "";
   std::string paritybits_name = "";
   std::string stopbits_name = "";
   std::string type_name = "";

   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.port_id);
   ::deserialize(buffer, offset, item.settings.font_color);
   ::deserialize(buffer, offset, item.settings.ip_address);
   ::deserialize(buffer, offset, item.settings.port);
   ::deserialize(buffer, offset, item.settings.port_id);
   ::deserialize(buffer, offset, item.settings.port_name);
   ::deserialize(buffer, offset, baudrate_name);
   ::deserialize(buffer, offset, databits_name);
   ::deserialize(buffer, offset, item.settings.serialSettings.device);
   ::deserialize(buffer, offset, (uint32_t&)item.settings.serialSettings.mode);
   ::deserialize(buffer, offset, paritybits_name);
   ::deserialize(buffer, offset, stopbits_name);
   ::deserialize(buffer, offset, item.settings.trace_color);
   ::deserialize(buffer, offset, type_name);

   item.settings.serialSettings.baudRate.fromName(baudrate_name);
   item.settings.serialSettings.dataBits.fromName(databits_name);
   item.settings.serialSettings.parityBits.fromName(paritybits_name);
   item.settings.serialSettings.stopBits.fromName(stopbits_name);
   item.settings.type.fromName(type_name);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetPortSettingsReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceFilterSettingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceFilterSettingsReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
   ::deserialize(buffer, offset, item.settings.background);
   ::deserialize(buffer, offset, item.settings.font);
   ::deserialize(buffer, offset, item.settings.id);
   ::deserialize(buffer, offset, item.settings.regex);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceFilterSettingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
   ::deserialize(buffer, offset, item.settings.background);
   ::deserialize(buffer, offset, item.settings.font);
   ::deserialize(buffer, offset, item.settings.id);
   ::deserialize(buffer, offset, item.settings.regex);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceFilterSettingsReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetUserButtonSettingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetUserButtonSettingsReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
   ::deserialize(buffer, offset, item.settings.button_name);
   ::deserialize(buffer, offset, item.settings.id);
   ::deserialize(buffer, offset, item.settings.raw_commands);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetUserButtonSettingsRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.id);
   ::deserialize(buffer, offset, item.settings.button_name);
   ::deserialize(buffer, offset, item.settings.id);
   ::deserialize(buffer, offset, item.settings.raw_commands);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetUserButtonSettingsReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetMessageBoxRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetMessageBoxReply& item)
{
   uint32_t offset = 0;
   uint8_t icon = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.title);
   ::deserialize(buffer, offset, icon);
   ::deserialize(buffer, offset, item.text);
   item.icon = static_cast<Dialogs::MessageDialog::Icon>(icon);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ResetMessageBoxRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::ResetMessageBoxReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetLoggingPathRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetLoggingPathReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.path);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetLoggingPathRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.path);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetLoggingPathReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalScrollPositionRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTerminalScrollPositionReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (int&)item.position);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewScrollPositionRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::GetTraceViewScrollPositionReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (int&)item.position);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTerminalScrollPositionRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (int&)item.position);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTerminalScrollPositionReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceViewScrollPositionRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, (int&)item.position);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::SetTraceViewScrollPositionReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
   ::deserialize(buffer, offset, item.result);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::CloseApplicationRequest& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}
static void deserialize(const std::vector<uint8_t>& buffer, RPC::CloseApplicationReply& item)
{
   uint32_t offset = 0;
   ::deserialize(buffer, offset, (uint8_t&)item.cmd);
}

