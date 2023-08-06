#include "gtest/gtest.h"
#include <thread>

#include "ThroughputCalculator.h"

using namespace testing;

namespace Utilities
{

TEST(ThroughputCalculator, throughput_calculation_when_module_not_started)
{
   /**
    * <b>scenario</b>: Object created, no start issued yet, get() function called.<br>
    * <b>expected</b>: Empty data rate returned.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::bytes_s);
   EXPECT_NEAR(result.value, 0, 0.0001);
}

TEST(ThroughputCalculator, throughput_calculate_on_empty_data)
{
   /**
    * <b>scenario</b>: Object started, get() function called.<br>
    * <b>expected</b>: Empty data rate returned.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::bytes_s);
   EXPECT_NEAR(result.value, 0, 0.0001);
}

TEST(ThroughputCalculator, throughput_calculate_on_non_empty_data)
{
   /**
    * <b>scenario</b>: Object started, bytes reported, get() function called.<br>
    * <b>expected</b>: Non-empty data rate returned.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   test_subject.reportBytes(200);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));
   test_subject.reportBytes(400);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::bytes_s);
   EXPECT_NEAR(result.value, 600, 60);
}

TEST(ThroughputCalculator, throughput_kilobytes_unit_reporting)
{
   /**
    * <b>scenario</b>: Object started, bytes reported, get() function called.<br>
    * <b>expected</b>: Non-empty data rate returned.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   test_subject.reportBytes(700);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));
   test_subject.reportBytes(700);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::kilobytes_s);
   EXPECT_NEAR(result.value, 1.36, 0.13);
}

TEST(ThroughputCalculator, throughput_megabytes_unit_reporting)
{
   /**
    * <b>scenario</b>: Object started, bytes reported, get() function called.<br>
    * <b>expected</b>: Non-empty data rate returned.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   test_subject.reportBytes(700000);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));
   test_subject.reportBytes(700000);
   std::this_thread::sleep_for(std::chrono::milliseconds(500));

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::megabytes_s);
   EXPECT_NEAR(result.value, 1.33, 0.15);
}

TEST(ThroughputCalculator, no_data_throughput_decreased)
{
   /**
    * <b>scenario</b>: Object started, no data was reported for some time.<br>
    * <b>expected</b>: Data rate shall start decreasing.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   test_subject.reportBytes(1000);
   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::bytes_s);
   EXPECT_NEAR(result.value, 1000, 100);

   double last_value = result.value;
   for (uint8_t i = 0; i < 5; i++)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      result = test_subject.get();
      EXPECT_EQ(result.unit, ByteUnit::bytes_s);
      EXPECT_TRUE(last_value > result.value);
      last_value = result.value;
   }
}

TEST(ThroughputCalculator, values_reporting_when_less_time_than_second)
{
   /**
    * <b>scenario</b>: Object started, bytes reported, get() called after 500ms.<br>
    * <b>expected</b>: Data rate shall be recalculated to represent seconds baseline.<br>
    * ************************************************
    */
   ThroughputCalculator test_subject;
   test_subject.start();

   test_subject.reportBytes(1000);
   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   auto result = test_subject.get();
   EXPECT_EQ(result.unit, ByteUnit::kilobytes_s);
   EXPECT_NEAR(result.value, 9.76, 2);
}


}
