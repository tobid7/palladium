#include <3ds.h>

#include <pd/controls/hid.hpp>

/// Reform of the RenderD7 095 Hid Api
/// Using Custom Keybindings for future
/// Porting of the library

namespace PD {
Hid::Hid() {
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
void Hid::Update() {
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
  touch[0] = vec2(t.px, t.py);
}

bool Hid::IsEvent(Event e, Key keys) { return key_events[0][e] & keys; }

void Hid::SwappyTable() {
  auto tkd = key_events[1][Event_Down];
  auto tkh = key_events[1][Event_Held];
  auto tku = key_events[1][Event_Up];
  key_events[1][Event_Down] = key_events[0][Event_Down];
  key_events[1][Event_Held] = key_events[0][Event_Held];
  key_events[1][Event_Up] = key_events[0][Event_Up];
  key_events[0][Event_Down] = tkd;
  key_events[0][Event_Held] = tkh;
  key_events[0][Event_Up] = tku;
}
}  // namespace PD