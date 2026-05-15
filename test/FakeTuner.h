#pragma once

#include "ITuner.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class FakeTuner : public ITuner {
private:
  int current_ = 0;
  std::vector<int> available_;

public:
  explicit FakeTuner(std::vector<int> avail) : available_(std::move(avail)) {
    if (available_.empty()) {
      throw std::invalid_argument("사용 가능한 채널이 없습니다.");
    }

    std::sort(available_.begin(), available_.end());
  }

  std::string seekCH() override {
    auto it = std::find_if(available_.begin(), available_.end(),
                           [&](int ch) { return ch > current_; });

    current_ = (it != available_.end()) ? *it : available_.front();

    return std::to_string(current_);
  }

  void setCH(std::string ch) override {
    int v = std::stoi(ch);

    if (v < 0 || v > 99) {
      throw std::invalid_argument("채널범위초과: " + ch);
    }

    current_ = v;
  }

  std::string getCurrentCH() override { return std::to_string(current_); }
};