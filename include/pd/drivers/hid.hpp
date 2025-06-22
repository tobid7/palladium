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
FITNESS FOR A PARTICULAR PURPHidE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/core/core.hpp>

namespace PD {
class HidDriver {
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

  HidDriver(const std::string& name = "NullHid") : pName(name) {};
  virtual ~HidDriver() = default;
  PD_SHARED(HidDriver);

  /**
   * Get Mouse Position
   * @return Mouse pos
   */
  fvec2 MousePos() const {
    return pMouse[0];
  } /**
     * Get Last Mouse Position (from last frame)
     * @return Mouse pos
     */
  fvec2 MousePosLast() const { return pMouse[1]; }

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
  bool IsDown(Key keys) const { return KeyEvents[0].at(Event_Down) & keys; }
  /**
   * Check for Key Held Event
   * @param keys set of keys
   * @return true if key is held
   */
  bool IsHeld(Key keys) const { return KeyEvents[0].at(Event_Held) & keys; }
  /**
   * Check for Key Release Event
   * @param keys set of keys
   * @return true if key is released
   */
  bool IsUp(Key keys) const { return KeyEvents[0].at(Event_Up) & keys; }

  /**
   * Sett all keyevents to 0
   */
  void Clear() {
    for (int i = 0; i < 2; i++) {
      KeyEvents[i][Event_Down] = 0;
      KeyEvents[i][Event_Up] = 0;
      KeyEvents[i][Event_Held] = 0;
    }
  }

  /**
   * Lock input driver
   * @param v lock or not lock
   */
  void Lock(bool v) {
    if (v != pLocked) {
      SwapTab();
    }
    pLocked = v;
  }

  /**
   * Check if Driver is locked
   * @return true if locked
   */
  bool Locked() const { return pLocked; }

  /**
   * Lock Input Driver
   */
  void Lock() {
    if (!pLocked) {
      SwapTab();
    }
    pLocked = true;
  }
  /**
   * Unlock Input Driver
   */
  void Unlock() {
    if (pLocked) {
      SwapTab();
    }
    pLocked = false;
  }

  /**
   * Template Update Function for a device specific driver
   */
  virtual void Update() {}

  /** Data Section */

  /** Backend Identification Name */
  const std::string pName;

  /** Key Binds Map */
  std::unordered_map<u32, u32> pBinds;
  /** Swap Tabe Function */
  void SwapTab();
  /** Using 2 Positions for Current and Last */
  fvec2 pMouse[2];
  /** Lock State */
  bool pLocked = false;
  /** Key Event Table Setup */
  std::unordered_map<Event, u32> KeyEvents[2];
};

/** Static Hid Controller */
class Hid {
 public:
  Hid() = default;
  ~Hid() = default;

  /** Referenec to Drivers enums */
  using Key = HidDriver::Key;
  using Event = HidDriver::Event;

  static void Init(HidDriver::Ref v = nullptr) {
    if (v) {
      pHid = v;
    } else {
      pHid = HidDriver::New();
    }
  }

  static bool IsEvent(Event e, Key keys) { return pHid->IsEvent(e, keys); }
  static bool IsDown(Key keys) { return pHid->IsDown(keys); }
  static bool IsUp(Key keys) { return pHid->IsUp(keys); }
  static bool IsHeld(Key keys) { return pHid->IsHeld(keys); }
  static fvec2 MousePos() { return pHid->MousePos(); }
  static fvec2 MousePosLast() { return pHid->MousePosLast(); }
  static void Clear() { pHid->Clear(); }
  static void Lock() { pHid->Lock(); }
  static void Lock(bool v) { pHid->Lock(v); }
  static void Unlock() { pHid->Unlock(); }
  static bool Locked() { return pHid->Locked(); }
  static void Update() { pHid->Update(); }

  static HidDriver::Ref pHid;
};
}  // namespace PD