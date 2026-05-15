#include <gtest/gtest.h>

#include "FakeTuner.h"
#include "TVChannelController.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class ControllerTest : public ::testing::Test {
protected:
  std::unique_ptr<FakeTuner> tuner;
  std::unique_ptr<TVChannelController> ctrl;

  void SetUp() override {
    tuner = std::make_unique<FakeTuner>(std::vector<int>{1, 4, 12, 56});
    ctrl = std::make_unique<TVChannelController>(*tuner);
  }
};

TEST_F(ControllerTest, PressNumber1ThenConfirm) {
  ctrl->pressNumber(1);
  ctrl->pressConfirm();

  EXPECT_EQ("1", tuner->getCurrentCH());
}

TEST_F(ControllerTest, Press1Then2_AutoChange) {
  ctrl->pressNumber(1);
  ctrl->pressNumber(2);

  EXPECT_EQ("12", tuner->getCurrentCH());
}

TEST_F(ControllerTest, FavoriteAdd_NewChannel) {
  tuner->setCH("12");

  ctrl->pressFavorite();

  const auto &favs = ctrl->getFavoriteChannels();
  EXPECT_NE(favs.end(), std::find(favs.begin(), favs.end(), 12));
}

TEST_F(ControllerTest, FavoriteToggle_Remove) {
  tuner->setCH("12");

  ctrl->pressFavorite();
  ctrl->pressFavorite();

  const auto &favs = ctrl->getFavoriteChannels();
  EXPECT_EQ(favs.end(), std::find(favs.begin(), favs.end(), 12));
}

TEST_F(ControllerTest, FavoriteToggleScenario) {
  for (int ch : {12, 8, 37, 8, 6}) {
    tuner->setCH(std::to_string(ch));
    ctrl->pressFavorite();
  }

  const auto &favs = ctrl->getFavoriteChannels();

  EXPECT_EQ(3u, favs.size());
  EXPECT_NE(favs.end(), std::find(favs.begin(), favs.end(), 6));
  EXPECT_NE(favs.end(), std::find(favs.begin(), favs.end(), 12));
  EXPECT_NE(favs.end(), std::find(favs.begin(), favs.end(), 37));
}

TEST_F(ControllerTest, NextFavorite_Normal) {
  for (int ch : {1, 4, 12, 56}) {
    ctrl->addFavorite(ch);
  }

  tuner->setCH("6");

  ctrl->pressNextFavorite();

  EXPECT_EQ("12", tuner->getCurrentCH());
}

TEST_F(ControllerTest, NextFavorite_WrapAround) {
  ctrl->addFavorite(1);
  ctrl->addFavorite(56);
  tuner->setCH("56");

  ctrl->pressNextFavorite();

  EXPECT_EQ("1", tuner->getCurrentCH());
}

TEST_F(ControllerTest, NextFavorite_EmptyList) {
  tuner->setCH("6");

  ctrl->pressNextFavorite();

  EXPECT_EQ("6", tuner->getCurrentCH());
}
