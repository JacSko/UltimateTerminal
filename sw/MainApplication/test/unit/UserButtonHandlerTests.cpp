#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "UserButtonHandler.h"
#include "Logger.h"
#include "QPushButtonMock.hpp"
#include "PersistenceHandlerFake.h"

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
      g_writer_mock = new WriterMock();
   }
   void TearDown()
   {
      delete g_writer_mock;
   }
};

TEST_F(ButtonCommandExecutorTests, executing_one_command)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
    * ************************************************
    */
   QPushButtonMock btn_mock;
   Persistence::PersistenceHandlerFake pers_fake;
   UserButtonHandler handler (btn_mock, nullptr, pers_fake, {});
}

}
