#pragma once

#include <string>

class ITuner {
public:
  virtual ~ITuner() = default;

  virtual std::string seekCH() = 0;
  virtual void setCH(std::string ch) = 0;
  virtual std::string getCurrentCH() = 0;
};
