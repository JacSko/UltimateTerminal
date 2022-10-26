#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>
#include "UserButtonDialog.h"
#include "Logger.h"

using namespace ::testing;

struct UserButtonDialogTests : public testing::Test
{
   void SetUp()
   {
   }
   void TearDown()
   {
   }
};

TEST_F(UserButtonDialogTests, test)
{
   /**
    * <b>scenario</b>: Simple command added by user, executed correctly <br>
    * <b>expected</b>: Thread not started, callback called.<br>
    * ************************************************
    */
   UserButtonDialog dialog;
}
