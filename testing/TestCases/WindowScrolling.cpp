#include "gtest/gtest.h"
#include "TestFramework.h"

using namespace Dialogs;

struct WindowsScrollingTests : public testing::Test
{
   const uint32_t TEST_SOCKET_PORT = 2222;
   const std::string TEST_IP_ADDRESS = "127.0.0.1";
   static constexpr uint32_t TEST_TRACES_COUNT = 500;
   const uint8_t PORT_ID = 0;
   const std::string PORT_BUTTON_NAME = "portButton_" + std::to_string(PORT_ID);
   const std::string NEW_PORT_NAME = "NEW_NAME" + std::to_string(PORT_ID);

   static void SetUpTestSuite()
   {
      TF::Init();
   }
   static void TearDownTestSuite()
   {
      TF::Deinit();
   }
   virtual void SetUp()
   {
      ASSERT_TRUE(TF::StartTestSubject());
      TF::wait(500);
      ASSERT_TRUE(TF::Connect());
      TF::wait(1000);
      TF::BeginTest();
      openPort();
   }
   virtual void TearDown()
   {
      closePort();
      TF::FinishTest();
      TF::StopTestSubject();
      TF::Disconnect();
   }

   void openPort()
   {
      /* start socket server on TEST_SOCKET_PORT */
      EXPECT_TRUE(TF::Socket::startServer(TEST_SOCKET_PORT));
      TF::wait(100);

      PortSettingDialog::Settings port_settings;
      port_settings.port_name = NEW_PORT_NAME;
      port_settings.type = PortSettingDialog::PortType::ETHERNET;
      port_settings.ip_address = TEST_IP_ADDRESS;
      port_settings.port = TEST_SOCKET_PORT;
      port_settings.port_id = PORT_ID;
      EXPECT_TRUE(TF::Ports::setPortSettings(PORT_ID, port_settings));
      EXPECT_TRUE(TF::Buttons::simulateContextMenuClick(PORT_BUTTON_NAME));
      TF::wait(100);

      /* open port by clicking on button */
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(500);
   }

   void closePort()
   {
      EXPECT_TRUE(TF::Buttons::simulateButtonClick(PORT_BUTTON_NAME));
      TF::wait(100);
      EXPECT_TRUE(TF::Socket::stopServer(TEST_SOCKET_PORT));
      TF::wait(100);
   }

   void sendDataChunk()
   {
      /* start generating data on socket server */
      for (uint32_t i = 0; i < TEST_TRACES_COUNT; i++)
      {
         EXPECT_TRUE(TF::Socket::sendMessage(TEST_SOCKET_PORT, "TEST MESSAGE " + std::to_string(i) + '\n'));
         TF::wait(10);
      }
      TF::wait(100);
   }
};


TEST_F(WindowsScrollingTests, tracesAdded_terminalWindowScrolled)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Many traces received. <br>
    * <b>expected</b>: <br>
    *       Terminal gets scrolled on every new line . <br>
    * ************************************************
    */

   ASSERT_EQ(TF::TerminalView::getScrollPosition(), 0);
   sendDataChunk();
   EXPECT_TRUE(TF::TerminalView::getScrollPosition() != 0);
}

TEST_F(WindowsScrollingTests, scrollingStoppedThenResumedByButton)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Many traces received, but user started scrolling up and hit BOTTOM button after some time. <br>
    * <b>expected</b>: <br>
    *       Terminal gets scrolled on every new line, when user scrolled up the scrolling was paused. After hitting BOTTOM button, traces get scrolled again. <br>
    * ************************************************
    */
   ASSERT_EQ(TF::TerminalView::getScrollPosition(), 0);
   sendDataChunk();
   int scrollPosition = TF::TerminalView::getScrollPosition();
   EXPECT_TRUE(scrollPosition != 0);

   /* pause scrolling by setting scrollBar position to middle */
   EXPECT_TRUE(TF::TerminalView::setPosition(scrollPosition / 2));
   TF::wait(100);

   sendDataChunk();

   int newScrollPosition = TF::TerminalView::getScrollPosition();
   EXPECT_TRUE(newScrollPosition == (scrollPosition / 2));

   /* hit BOTTOM button to force scrolling to bottom */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("bottomButton"));
   TF::wait(100);

   int bottomScrollPosition = TF::TerminalView::getScrollPosition();
   EXPECT_TRUE(bottomScrollPosition > newScrollPosition);

   sendDataChunk();

   int finalScrollPosition = TF::TerminalView::getScrollPosition();
   EXPECT_TRUE(finalScrollPosition > bottomScrollPosition);

}

TEST_F(WindowsScrollingTests, tracesAddedToTraceView_traceViewWindowScrolled)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Many traces received, all of them all filtered to trace view. <br>
    * <b>expected</b>: <br>
    *       TraceView gets scrolled on every new line. <br>
    * ************************************************
    */

   /* enable trace filter */
   EXPECT_TRUE(TF::TraceFilters::setText("traceFilter_0", "TEST MESSAGE"));
   TF::wait(100);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_0"));
   TF::wait(100);
   EXPECT_FALSE(TF::TraceFilters::isEditable("traceFilter_0"));

   ASSERT_EQ(TF::TraceView::getScrollPosition(), 0);
   sendDataChunk();
   EXPECT_TRUE(TF::TraceView::getScrollPosition() != 0);
}

TEST_F(WindowsScrollingTests, traceViewScrollingStoppedThenResumedByButton)
{
   /**
    * @test
    * <b>scenario</b>: <br>
    *       Many traces received, all of them all filtered to trace view. <br>
    * <b>expected</b>: <br>
    *       TraceView gets scrolled on every new line. <br>
    * ************************************************
    */

   /* enable trace filter */
   EXPECT_TRUE(TF::TraceFilters::setText("traceFilter_0", "TEST MESSAGE"));
   TF::wait(100);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_0"));
   TF::wait(100);
   EXPECT_FALSE(TF::TraceFilters::isEditable("traceFilter_0"));

   ASSERT_EQ(TF::TraceView::getScrollPosition(), 0);
   sendDataChunk();
   EXPECT_TRUE(TF::TraceView::getScrollPosition() != 0);

   int scrollPosition = TF::TraceView::getScrollPosition();
   EXPECT_TRUE(scrollPosition != 0);

   /* pause scrolling by setting scrollBar position to middle */
   EXPECT_TRUE(TF::TraceView::setPosition(scrollPosition / 2));
   TF::wait(100);
   sendDataChunk();
   int newScrollPosition = TF::TraceView::getScrollPosition();
   EXPECT_TRUE(newScrollPosition == (scrollPosition / 2));

   /* hit BOTTOM button to force scrolling to bottom */
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceBottomButton"));
   TF::wait(100);

   int bottomScrollPosition = TF::TraceView::getScrollPosition();
   EXPECT_TRUE(bottomScrollPosition > newScrollPosition);
   sendDataChunk();
   int finalScrollPosition = TF::TraceView::getScrollPosition();
   EXPECT_TRUE(finalScrollPosition > bottomScrollPosition);
}
