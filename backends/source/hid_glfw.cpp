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

constexpr int KEY_BASE = GLFW_GAMEPAD_BUTTON_LAST;
constexpr int KEY_LSTICK_LEFT = KEY_BASE + 1;
constexpr int KEY_LSTICK_RIGHT = KEY_BASE + 2;
constexpr int KEY_LSTICK_UP = KEY_BASE + 3;
constexpr int KEY_LSTICK_DOWN = KEY_BASE + 4;
constexpr int KEY_RSTICK_LEFT = KEY_BASE + 5;
constexpr int KEY_RSTICK_RIGHT = KEY_BASE + 6;
constexpr int KEY_RSTICK_UP = KEY_BASE + 7;
constexpr int KEY_RSTICK_DOWN = KEY_BASE + 8;
constexpr int KEY_LEFT_TRIGGER = KEY_BASE + 9;
constexpr int KEY_RIGHT_TRIGGER = KEY_BASE + 10;
constexpr int KEY_LAST = KEY_RIGHT_TRIGGER;

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
  pGamepad[KEY_LEFT_TRIGGER] = HidInternal::Gamepad::ZL;
  pGamepad[KEY_RIGHT_TRIGGER] = HidInternal::Gamepad::ZR;
  pGamepad[KEY_LSTICK_LEFT] = HidInternal::Gamepad::CPLeft;
  pGamepad[KEY_LSTICK_RIGHT] = HidInternal::Gamepad::CPRight;
  pGamepad[KEY_LSTICK_UP] = HidInternal::Gamepad::CPUp;
  pGamepad[KEY_LSTICK_DOWN] = HidInternal::Gamepad::CPDown;
  pGamepad[KEY_RSTICK_LEFT] = HidInternal::Gamepad::CSLeft;
  pGamepad[KEY_RSTICK_RIGHT] = HidInternal::Gamepad::CSRight;
  pGamepad[KEY_RSTICK_UP] = HidInternal::Gamepad::CSUp;
  pGamepad[KEY_RSTICK_DOWN] = HidInternal::Gamepad::CSDown;
  for (int i = 0; i <= KEY_LAST; i++) {
    impl->GPPrevStates[i] = 0;
  }
}

HidGlfw::~HidGlfw() { delete impl; }

void HidGlfw::Update() {
  HidDriver::Update();  // clear stats
  GLFWgamepadstate gpstate;
  int gps = glfwGetGamepadState(GLFW_JOYSTICK_1, &gpstate);
  if (gps == GLFW_TRUE) {
    HandleAxisKey(gpstate, KEY_LEFT_TRIGGER, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
                  false);
    HandleAxisKey(gpstate, KEY_RIGHT_TRIGGER, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
                  false);
    HandleAxisKey(gpstate, KEY_LSTICK_LEFT, GLFW_GAMEPAD_AXIS_LEFT_X, true);
    HandleAxisKey(gpstate, KEY_LSTICK_RIGHT, GLFW_GAMEPAD_AXIS_LEFT_X, false);
    HandleAxisKey(gpstate, KEY_LSTICK_UP, GLFW_GAMEPAD_AXIS_LEFT_Y, true);
    HandleAxisKey(gpstate, KEY_LSTICK_DOWN, GLFW_GAMEPAD_AXIS_LEFT_Y, false);
    HandleAxisKey(gpstate, KEY_RSTICK_LEFT, GLFW_GAMEPAD_AXIS_RIGHT_X, true);
    HandleAxisKey(gpstate, KEY_RSTICK_RIGHT, GLFW_GAMEPAD_AXIS_RIGHT_X, false);
    HandleAxisKey(gpstate, KEY_RSTICK_UP, GLFW_GAMEPAD_AXIS_RIGHT_Y, true);
    HandleAxisKey(gpstate, KEY_RSTICK_DOWN, GLFW_GAMEPAD_AXIS_RIGHT_Y, false);
    pLStick[0].x = gpstate.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    pLStick[0].y = gpstate.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    pRStick[0].x = gpstate.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    pRStick[0].y = gpstate.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) {
      if (gpstate.buttons[i] == GLFW_PRESS) {
        if (impl->GPPrevStates[i] == GLFW_RELEASE) {
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

void HidGlfw::HandleAxisKey(GLFWgamepadstate s, int iK, int eA, bool negative) {
  if (s.axes[eA] <= 1.f && s.axes[eA] >= -1.f) {
    if (s.axes[eA] > 0.05f && !negative) {
      if (impl->GPPrevStates[iK] == GLFW_RELEASE) {
        pGamepadEvents[0][Event::Down] |= pGamepad[iK];
      }
      pGamepadEvents[0][Event::Held] |= pGamepad[iK];
    }
    if (s.axes[eA] < -0.05f && negative) {
      if (impl->GPPrevStates[iK] == GLFW_RELEASE) {
        pGamepadEvents[0][Event::Down] |= pGamepad[iK];
      }
      pGamepadEvents[0][Event::Held] |= pGamepad[iK];
    }
  }
  impl->GPPrevStates[iK] = (s.axes[eA] <= -0.05 && s.axes[eA] >= 0.05 &&
                            s.axes[eA] >= -1.f && s.axes[eA] <= 1);
}
}  // namespace PD
#else
struct GLFWgamepadstate {};
namespace PD {

HidGlfw::HidGlfw(GLFWwindow* win) : HidDriver("HidGlfw") {}

HidGlfw::~HidGlfw() {}

void HidGlfw::Update() {
  HidDriver::Update();  // clear stats
}
void HidGlfw::HandleAxisKey(GLFWgamepadstate s, int iK, int eA, bool negative) {
}
}  // namespace PD
#endif