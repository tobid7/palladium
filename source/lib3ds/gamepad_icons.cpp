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

#include <pd/lib3ds/gamepad_icons.hpp>

namespace PD {
namespace GamePadIcons {
/// For Reference see docs at 3DBrew
std::unordered_map<ID, std::string> gp_icons{
    {A, "\uE000"},
    {B, "\uE001"},
    {X, "\uE002"},
    {Y, "\uE003"},
    {L, "\uE004"},
    {R, "\uE005"},
    {Dpad, "\uE006"},
    {Start, "\uE045"},
    {Select, "\uE046"},
    {Home, "\uE073"},
    {Steps, "\uE074"},
    {PlayCoin, "\uE075"},
    {AnalogStick, "\uE077"},
    {Power3DS, "\uE078"},
    {DpadUp, "\uE079"},
    {DpadDown, "\uE07A"},
    {DpadLeft, "\uE07B"},
    {DpadRight, "\uE07C"},
    {DpadHorizontal, "\uE07D"},
    {DpadVertical, "\uE07E"},
};
std::unordered_map<Hid::Key, ID> gp_link{
    {Hid::A, ID::A},
    {Hid::B, ID::B},
    {Hid::X, ID::X},
    {Hid::Y, ID::Y},
    {Hid::L, ID::L},
    {Hid::R, ID::R},
    {Hid::Start, ID::Start},
    {Hid::Select, ID::Select},
    {Hid::DUp, ID::DpadUp},
    {Hid::DDown, ID::DpadDown},
    {Hid::DLeft, ID::DpadLeft},
    {Hid::DRight, ID::DpadRight},
};
std::string GetIcon(ID id) {
  auto e = gp_icons.find(id);
  if (e == gp_icons.end()) {
    return "";
  }
  return e->second;
}
std::string GetIcon(Hid::Key key) {
  auto e = gp_link.find(key);
  if (e == gp_link.end()) {
    return "";
  }
  return gp_icons[gp_link[key]];
}
}  // namespace GamePadIcons
}  // namespace PD
