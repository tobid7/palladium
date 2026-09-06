#pragma once

#include <pd/drivers/hid.hpp>

namespace PD {
class HidNX : public HidDriver {
 public:
  HidNX();
  ~HidNX();

  void Update() override;

 private:
  struct Impl;
  Impl* impl;
};
}  // namespace PD