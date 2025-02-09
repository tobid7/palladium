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

#include <pd/common/app.hpp>
#include <pd/controls/hid.hpp>
#include <pd/maths/tween.hpp>
#include <pd/overlays/overlay.hpp>
#include <pd/ui7/ui7.hpp>

namespace PD {
class SettingsMenu : public Overlay {
 public:
  SettingsMenu(PD::App* app) {
    too++;
    if (too > 1) {
      Kill();
      return;
    }
    app_ref = app;
    app->FeatureDisable(PD::App::AppFLags_UserLoop);
    flymgr.From(vec2(0, 240)).To(vec2(0, 115)).In(0.3f).As(flymgr.EaseInQuad);
  }
  ~SettingsMenu() { too--; }

  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

  void Rem() {
    rem = true;
    app_ref->FeatureEnable(App::AppFLags_UserLoop);
    flymgr.From(vec2(0, 115)).To(vec2(0, 240)).In(0.2f).As(flymgr.EaseOutQuad);
  }

 private:
  PD::App* app_ref = nullptr;
  /// Section is used to determinate what
  /// should be displayed on the top screen
  int section = 0;
  // Stands for The Only One
  static int too;

  // Some Animation
  bool rem = false;
  Tween<vec2> flymgr;

  // Custom UI7 Context
  UI7::Context::Ref ctx;
};
}  // namespace PD