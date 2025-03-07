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
#include <pd/core/timer.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/menu.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
/**
 * Shared Configuration and Runtime Data for a UI7 Context
 */
class IO : public SmartCtor<IO> {
 public:
  /**
   * IO Constructor setting UP References
   */
  IO(Hid::Ref input_driver) {
    Time = Timer::New();
    Theme = UI7::Theme::New();
    Inp = input_driver;
  };
  ~IO() = default;

  float Framerate = 0.f;
  float Delta = 0.f;
  Timer::Ref Time;
  Hid::Ref Inp;
  UI7::Theme::Ref Theme;
  float MenuPadding = 5.f;
  DrawList::Ref Back;
  DrawList::Ref Front;
};
}  // namespace UI7
}  // namespace PD