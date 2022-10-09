#ifndef ITIMERS_MOCK_H_
#define ITIMERS_MOCK_H_

#include "ITimers.h"
#include "gmock/gmock.h"

namespace Utilities
{

class ITimersMock : public ITimers
{
public:
   MOCK_METHOD0(start, bool());
   MOCK_METHOD0(stop, void());
   MOCK_METHOD2(createTimer, uint32_t(ITimerClient*, uint32_t));
   MOCK_METHOD1(removeTimer, void(uint32_t));
   MOCK_METHOD2(setTimeout, bool(uint32_t, uint32_t));
   MOCK_METHOD1(getTimeout, uint32_t(uint32_t));
   MOCK_METHOD1(startTimer, void(uint32_t));
   MOCK_METHOD2(startTimer, void(uint32_t, uint32_t));
   MOCK_METHOD2(startTimer, void(uint32_t, bool));
   MOCK_METHOD1(stopTimer, void(uint32_t));
};


}


#endif
