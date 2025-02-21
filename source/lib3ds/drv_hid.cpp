/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <3ds.h>

#include <pd/lib3ds/drv_hid.hpp>

/// Reform of the RenderD7 095 Hid Api
/// Using Custom Keybindings for future
/// Porting of the library

namespace PD {
CtrHid::CtrHid() {
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
void CtrHid::Update() {
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
}  // namespace PD