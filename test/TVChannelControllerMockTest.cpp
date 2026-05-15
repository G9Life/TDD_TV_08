#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "MockTuner.h"
#include "TVChannelController.h"

#include <memory>

using ::testing::Return;

class ControllerMockTest : public ::testing::Test {
protected:
  MockTuner mockTuner;
  std::unique_ptr<TVChannelController> ctrl;

  void SetUp() override {
    ctrl = std::make_unique<TVChannelController>(mockTuner);
  }
};

TEST_F(ControllerMockTest, PressNumber1Confirm) {
  EXPECT_CALL(mockTuner, setCH("1")).Times(1);

  ctrl->pressNumber(1);
  ctrl->pressConfirm();
}

TEST_F(ControllerMockTest, Press1Then2_SetCH12) {
  EXPECT_CALL(mockTuner, setCH("12")).Times(1);

  ctrl->pressNumber(1);
  ctrl->pressNumber(2);
}

TEST_F(ControllerMockTest, PressFavorite_GetsCurrentCH) {
  EXPECT_CALL(mockTuner, getCurrentCH()).WillOnce(Return("12"));

  ctrl->pressFavorite();
}

TEST_F(ControllerMockTest, NextFav_CallsSetCH) {
  ctrl->addFavorite(12);
  ctrl->addFavorite(56);

  EXPECT_CALL(mockTuner, getCurrentCH()).WillOnce(Return("6"));
  EXPECT_CALL(mockTuner, setCH("12")).Times(1);

  ctrl->pressNextFavorite();
}
