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
#include <pd/drivers/hid.hpp>

namespace PD {
/**
 * Simple Table Containing the codepoint references
 * for the Controller Icons on the 3ds
 */
namespace GamePadIcons {
/**
 * Icon ID
 */
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
/**
 * Get Icon by ID
 * @param id ID to Get
 * @return codepoint
 */
std::string GetIcon(ID id);
/**
 * Get Icon By Input Driver Key
 * @param key Key to find
 * @return codepoint
 */
std::string GetIcon(Hid::Key key);
}  // namespace GamePadIcons
}  // namespace PD