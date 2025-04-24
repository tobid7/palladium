#include <3ds.h>

#include <pd_hid_3ds.hpp>

namespace PD {
Hid3DS::Hid3DS() : Hid("3DS") {
  binds[KEY_A] = A;
  binds[KEY_B] = B;
  binds[KEY_X] = X;
  binds[KEY_Y] = Y;
  binds[KEY_START] = Start;
  binds[KEY_SELECT] = Select;
  binds[KEY_L] = L;
  binds[KEY_R] = R;
  binds[KEY_DUP] = DUp;
  binds[KEY_DDOWN] = DDown;
  binds[KEY_DLEFT] = DLeft;
  binds[KEY_DRIGHT] = DRight;
  binds[KEY_CPAD_UP] = CPUp;
  binds[KEY_CPAD_DOWN] = CPDown;
  binds[KEY_CPAD_LEFT] = CPLeft;
  binds[KEY_CPAD_RIGHT] = CPRight;
  binds[KEY_CSTICK_UP] = CSUp;
  binds[KEY_CSTICK_DOWN] = CSDown;
  binds[KEY_CSTICK_LEFT] = CSLeft;
  binds[KEY_CSTICK_RIGHT] = CSRight;
  binds[KEY_ZL] = ZL;
  binds[KEY_ZR] = ZR;
  binds[KEY_TOUCH] = Touch;
}
void Hid3DS::Update() {
  hidScanInput();
  for (int i = 0; i < 2; i++) {
    key_events[i][Event_Down] = 0;
    key_events[i][Event_Held] = 0;
    key_events[i][Event_Up] = 0;
  }
  u32 kd = hidKeysDown();
  u32 kh = hidKeysHeld();
  u32 ku = hidKeysUp();
  for (auto &b : binds) {
    if (b.first & kd) {
      key_events[0][Event_Down] |= b.second;
    }
    if (b.first & kh) {
      key_events[0][Event_Held] |= b.second;
    }
    if (b.first & ku) {
      key_events[0][Event_Up] |= b.second;
    }
  }
  if (locked) {
    SwappyTable();
  }
  touchPosition t;
  hidTouchRead(&t);
  touch[1] = touch[0];  // Cycle touch pos
  touch[0] = fvec2(t.px, t.py);
}
}  // namespace PD