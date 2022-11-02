#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "UserButtonHandler.h"
#include "UserButtonDialogMock.h"
#include "Logger.h"
#include <QtWidgets/QPushButton>

namespace GUI
{

using namespace ::testing;

struct WriterMock
{
   MOCK_METHOD1(write, bool(const std::string&));
};

WriterMock* g_writer_mock;

auto WriterFunction = [](const std::string& str)->bool
      {
         UT_Assert(g_writer_mock && "create writer mock");
         return g_writer_mock->write(str);
      };

struct ButtonCommandExecutorTests : public testing::Test
{
   void SetUp()
   {
      QtWidgetsMock_init();
      g_writer_mock = new WriterMock();
   }
   void TearDown()
   {
      delete g_writer_mock;
      QtWidgetsMock_deinit();
   }
};

void UserButtonHandler::commandsFinished()
{

}

TEST_F(ButtonCommandExecutorTests, executing_one_command)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
    * ************************************************
    */
   QPushButton btn_mock;
   Persistence::PersistenceHandler pers_fake;
   UserButtonHandler handler (&btn_mock, nullptr, pers_fake, {});
}

}
