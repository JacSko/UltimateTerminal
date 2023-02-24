#pragma once
#include <gmock/gmock.h>
#include "Logger.h"
#include "MessageDialog.h"

struct MessageDialogMock
{

   MOCK_METHOD4(show,void(Dialogs::MessageDialog::Icon icon, const std::string& window_title, const std::string& text, QPalette palette));
};

MessageDialogMock* m_message_box_mock;

void MessageDialogMock_init()
{
   if (!m_message_box_mock)
   {
      m_message_box_mock = new MessageDialogMock();
   }
}
void MessageDialogMock_deinit()
{
   if (m_message_box_mock)
   {
      delete m_message_box_mock;
      m_message_box_mock = nullptr;
   }
}
MessageDialogMock* MessageDialogMock_get()
{
   UT_Assert(m_message_box_mock && "Create MessageDialogMock mock first!");
   return m_message_box_mock;

}

namespace Dialogs
{

void MessageDialog::show(Icon icon, const std::string& window_title, const std::string& text, QPalette palette)
{
   m_message_box_mock->show(icon, window_title, text, palette);
}

}
