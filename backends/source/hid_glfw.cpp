#include <pd_system/hid_glfw.hpp>
#ifdef PD_ENABLE_HID_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace PD {
struct HidGlfw::Impl {
  GLFWwindow* Win;
  int PrevState;
  std::unordered_map<int, int> PrevStates;
  std::unordered_map<int, int> GPPrevStates;
  GLFWcharfun OldTextCB;
  std::string* Text;
  bool InTextMode = false;
};

HidGlfw::HidGlfw(GLFWwindow* win) : HidDriver("HidGlfw") {
  impl = new Impl;
  impl->Win = win;

  pFlags |= PDHidBackendFlags_HasMouse;
  pFlags |= PDHidBackendFlags_HasKeyboard;
  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    pFlags |= PDHidBackendFlags_HasGamepad;
  }
  pGamepad[GLFW_GAMEPAD_BUTTON_A] = HidInternal::Gamepad::A;
  pGamepad[GLFW_GAMEPAD_BUTTON_B] = HidInternal::Gamepad::B;
  pGamepad[GLFW_GAMEPAD_BUTTON_X] = HidInternal::Gamepad::X;
  pGamepad[GLFW_GAMEPAD_BUTTON_Y] = HidInternal::Gamepad::Y;
  pGamepad[GLFW_GAMEPAD_BUTTON_START] = HidInternal::Gamepad::Start;
  pGamepad[GLFW_GAMEPAD_BUTTON_BACK] = HidInternal::Gamepad::Select;
  pGamepad[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = HidInternal::Gamepad::DLeft;
  pGamepad[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = HidInternal::Gamepad::DRight;
  pGamepad[GLFW_GAMEPAD_BUTTON_DPAD_UP] = HidInternal::Gamepad::DUp;
  pGamepad[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = HidInternal::Gamepad::DDown;
  pGamepad[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = HidInternal::Gamepad::L;
  pGamepad[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = HidInternal::Gamepad::R;
  pGamepad[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = HidInternal::Gamepad::LStick;
  pGamepad[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = HidInternal::Gamepad::RStick;
  for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
    impl->GPPrevStates[i] = 0;
  }
}

HidGlfw::~HidGlfw() {}

void HidGlfw::Update() {
  HidDriver::Update();  // clear stats
  GLFWgamepadstate gpstate;
  int gps = glfwGetGamepadState(GLFW_JOYSTICK_1, &gpstate);
  if (gps == GLFW_TRUE) {
    for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
      if (gpstate.buttons[i] == GLFW_PRESS) {
        if (impl->PrevStates[i] == GLFW_RELEASE) {
          pGamepadEvents[0][Event::Down] |= pGamepad[i];
        }
        pGamepadEvents[0][Event::Held] |= pGamepad[i];
      } else if (gpstate.buttons[i] == GLFW_RELEASE &&
                 impl->GPPrevStates[i] == GLFW_PRESS) {
        pGamepadEvents[0][Event::Up] |= pGamepad[i];
      }
      impl->GPPrevStates[i] = gpstate.buttons[i];
    }
  }
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