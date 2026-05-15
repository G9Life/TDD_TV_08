#pragma once

#include "ITuner.h"

#include <string>
#include <vector>

class TVChannelController {
public:
  explicit TVChannelController(ITuner &tuner);

  void pressNumber(int number);
  void pressConfirm();
  void pressFavorite();
  void pressNextFavorite();
  void addFavorite(int channel);

  const std::vector<int> &getFavoriteChannels() const;

private:
  ITuner &tuner_;
  std::string processingCH_;
  std::vector<int> favoriteChannels_;

  void setTunerCH();
};
