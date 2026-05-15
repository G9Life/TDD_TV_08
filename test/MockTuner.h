#pragma once

#include "ITuner.h"

#include <gmock/gmock.h>

#include <string>

class MockTuner : public ITuner {
public:
    MOCK_METHOD(std::string, seekCH, (), (override));
    MOCK_METHOD(void, setCH, (std::string ch), (override));
    MOCK_METHOD(std::string, getCurrentCH, (), (override));
};
