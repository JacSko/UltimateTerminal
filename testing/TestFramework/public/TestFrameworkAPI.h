#pragma once

#include <stdint.h>
#include "Serialize.hpp"

namespace RPC
{

enum class Command : uint8_t
{
   GetButtonState,
   ButtonClick,
   ButtonContextMenuClick
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




