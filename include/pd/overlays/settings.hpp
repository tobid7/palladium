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
#include <pd/ui7/ui7.hpp>

namespace PD {
/**
 * Settings Menu Overlay
 */
class SettingsMenu : public Overlay {
 public:
  /**
   * Constructor to setup Overlay
   */
  SettingsMenu() {
    too++;
    if (too > 1) {
      Kill();
      return;
    }
    flymgr.From(vec2(0, 240)).To(vec2(0, 115)).In(0.3f).As(flymgr.EaseInQuad);
  }
  /** Deconstructor */
  ~SettingsMenu() { too--; }

  /** Rendering and Input Handler */
  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

  /**
   * Function to Trigger remove animation
   */
  void Rem() {
    rem = true;
    flymgr.From(vec2(0, 115)).To(vec2(0, 240)).In(0.2f).As(flymgr.EaseOutQuad);
  }

 private:
  /// Section is used to determinate what
  /// should be displayed on the top screen
  int section = 0;
  // Stands for The Only One
  static int too;

  // Some Animation
  bool rem = false;
  Tween<fvec2> flymgr;

  // Custom UI7 Context
  UI7::Context::Ref ctx;
};
}  // namespace PD