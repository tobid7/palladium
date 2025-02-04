#pragma once

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

#include <pd/common/common.hpp>
#include <pd/maths/vec.hpp>

namespace PD {
class Hid : public SmartCtor<Hid> {
 public:
  enum Key : u32 {
    No = 0,
    A = 1 << 0,
    B = 1 << 1,
    X = 1 << 2,
    Y = 1 << 3,
    Start = 1 << 4,
    Select = 1 << 5,
    L = 1 << 6,
    R = 1 << 7,
    DUp = 1 << 8,
    DDown = 1 << 9,
    DLeft = 1 << 10,
    DRight = 1 << 11,
    CPUp = 1 << 12,
    CPDown = 1 << 13,
    CPLeft = 1 << 14,
    CPRight = 1 << 15,
    CSUp = 1 << 16,
    CSDown = 1 << 17,
    CSLeft = 1 << 18,
    CSRight = 1 << 19,
    ZL = 1 << 20,
    ZR = 1 << 21,
    Touch = 1 << 22,
    Up = DUp | CPUp,
    Down = DDown | CPDown,
    Left = DLeft | CPLeft,
    Right = DRight | CPRight,
  };
  enum Event {
    Event_Down,
    Event_Held,
    Event_Up,
  };
  Hid();
  ~Hid() {}

  vec2 TouchPos() const { return touch[0]; }
  vec2 TouchPosLast() const { return touch[1]; }

  bool IsEvent(Event e, Key keys);
  bool IsDown(Key keys) const { return key_events[0].at(Event_Down) & keys; }
  bool IsHeld(Key keys) const { return key_events[0].at(Event_Held) & keys; }
  bool IsUp(Key keys) const { return key_events[0].at(Event_Up) & keys; }

  void Clear() {
    for (int i = 0; i < 2; i++) {
      key_events[i][Event_Down] = 0;
      key_events[i][Event_Up] = 0;
      key_events[i][Event_Held] = 0;
    }
  }

  void Lock(bool v) {
    if (v != locked) {
      SwappyTable();
    }
    locked = v;
  }
  bool Locked() const { return locked; }
  void Lock() {
    if (!locked) {
      SwappyTable();
    }
    locked = true;
  }
  void Unlock() {
    if (locked) {
      SwappyTable();
    }
    locked = false;
  }

  /// @brief Get the New Keystates etc
  /// @note WOW not using the deltatime
  void Update();

 private:
  void SwappyTable();
  /// Using 2 Touch positions for current and last frame
  vec2 touch[2];
  bool locked = false;
  std::unordered_map<Event, u32> key_events[2];
  std::unordered_map<u32, u32> binds;
};
}  // namespace PD