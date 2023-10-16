#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "UserButtonsTab.h"
#include "GUIControllerMock.h"
#include "UserButtonMock.h"
#include "TabNameDialogMock.h"

namespace MainApplication
{

using namespace ::testing;

struct UserButtonsTabFixture : public testing::Test
{
   UserButtonsTabFixture():
   test_controller(nullptr)
   {}

   void SetUp()
   {
      GUIControllerMock_init();
      TabNameDialogMock_init();
      EXPECT_CALL(*GUIControllerMock_get(), setTabName(TEST_TAB_ID,TEST_TAB_NAME));
      EXPECT_CALL(*GUIControllerMock_get(), subscribeForTabNameChangeRequest(TEST_TAB_ID, _)).
                                                                  WillOnce(SaveArg<1>(&tab_name_listener));
      m_test_subject.reset(new UserButtonsTab(test_controller, TEST_TAB_ID, TEST_BUTTONS_COUNT,
    		  	  	  	  	  	  	  	  	  	  	 fake_persistence, {}));
      ASSERT_TRUE(tab_name_listener);
   }
   void TearDown()
   {
      EXPECT_CALL(*GUIControllerMock_get(), unsubscribeFromTabNameChangeRequest(TEST_TAB_ID, _));

      m_test_subject.reset(nullptr);
      GUIControllerMock_deinit();
      TabNameDialogMock_deinit();
   }

   const uint32_t TEST_TAB_ID = 1;
   const uint32_t TEST_BUTTONS_COUNT = 10;
   const std::string TEST_TAB_NAME = "TAB" + std::to_string(TEST_TAB_ID);
   std::unique_ptr<UserButtonsTab> m_test_subject;
   GUIController::GUIController test_controller;
   Utilities::Persistence::Persistence fake_persistence;
   GUIController::TabNameChangeRequestListener* tab_name_listener;
};

TEST_F(UserButtonsTabFixture, tabNameChange)
{
   const std::string NEW_TAB_NAME = "NEW TAB NAME";

   /**
    * <b>scenario</b>: Tab name change was requested from GUI, user rejected dialog <br>
    * <b>expected</b>: Dialog was opened to allow user to enter the new name. <br>
    *                  No tab name notification sent to GUIController .<br>
    * ************************************************
    */
   EXPECT_CALL(*TabNameDialogMock_get(), showDialog(_, TEST_TAB_NAME)).WillOnce(
                                                                    Return(std::optional<std::string>{}));
   EXPECT_CALL(*GUIControllerMock_get(), setTabName(_,_)).Times(0);
   tab_name_listener->onTabNameChangeRequest();

   /**
    * <b>scenario</b>: Tab name change was requested from GUI, user accepted dialog <br>
    * <b>expected</b>: Dialog was opened to allow user to enter the new name. <br>
    *                  Tab name notification sent to GUIController .<br>
    * ************************************************
    */
   EXPECT_CALL(*TabNameDialogMock_get(), showDialog(_, TEST_TAB_NAME)).WillOnce(
                                                        Return(std::optional<std::string>(NEW_TAB_NAME)));
   EXPECT_CALL(*GUIControllerMock_get(), setTabName(TEST_TAB_ID,NEW_TAB_NAME));
   tab_name_listener->onTabNameChangeRequest();

}

TEST_F(UserButtonsTabFixture, persistenceWriteAndRead)
{
   Utilities::Persistence::PersistenceListener::PersistenceItems data_buffer;
   /**
    * <b>scenario</b>: Persistence write requested <br>
    * <b>expected</b>: Tab name stored in persistence. <br>
    * ************************************************
    */
   ((Utilities::Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceWrite(data_buffer);
   EXPECT_FALSE(data_buffer.empty());

   /**
    * <b>scenario</b>: Persistence restore requested <br>
    * <b>expected</b>: Tab name notified to GUIController. <br>
    * ************************************************
    */

   EXPECT_CALL(*GUIControllerMock_get(), setTabName(TEST_TAB_ID,TEST_TAB_NAME));
   ((Utilities::Persistence::PersistenceListener*)m_test_subject.get())->onPersistenceRead(data_buffer);
}


}
