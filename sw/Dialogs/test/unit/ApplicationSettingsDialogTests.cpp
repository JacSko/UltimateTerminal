#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <optional>

#include "QtWidgets/QtWidgetsMock.h"
#include "QtCore/QtCoreMock.h"

#include "PortSettingDialog.h"
#include "Logger.h"
#include "SerialDriverMock.h"

/**
 * @file ApplicationSettingsDialogTests.cpp
 *
 * @brief
 *    Class to test behavior of ApplicationSettingsDialog class.
 *
 * @author Jacek Skowronek
 * @date   18-11-2022
 *
 */

using namespace ::testing;

namespace Dialogs
{

struct ApplicationSettingsDialogFixture : public testing::Test
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

TEST_F(ApplicationSettingsDialogFixture, some_test)
{
}

}
