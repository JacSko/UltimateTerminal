#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "MessageBox.h"

struct MessageBoxMock
{

   MOCK_METHOD4(show,void(Dialogs::MessageBox::Icon icon, const std::string& window_title, const std::string& text, QPalette palette));
};

MessageBoxMock* m_message_box_mock;

void MessageBoxMock_init()
{
   if (!m_message_box_mock)
   {
      m_message_box_mock = new MessageBoxMock();
   }
}
void MessageBoxMock_deinit()
{
   if (m_message_box_mock)
   {
      delete m_message_box_mock;
      m_message_box_mock = nullptr;
   }
}
MessageBoxMock* MessageBoxMock_get()
{
   UT_Assert(m_message_box_mock && "Create MessageBoxMock mock first!");
   return m_message_box_mock;

}

namespace Dialogs
{

void MessageBox::show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette)
{
   m_message_box_mock->show(icon, window_title, text, palette);
}

}
