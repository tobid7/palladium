/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd-3ds/bknd-hid.hpp>

namespace PD {
Hid3DS::Hid3DS() : HidDriver("Hid3DS") {
  this->Flags |= Flags_HasTouch;
  this->Flags |= FLags_HasGamepad;
  pBinds[KEY_A] = A;
  pBinds[KEY_B] = B;
  pBinds[KEY_X] = X;
  pBinds[KEY_Y] = Y;
  pBinds[KEY_START] = Start;
  pBinds[KEY_SELECT] = Select;
  pBinds[KEY_L] = L;
  pBinds[KEY_R] = R;
  pBinds[KEY_DUP] = DUp;
  pBinds[KEY_DDOWN] = DDown;
  pBinds[KEY_DLEFT] = DLeft;
  pBinds[KEY_DRIGHT] = DRight;
  pBinds[KEY_CPAD_UP] = CPUp;
  pBinds[KEY_CPAD_DOWN] = CPDown;
  pBinds[KEY_CPAD_LEFT] = CPLeft;
  pBinds[KEY_CPAD_RIGHT] = CPRight;
  pBinds[KEY_CSTICK_UP] = CSUp;
  pBinds[KEY_CSTICK_DOWN] = CSDown;
  pBinds[KEY_CSTICK_LEFT] = CSLeft;
  pBinds[KEY_CSTICK_RIGHT] = CSRight;
  pBinds[KEY_ZL] = ZL;
  pBinds[KEY_ZR] = ZR;
  pBinds[KEY_TOUCH] = Touch;
}

void Hid3DS::Update() {
  hidScanInput();
  for (int i = 0; i < 2; i++) {
    KeyEvents[i][Event_Down] = 0;
    KeyEvents[i][Event_Held] = 0;
    KeyEvents[i][Event_Up] = 0;
  }
  u32 kd = hidKeysDown();
  u32 kh = hidKeysHeld();
  u32 ku = hidKeysUp();
  for (auto &b : pBinds) {
    if (b.first & kd) {
      KeyEvents[0][Event_Down] |= b.second;
    }
    if (b.first & kh) {
      KeyEvents[0][Event_Held] |= b.second;
    }
    if (b.first & ku) {
      KeyEvents[0][Event_Up] |= b.second;
    }
  }
  if (pLocked) {
    SwapTab();
  }
  touchPosition t;
  hidTouchRead(&t);
  pMouse[1] = pMouse[0];  // Cycle pMouse pos
  pMouse[0] = fvec2(t.px, t.py);
}
}  // namespace PD