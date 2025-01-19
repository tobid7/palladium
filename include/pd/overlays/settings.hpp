#pragma once

#include <pd/maths/tween.hpp>
#include <pd/overlays/overlay.hpp>
#include <pd/controls/hid.hpp>

namespace PD {
class SettingsMenu : public Overlay {
 public:
  SettingsMenu() {
    too++;
    if (too > 1) {
      Kill();
      return;
    }
    flymgr.From(vec2(0, 240)).To(vec2(0, 115)).In(0.3f).As(flymgr.EaseInQuad);
  }
  ~SettingsMenu() { too--; }

  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

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
  Tween<vec2> flymgr;
};
}  // namespace PD