#pragma once

#include <pd/overlays/overlay.hpp>
#include <pd/controls/hid.hpp>

namespace PD {
class Performance : public Overlay {
 public:
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
  ~Performance() { too--; }

  void Update(float delta, LI::Renderer::Ref ren, Hid::Ref inp) override;

 private:
  void Line(vec2& pos, const std::string& text, LI::Renderer::Ref ren);
  // Trace String Average
  std::string TSA(const std::string& id);
  // Described in Keyboard
  static int too;
  bool *skill, *screen;
};
}  // namespace PD