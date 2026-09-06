#pragma once

#include <pd/drivers/hid.hpp>

typedef struct GLFWwindow GLFWwindow;

namespace PD {
class HidGlfw : public HidDriver {
 public:
  HidGlfw(GLFWwindow* window);
  ~HidGlfw();

  void Update() override;

 private:
  struct Impl;
  Impl* impl;
};
}  // namespace PD