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
   EXPECT_EQ(TF::Buttons::getBackgroundColor("scrollButton"), 0x00FF00);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("scrollButton"));
   EXPECT_EQ(TF::Buttons::getBackgroundColor("scrollButton"), 0xffefefef);

   EXPECT_EQ(TF::Common::getCommand(), "");
   EXPECT_TRUE(TF::Common::setCommand("test command"));
   EXPECT_EQ(TF::Common::getCommand(), "test command");

//   EXPECT_EQ(TF::Ports::getLabelText(0), "");
//   EXPECT_EQ(TF::Ports::getLabelStylesheet(0), "");
//
//   EXPECT_EQ(TF::Ports::getLabelText(1), "");
//   EXPECT_EQ(TF::Ports::getLabelStylesheet(1), "");

   EXPECT_EQ(TF::Common::getLineEnding(), "\\r\\n");
   EXPECT_TRUE(TF::Common::setLineEnding("\\n"));
   EXPECT_EQ(TF::Common::getLineEnding(), "\\n");

   EXPECT_TRUE(TF::Common::isLineEndingVisible("\\r\\n"));
   EXPECT_TRUE(TF::Common::isLineEndingVisible("\\n"));
   EXPECT_TRUE(TF::Common::isLineEndingVisible("EMPTY"));
   EXPECT_FALSE(TF::Common::isLineEndingVisible("not existing"));

   EXPECT_EQ(TF::Common::getTargetPort(), "");
   EXPECT_FALSE(TF::Common::setTargetPort("not exist"));
   EXPECT_EQ(TF::Common::countTargetPorts(), 0);


//   EXPECT_TRUE(TF::TraceFilters::isEditable("traceFilterButton_1"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "");
//   EXPECT_TRUE(TF::TraceFilters::setText("traceFilterButton_3", "test_filter"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "test_filter");
//   EXPECT_TRUE(TF::TraceFilters::isEditable("traceFilterButton_3"));
//
//
//   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_3"));
//   EXPECT_FALSE(TF::TraceFilters::isEditable("traceFilterButton_3"));
//   EXPECT_FALSE(TF::TraceFilters::setText("traceFilterButton_3", "new_filter"));
//   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "test_filter");



   EXPECT_EQ(TF::TerminalView::countItems(), 0);
   EXPECT_EQ(TF::TraceView::countItems(), 0);
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("markerButton"));
   EXPECT_EQ(TF::TerminalView::countItems(), 1);
   EXPECT_EQ(TF::TraceView::countItems(), 0);

   EXPECT_EQ(TF::TerminalView::countItemsWithBackgroundColor(0x00FF0000), 1);
   EXPECT_EQ(TF::TerminalView::countItemsWithFontColor(0xFF000000), 1);

   EXPECT_TRUE(TF::TraceFilters::setText("traceFilterButton_3", "MARKER"));
   EXPECT_EQ(TF::TraceFilters::getText("traceFilterButton_3"), "MARKER");
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("traceFilterButton_3"));
   EXPECT_TRUE(TF::Buttons::simulateButtonClick("markerButton"));
   EXPECT_EQ(TF::TerminalView::countItems(), 2);
   EXPECT_EQ(TF::TraceView::countItems(), 1);

}
