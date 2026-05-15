#include "TVChannelController.h"

#include <algorithm>
#include <stdexcept>

TVChannelController::TVChannelController(ITuner &tuner) : tuner_(tuner) {}

void TVChannelController::pressNumber(int number) {
  if (number < 0 || number > 9) {
    throw std::invalid_argument("number must be between 0 and 9");
  }

  processingCH_ += std::to_string(number);

  if (processingCH_.size() == 2) {
    setTunerCH();
  }
}

void TVChannelController::pressConfirm() {
  if (!processingCH_.empty()) {
    setTunerCH();
  }
}

void TVChannelController::pressFavorite() {
  int currentCH = std::stoi(tuner_.getCurrentCH());
  auto it =
      std::find(favoriteChannels_.begin(), favoriteChannels_.end(), currentCH);

  if (it == favoriteChannels_.end()) {
    addFavorite(currentCH);
    return;
  }

  favoriteChannels_.erase(it);
}

void TVChannelController::pressNextFavorite() {
  if (favoriteChannels_.empty()) {
    return;
  }

  int currentCH = std::stoi(tuner_.getCurrentCH());
  auto it = std::find_if(favoriteChannels_.begin(), favoriteChannels_.end(),
                         [&](int ch) { return ch > currentCH; });

  int nextCH =
      (it != favoriteChannels_.end()) ? *it : favoriteChannels_.front();
  tuner_.setCH(std::to_string(nextCH));
}

void TVChannelController::addFavorite(int channel) {
  if (channel < 0 || channel > 99) {
    throw std::invalid_argument("favorite channel must be between 0 and 99");
  }

  auto it =
      std::find(favoriteChannels_.begin(), favoriteChannels_.end(), channel);
  if (it != favoriteChannels_.end()) {
    return;
  }

  favoriteChannels_.push_back(channel);
  std::sort(favoriteChannels_.begin(), favoriteChannels_.end());
}

const std::vector<int> &TVChannelController::getFavoriteChannels() const {
  return favoriteChannels_;
}

void TVChannelController::setTunerCH() {
  tuner_.setCH(processingCH_);
  processingCH_.clear();
}
