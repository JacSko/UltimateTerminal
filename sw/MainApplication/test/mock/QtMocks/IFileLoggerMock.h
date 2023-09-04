#pragma once
#include <gmock/gmock.h>
#include "IFileLogger.h"

struct IFileLoggerMock : public MainApplication::IFileLogger
{
   MOCK_METHOD0(isActive, bool());
   MOCK_METHOD1(openFile, bool(const std::string&));
   MOCK_METHOD0(closeFile, void());
   MOCK_METHOD1(putLog, void(const std::string&));
};
