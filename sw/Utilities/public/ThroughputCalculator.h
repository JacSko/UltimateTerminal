#pragma once

#include <stdint.h>
#include <chrono>

/* ============================= */
/**
 * @file ThroughputCalculator.h
 *
 * @brief Allows to calculate the bitrate of data per second.
 *
 * @author Jacek Skowronek
 * @date 21/03/2023
 */
/* ============================= */


namespace Utilities
{

enum class ByteUnit : uint32_t
{
   bytes_s = 1,
   kilobytes_s = 1024,
   megabytes_s = 1048576,
};

struct ThroughputResult
{
   double value = 0.0;
   ByteUnit unit = ByteUnit::bytes_s;
};

class ThroughputCalculator
{
public:
   void start()
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_start.time_since_epoch().count())
      {
         m_start = std::chrono::steady_clock::now();
         m_bytesCount = 0;
      }
   }
   void reset()
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_start = {};
      m_bytesCount = 0;
   }
   void reportBytes(uint32_t bytes_count)
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (m_start.time_since_epoch().count())
      {
         m_bytesCount += bytes_count;
      }
   }
   ThroughputResult get()
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      ThroughputResult result = {};
      if (m_start.time_since_epoch().count())
      {
         auto milliseconds_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start).count();
         if (milliseconds_elapsed != 0 && m_bytesCount)
         {
            double current_thoughput = ((double)m_bytesCount / milliseconds_elapsed) * 1000;
            result.unit = deduceByteUnit(current_thoughput);
            result.value = current_thoughput / static_cast<uint32_t>(result.unit);
         }
      }
      return result;
   }

private:

   ByteUnit deduceByteUnit(double value)
   {
      ByteUnit result = ByteUnit::bytes_s;
      if (value > static_cast<uint32_t>(ByteUnit::kilobytes_s))
      {
         result = ByteUnit::kilobytes_s;
      }
      if (value > static_cast<uint32_t>(ByteUnit::megabytes_s))
      {
         result = ByteUnit::megabytes_s;
      }
      return result;
   }

   uint32_t m_bytesCount;
   std::chrono::steady_clock::time_point m_start;
   std::mutex m_mutex;
};

}
