#pragma once

#include <pd/drivers/hid.hpp>

namespace PD {
class Hid3DS : public HidDriver {
 public:
  Hid3DS();
  ~Hid3DS();

  void Update() override;

 private:
  struct Impl;
  Impl* impl;
};
}  // namespace PD