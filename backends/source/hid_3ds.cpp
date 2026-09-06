#include <pd_system/hid_3ds.hpp>
#if defined(PD_ENABLE_HID3DS) && defined(__3DS__)
#include <3ds.h>
namespace PD {
struct Hid3DS::Impl {
  std::string* Text;
  bool InTextMode = false;
};

Hid3DS::Hid3DS() : HidDriver("Hid3DS") {
  impl = new Impl;

  pFlags |= PDHidBackendFlags_HasGamepad;
  pFlags |= PDHidBackendFlags_HasTouch;

  pGamepad[KEY_A] = HidInternal::Gamepad::A;
  pGamepad[KEY_B] = HidInternal::Gamepad::B;
  pGamepad[KEY_X] = HidInternal::Gamepad::X;
  pGamepad[KEY_Y] = HidInternal::Gamepad::Y;
  pGamepad[KEY_START] = HidInternal::Gamepad::Start;
  pGamepad[KEY_SELECT] = HidInternal::Gamepad::Select;
  pGamepad[KEY_L] = HidInternal::Gamepad::L;
  pGamepad[KEY_R] = HidInternal::Gamepad::R;
  pGamepad[KEY_DUP] = HidInternal::Gamepad::DUp;
  pGamepad[KEY_DDOWN] = HidInternal::Gamepad::DDown;
  pGamepad[KEY_DLEFT] = HidInternal::Gamepad::DLeft;
  pGamepad[KEY_DRIGHT] = HidInternal::Gamepad::DRight;
  pGamepad[KEY_CPAD_UP] = HidInternal::Gamepad::CPUp;
  pGamepad[KEY_CPAD_DOWN] = HidInternal::Gamepad::CPDown;
  pGamepad[KEY_CPAD_LEFT] = HidInternal::Gamepad::CPLeft;
  pGamepad[KEY_CPAD_RIGHT] = HidInternal::Gamepad::CPRight;
  pGamepad[KEY_CSTICK_UP] = HidInternal::Gamepad::CSUp;
  pGamepad[KEY_CSTICK_DOWN] = HidInternal::Gamepad::CSDown;
  pGamepad[KEY_CSTICK_LEFT] = HidInternal::Gamepad::CSLeft;
  pGamepad[KEY_CSTICK_RIGHT] = HidInternal::Gamepad::CSRight;
  pGamepad[KEY_ZL] = HidInternal::Gamepad::ZL;
  pGamepad[KEY_ZR] = HidInternal::Gamepad::ZR;
  pGamepad[KEY_TOUCH] = HidInternal::Gamepad::Touch;
}

Hid3DS::~Hid3DS() { delete impl; }

void Hid3DS::Update() {
  HidDriver::Update();  // clear stats
  hidScanInput();
  u32 kd = hidKeysDown();
  u32 kh = hidKeysHeld();
  u32 ku = hidKeysUp();

  for (const auto& b : pGamepad) {
    if (b.first & kd) {
      pGamepadEvents[0][Event::Down] |= b.second;
    }
    if (b.first & kh) {
      pGamepadEvents[0][Event::Held] |= b.second;
    }
    if (b.first & ku) {
      pGamepadEvents[0][Event::Up] |= b.second;
    }
  }

  /*if (pLocked) {
    SwapTab();
  }*/
  touchPosition t;
  hidTouchRead(&t);
  pMouse[0] = fvec2(t.px, t.py);
}
}  // namespace PD
#else
struct GLFWgamepadstate {};
namespace PD {

Hid3DS::Hid3DS() : HidDriver("Hid3DS") {}

Hid3DS::~Hid3DS() {}

void Hid3DS::Update() {
  HidDriver::Update();  // clear stats
}
}  // namespace PD
#endif