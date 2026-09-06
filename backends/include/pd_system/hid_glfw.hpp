#pragma once

#include <pd/drivers/hid.hpp>

typedef struct GLFWwindow GLFWwindow;
typedef struct GLFWgamepadstate GLFWgamepadstate;

namespace PD {
class HidGlfw : public HidDriver {
 public:
  HidGlfw(GLFWwindow* window);
  ~HidGlfw();

  void Update() override;

 private:
  void HandleAxisKey(GLFWgamepadstate s, int iK, int eA, bool negative);
  struct Impl;
  Impl* impl;
};
}  // namespace PD