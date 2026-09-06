#include <pd_system/hid_glfw.hpp>
#ifdef PD_ENABLE_HID_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace PD {
struct HidGlfw::Impl {
  GLFWwindow* Win;
  int PrevState;
  std::unordered_map<int, int> PrevStates;
  GLFWcharfun OldTextCB;
  std::string* Text;
  bool InTextMode = false;
};

HidGlfw::HidGlfw(GLFWwindow* win) : HidDriver("HidGlfw") {
  impl = new Impl;
  impl->Win = win;
  pFlags |= PDHidBackendFlags_HasMouse;
  pFlags |= PDHidBackendFlags_HasKeyboard;
  pGamepad[GLFW_MOUSE_BUTTON_LEFT] = HidInternal::Touch;
}

HidGlfw::~HidGlfw() {}

void HidGlfw::Update() {
  HidDriver::Update();  // clear stats
  int state = glfwGetMouseButton(impl->Win, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS) {
    if (impl->PrevState == GLFW_RELEASE) {
      pGamepadEvents[0][Event::Down] |= HidInternal::Touch;
    }
    pGamepadEvents[0][Event::Held] |= HidInternal::Touch;
  } else if (state == GLFW_RELEASE && impl->PrevState == GLFW_PRESS) {
    pGamepadEvents[0][Event::Up] |= HidInternal::Touch;
  }

  impl->PrevState = state;
  // if (pLocked) {
  //   SwapTab();
  // }
  double x, y;
  glfwGetCursorPos(impl->Win, &x, &y);
  pMouse[1] = pMouse[0];  // Cycle pMouse pos
  pMouse[0] = fvec2(x, y);
}
}  // namespace PD
#else
namespace PD {

HidGlfw::HidGlfw(GLFWwindow* win) : HidDriver("HidGlfw") {}

HidGlfw::~HidGlfw() {}

void HidGlfw::Update() {
  HidDriver::Update();  // clear stats
}
}  // namespace PD
#endif