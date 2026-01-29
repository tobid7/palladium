#pragma once

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
FITNESS FOR A PARTICULAR PURPHidE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <pd/core/core.hpp>
#include <pd/drivers/types.hpp>

namespace PD {
/** Did not found a better solution yet sadly */
namespace HidKb {
// Lets use u128 here
using KbKey = u128;
constexpr static KbKey Kb_No = 0;
constexpr static KbKey Kb_Escape = KbKey::Flag(0);
constexpr static KbKey Kb_Q = KbKey::Flag(1);
constexpr static KbKey Kb_W = KbKey::Flag(2);
constexpr static KbKey Kb_E = KbKey::Flag(3);
constexpr static KbKey Kb_R = KbKey::Flag(4);
constexpr static KbKey Kb_T = KbKey::Flag(5);
constexpr static KbKey Kb_Z = KbKey::Flag(6);
constexpr static KbKey Kb_U = KbKey::Flag(7);
constexpr static KbKey Kb_I = KbKey::Flag(8);
constexpr static KbKey Kb_O = KbKey::Flag(9);
constexpr static KbKey Kb_P = KbKey::Flag(10);
constexpr static KbKey Kb_A = KbKey::Flag(11);
constexpr static KbKey Kb_S = KbKey::Flag(12);
constexpr static KbKey Kb_D = KbKey::Flag(13);
constexpr static KbKey Kb_F = KbKey::Flag(14);
constexpr static KbKey Kb_G = KbKey::Flag(15);
constexpr static KbKey Kb_H = KbKey::Flag(16);
constexpr static KbKey Kb_J = KbKey::Flag(17);
constexpr static KbKey Kb_K = KbKey::Flag(18);
constexpr static KbKey Kb_L = KbKey::Flag(19);
constexpr static KbKey Kb_Y = KbKey::Flag(20);
constexpr static KbKey Kb_X = KbKey::Flag(21);
constexpr static KbKey Kb_C = KbKey::Flag(22);
constexpr static KbKey Kb_V = KbKey::Flag(23);
constexpr static KbKey Kb_B = KbKey::Flag(24);
constexpr static KbKey Kb_N = KbKey::Flag(25);
constexpr static KbKey Kb_M = KbKey::Flag(26);
constexpr static KbKey Kb_1 = KbKey::Flag(27);
constexpr static KbKey Kb_2 = KbKey::Flag(28);
constexpr static KbKey Kb_3 = KbKey::Flag(29);
constexpr static KbKey Kb_4 = KbKey::Flag(30);
constexpr static KbKey Kb_5 = KbKey::Flag(31);
constexpr static KbKey Kb_6 = KbKey::Flag(32);
constexpr static KbKey Kb_7 = KbKey::Flag(33);
constexpr static KbKey Kb_8 = KbKey::Flag(34);
constexpr static KbKey Kb_9 = KbKey::Flag(35);
constexpr static KbKey Kb_0 = KbKey::Flag(36);
constexpr static KbKey Kb_F1 = KbKey::Flag(37);
constexpr static KbKey Kb_F2 = KbKey::Flag(38);
constexpr static KbKey Kb_F3 = KbKey::Flag(39);
constexpr static KbKey Kb_F4 = KbKey::Flag(40);
constexpr static KbKey Kb_F5 = KbKey::Flag(41);
constexpr static KbKey Kb_F6 = KbKey::Flag(42);
constexpr static KbKey Kb_F7 = KbKey::Flag(43);
constexpr static KbKey Kb_F8 = KbKey::Flag(44);
constexpr static KbKey Kb_F9 = KbKey::Flag(45);
constexpr static KbKey Kb_F10 = KbKey::Flag(46);
constexpr static KbKey Kb_F11 = KbKey::Flag(47);
constexpr static KbKey Kb_F12 = KbKey::Flag(48);
constexpr static KbKey Kb_MouseLeft = KbKey::Flag(120);
}  // namespace HidKb

class PD_API HidDriver {
 public:
  enum Flags : u32 {
    Flags_None = 0,
    FLags_HasGamepad = 1 << 0,
    Flags_HasKeyboard = 1 << 1,
    Flags_HasTouch = 1 << 2,
    Flags_HasMouse = 1 << 3,
  };
  // Todo: Name to GpKey (GamepadKey)
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

  using KbKey = HidKb::KbKey;

  /** Event */
  enum Event {
    Event_Null,
    Event_Down,  ///< Key Pressed
    Event_Held,  ///< Key Held
    Event_Up,    ///< Key released
  };

  HidDriver(const std::string& name = "NullHid") : pName(name) {}
  HidDriver(PDDriverData data) : pName("NullHid") {}
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
  bool IsEvent(Event e, KbKey key);
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
  virtual void Update();
  /**
   * Get Text from Keyboard
   */
  virtual void GetInputStr(std::string& str) {}

  const std::string& GetName() const { return pName; }

  /** Data Section */

  /** Backend Identification Name */
  const std::string pName;

  /** Flags */
  u32 Flags = 0;

  /** Key Binds Map */
  std::unordered_map<u32, u32> pBinds;
  std::unordered_map<u128, u128> pKbBinds;
  /** Swap Tabe Function */
  void SwapTab();
  /** Using 2 Positions for Current and Last */
  fvec2 pMouse[2];
  /** Lock State */
  bool pLocked = false;
  /** Key Event Table Setup */
  std::unordered_map<Event, u32> KeyEvents[2];
  /** Keyboard Key Event Table Setup */
  std::unordered_map<Event, u128> KbKeyEvents[2];
};
namespace Hid {
using Event = HidDriver::Event;
using Key = HidDriver::Key;
}  // namespace Hid
}  // namespace PD