#pragma once

#include <pd/common/common.hpp>
#include <pd/controls/hid.hpp>

namespace PD {
namespace GamePadIcons {
enum ID {
  A,
  B,
  X,
  Y,
  L,
  R,
  Dpad,
  Start,
  Select,
  Home,
  Steps,
  PlayCoin,
  AnalogStick,
  Power3DS,
  DpadUp,
  DpadDown,
  DpadLeft,
  DpadRight,
  DpadHorizontal,
  DpadVertical,
};
std::string GetIcon(ID id);
std::string GetIcon(Hid::Key key);
}  // namespace GamePadIcons
}  // namespace PD