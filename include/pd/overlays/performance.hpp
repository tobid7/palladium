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

#include <pd/core/core.hpp>
#include <pd/overlays/overlay.hpp>

namespace PD {
/**
 * Performance Overlay
 *
 * - Framerate / Frametime
 * - Renderer Avarage Time
 * - Overlays Avarage Time
 * - UserApp Average Time
 * - **V**ertices and **I**ndices
 * - **D**Draw Commands and Draw **C**alls
 * - Text Map System Texts
 * - Auto static Text Texts
 */
class Performance : public Overlay {
 public:
  /**
   * Constructor
   * @param skill (if set to true the Overlay self kills)
   * @param screen Bottom or Top Screen
   */
  Performance(bool& skill, bool& screen) {
    too++;
    if (too > 1) {
      Kill();
      return;
    }
    this->skill = &skill;
    *this->skill = false;  // Make sure its false
    this->screen = &screen;
  }
  /** Deconstructor */
  ~Performance() { too--; }

  /**
   * Rendering Function
   * @param delta Deltatime
   * @param ren Renderer Reference
   * @param inp Input Driver Reference
   */
  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

 private:
  /**
   * Render an Info line
   * @param pos Position reference (gets updated for next line)
   * @param text Text to Show
   * @param ren Renderer Reference
   */
  void Line(fvec2& pos, const std::string& text, LI::Renderer::Ref ren);
  // Trace String Average
  std::string TSA(const std::string& id);
  // Described in Keyboard
  static int too;
  bool *skill, *screen;
};
}  // namespace PD