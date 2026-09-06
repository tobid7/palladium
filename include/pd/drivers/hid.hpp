#pragma once

#include <pd/core/core.hpp>
#include <pd/drivers/interface.hpp>

using PDHidBackendFlags = PD::u32;
enum PDHidBackendFlags_ {
  PDHidBackendFlags_None = 0,
  PDHidBackendFlags_HasTouch = 1 << 1,
  PDHidBackendFlags_HasGamepad = 1 << 2,
  PDHidBackendFlags_HasMouse = 1 << 3,
  PDHidBackendFlags_HasKeyboard = 1 << 4,
};

namespace PD {
namespace HidInternal {
class Keyboard {
 public:
  Keyboard() = default;
  virtual ~Keyboard() = default;
  using Key = u128;
  constexpr static Key No = 0;
  constexpr static Key Escape = Key::Flag(0);
  constexpr static Key Q = Key::Flag(1);
  constexpr static Key W = Key::Flag(2);
  constexpr static Key E = Key::Flag(3);
  constexpr static Key R = Key::Flag(4);
  constexpr static Key T = Key::Flag(5);
  constexpr static Key Z = Key::Flag(6);
  constexpr static Key U = Key::Flag(7);
  constexpr static Key I = Key::Flag(8);
  constexpr static Key O = Key::Flag(9);
  constexpr static Key P = Key::Flag(10);
  constexpr static Key A = Key::Flag(11);
  constexpr static Key S = Key::Flag(12);
  constexpr static Key D = Key::Flag(13);
  constexpr static Key F = Key::Flag(14);
  constexpr static Key G = Key::Flag(15);
  constexpr static Key H = Key::Flag(16);
  constexpr static Key J = Key::Flag(17);
  constexpr static Key K = Key::Flag(18);
  constexpr static Key L = Key::Flag(19);
  constexpr static Key Y = Key::Flag(20);
  constexpr static Key X = Key::Flag(21);
  constexpr static Key C = Key::Flag(22);
  constexpr static Key V = Key::Flag(23);
  constexpr static Key B = Key::Flag(24);
  constexpr static Key N = Key::Flag(25);
  constexpr static Key M = Key::Flag(26);
  constexpr static Key _1 = Key::Flag(27);
  constexpr static Key _2 = Key::Flag(28);
  constexpr static Key _3 = Key::Flag(29);
  constexpr static Key _4 = Key::Flag(30);
  constexpr static Key _5 = Key::Flag(31);
  constexpr static Key _6 = Key::Flag(32);
  constexpr static Key _7 = Key::Flag(33);
  constexpr static Key _8 = Key::Flag(34);
  constexpr static Key _9 = Key::Flag(35);
  constexpr static Key _0 = Key::Flag(36);
  constexpr static Key F1 = Key::Flag(37);
  constexpr static Key F2 = Key::Flag(38);
  constexpr static Key F3 = Key::Flag(39);
  constexpr static Key F4 = Key::Flag(40);
  constexpr static Key F5 = Key::Flag(41);
  constexpr static Key F6 = Key::Flag(42);
  constexpr static Key F7 = Key::Flag(43);
  constexpr static Key F8 = Key::Flag(44);
  constexpr static Key F9 = Key::Flag(45);
  constexpr static Key F10 = Key::Flag(46);
  constexpr static Key F11 = Key::Flag(47);
  constexpr static Key F12 = Key::Flag(48);
  constexpr static Key MouseLeft = Key::Flag(120);
};

using GamepadKey = u32;
enum Gamepad : GamepadKey {
  None = 0,                  ///< No Key
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
  LStick = 1 << 23,          ///< Left Stick
  RStick = 1 << 24,          ///< Right Stick
  Up = DUp | CPUp,           ///< DPad or CPad Up
  Down = DDown | CPDown,     ///< DPad or CPad Down
  Left = DLeft | CPLeft,     ///< DPad or CPad Left
  Right = DRight | CPRight,  ///< DPad or CPad Right
};
}  // namespace HidInternal
// Pre interface class
class PD_API HidDriver : public DriverInterface {
 public:
  enum class Event {
    Null,  ///< Nothing happended
    Down,  ///< Key Pressed
    Held,  ///< Key held
    Up,    ///< Key released
  };
  HidDriver(std::string_view name = "HidNull");
  virtual ~HidDriver();

  virtual void Init() {}
  virtual void Deinit() {}
  virtual const fvec2& MousePos() const { return pMouse[0]; }
  virtual const fvec2& MousePosLast() const { return pMouse[1]; }
  virtual const fvec2& TouchPos() const { return pMouse[0]; }
  virtual const fvec2& TouchPosLast() const { return pMouse[1]; }
  virtual void Update();
  virtual bool IsEvent(Event e, HidInternal::GamepadKey keys);
  virtual bool IsEvent(Event e, HidInternal::Keyboard::Key keys);

  PDHidBackendFlags GetFlags() const { return pFlags; }

 protected:
  void SwapTab();
  PDHidBackendFlags pFlags = PDHidBackendFlags_None;
  fvec2 pMouse[2];  // Current And last pos
  std::unordered_map<u32, u32> pGamepad;
  std::unordered_map<u128, u128> pKeyboard;
  std::unordered_map<Event, u32> pGamepadEvents[2];
  std::unordered_map<Event, u128> pKeyboardEvents[2];
};

class PD_API Hid {
 public:
  using Gamepad = HidInternal::Gamepad;
  using Keyboard = HidInternal::Keyboard;
  using Event = HidDriver::Event;
  Hid() = default;
  ~Hid() = default;

  template <typename T, typename... Args>
  static void UseDriver(Args&&... args) {
    // assert(driver == nullptr && "OS Driver already set");
    driver = std::make_unique<T>(std::forward<Args>(args)...);
  }

  static void Init() { driver->Init(); }
  static void Deinit() { driver->Deinit(); }
  static const fvec2& MousePos() { return driver->MousePos(); }
  static const fvec2& MousePosLast() { return driver->MousePosLast(); }
  static const fvec2& TouchPos() { return driver->TouchPos(); }
  static const fvec2& TouchPosLast() { return driver->TouchPosLast(); }
  static void Update() { driver->Update(); }
  static bool IsEvent(Event e, HidInternal::GamepadKey keys) {
    return driver->IsEvent(e, keys);
  }
  static bool IsEvent(Event e, HidInternal::Keyboard::Key keys) {
    return driver->IsEvent(e, keys);
  }
  static PDHidBackendFlags GetFlags() { return driver->GetFlags(); }

  static const char* GetDriverName() { return driver->GetName(); }

 private:
  static std::unique_ptr<HidDriver> driver;
};
}  // namespace PD