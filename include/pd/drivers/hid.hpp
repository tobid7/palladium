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

#include <pd/core/common.hpp>
#include <pd/core/vec.hpp>

namespace PD {
/** Input Driver Template class */
class Hid : public SmartCtor<Hid> {
 public:
  /** Key [Controller] */
  enum Key : u32 {
    No = 0,                    ///< No Key
    A = 1 << 0,                ///< A
    B = 1 << 1,                ///< B
    X = 1 << 2,                ///< X
    Y = 1 << 3,                ///< Y
    Start = 1 << 4,            ///< Start
    Select = 1 << 5,           ///< Select
    L = 1 << 6,                ///< L
    R = 1 << 7,                ///< R
    DUp = 1 << 8,              ///< Dpad Up
    DDown = 1 << 9,            ///< Dpad down
    DLeft = 1 << 10,           ///< Dpad left
    DRight = 1 << 11,          ///< Dpad right
    CPUp = 1 << 12,            ///< Cpad up
    CPDown = 1 << 13,          ///< cpad down
    CPLeft = 1 << 14,          ///< cpad left
    CPRight = 1 << 15,         ///< Cpad right
    CSUp = 1 << 16,            ///< Cstick up
    CSDown = 1 << 17,          ///< cstick down
    CSLeft = 1 << 18,          ///< cstick left
    CSRight = 1 << 19,         ///< cstick right
    ZL = 1 << 20,              ///< ZL
    ZR = 1 << 21,              ///< ZR
    Touch = 1 << 22,           ///< Touch
    Up = DUp | CPUp,           ///< DPad or CPad Up
    Down = DDown | CPDown,     ///< DPad or CPad Down
    Left = DLeft | CPLeft,     ///< DPad or CPad Left
    Right = DRight | CPRight,  ///< DPad or CPad Right
  };
  /** Event */
  enum Event {
    Event_Down,  ///< Key Pressed
    Event_Held,  ///< Key Held
    Event_Up,    ///< Key released
  };
  Hid() = default;
  ~Hid() = default;

  /**
   * Get TOuch Position
   * @return touch pos
   */
  vec2 TouchPos() const { return touch[0]; }
  /**
   * Get Last Touch Position (from last frame)
   * @return touch pos
   */
  vec2 TouchPosLast() const { return touch[1]; }

  /**
   * Check for a Button Event
   * @param e Event Type
   * @param keys Keys to check for
   * @return if key(s) doing the requiested event
   */
  bool IsEvent(Event e, Key keys);
  /**
   * Check for Key Press Event
   * @param keys set of keys
   * @return true if key is pressed
   */
  bool IsDown(Key keys) const { return key_events[0].at(Event_Down) & keys; }
  /**
   * Check for Key Held Event
   * @param keys set of keys
   * @return true if key is held
   */
  bool IsHeld(Key keys) const { return key_events[0].at(Event_Held) & keys; }
  /**
   * Check for Key Release Event
   * @param keys set of keys
   * @return true if key is released
   */
  bool IsUp(Key keys) const { return key_events[0].at(Event_Up) & keys; }

  /**
   * Sett all keyevents to 0
   */
  void Clear() {
    for (int i = 0; i < 2; i++) {
      key_events[i][Event_Down] = 0;
      key_events[i][Event_Up] = 0;
      key_events[i][Event_Held] = 0;
    }
  }

  /**
   * Lock input driver
   * @param v lock or not lock
   */
  void Lock(bool v) {
    if (v != locked) {
      SwappyTable();
    }
    locked = v;
  }
  /**
   * Check if Driver is locked
   * @return true if locked
   */
  bool Locked() const { return locked; }
  /**
   * Lock Input Driver
   */
  void Lock() {
    if (!locked) {
      SwappyTable();
    }
    locked = true;
  }
  /**
   * Unlock Input Driver
   */
  void Unlock() {
    if (locked) {
      SwappyTable();
    }
    locked = false;
  }

  /**
   * Template Update Function for a device specific driver
   */
  virtual void Update() {}

 protected:
  /** Key binds map */
  std::unordered_map<u32, u32> binds;
  /** Function to swap around the Table */
  void SwappyTable();
  /** Using 2 Touch positions for current and last frame */
  vec2 touch[2];
  /** locked state */
  bool locked = false;
  /** Key event tables */
  std::unordered_map<Event, u32> key_events[2];
};
}  // namespace PD