#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "TestFramework.h"

struct TestFixture : public testing::Test
{
   static void SetUpTestSuite()
   {
      ASSERT_TRUE(TF::Connect());
   }
   static void TearDownTestSuite()
   {
      TF::Disconnect();
   }
};

TEST_F(TestFixture, test)
{
   EXPECT_EQ(TF::Buttons::getBackgroundColor("scrollButton"), 0x112233);
}
