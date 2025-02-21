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

#include <pd/drivers/hid.hpp>

/// Reform of the RenderD7 095 Hid Api
/// Using Custom Keybindings for future
/// Porting of the library

namespace PD {
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