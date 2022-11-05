#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "QtWidgets/QtWidgetsMock.h"

#include "MainApplication.h"

using namespace ::testing;


struct MainApplicationFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
   }
   void TearDown()
   {
      QtWidgetsMock_deinit();
      QtCoreMock_deinit();
   }

   std::unique_ptr<MainApplication> m_test_subject;
};


TEST_F(MainApplicationFixture, test)
{
   /**
    * <b>scenario</b>: User requested to connect, but the port settings are default <br>
    * <b>expected</b>: Connection to serial port shall be requested. <br>
    *                  Button color shall be changed to green. <br>
    * ************************************************
    */
}

