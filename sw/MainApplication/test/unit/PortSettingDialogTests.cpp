#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "PortSettingDialog.h"
#include "Logger.h"

/**
 * @file PortSettingDialogTests.cpp
 *
 * @brief
 *    Class to test behavior of PortSettingDialog class.
 *
 * @author Jacek Skowronek
 * @date   26/10/2022
 *
 */

using namespace ::testing;

struct PortSettingDialogFixture : public testing::Test
{
   void SetUp()
   {
      QtCoreMock_init();
      QtWidgetsMock_init();
   }
   void TearDown()
   {
      QtCoreMock_deinit();
      QtWidgetsMock_deinit();
   }
};

TEST_F(PortSettingDialogFixture, current_settings_presentation)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Dialog created and show to the user <br>
    * <b>expected</b>: <br>
    *       Current settings should be correctly presented to user.<br>
    *       User changed one of the fields, accepted the dialog.<br>
    *       New settings should be returned. <br>
    * ************************************************
    */
}
