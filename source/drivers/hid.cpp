/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/drivers/hid.hpp>

namespace PD {
bool HidDriver::IsEvent(Event e, Key keys) { return KeyEvents[0][e] & keys; }

bool HidDriver::IsEvent(Event e, KbKey keys) {
  return KbKeyEvents[0][e].Has(keys);
}

void HidDriver::SwapTab() {
  auto tkd = KeyEvents[1][Event_Down];
  auto tkh = KeyEvents[1][Event_Held];
  auto tku = KeyEvents[1][Event_Up];
  KeyEvents[1][Event_Down] = KeyEvents[0][Event_Down];
  KeyEvents[1][Event_Held] = KeyEvents[0][Event_Held];
  KeyEvents[1][Event_Up] = KeyEvents[0][Event_Up];
  KeyEvents[0][Event_Down] = tkd;
  KeyEvents[0][Event_Held] = tkh;
  KeyEvents[0][Event_Up] = tku;
}

/**
 * If this func has no verride, still clear the stats
 * cause if they are empty this leads to a crash
 */
void HidDriver::Update() {
  // Clear States
  for (int i = 0; i < 2; i++) {
    KeyEvents[i][Event_Down] = 0;
    KeyEvents[i][Event_Held] = 0;
    KeyEvents[i][Event_Up] = 0;
    for (auto& it : KbKeyEvents[i]) {
      it.second = Event_Null;
    }
  }
}
}  // namespace PD