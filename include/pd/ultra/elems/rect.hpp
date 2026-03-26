#pragma once

#include <pd/core/core.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Rect : public ElementBase {
 public:
  Rect() {}
  ~Rect() {}

  void Draw(PD::Li::Drawlist& l) override;

  void SetPosition(const PD::fvec2& pos) { pPos = pos; }
  void SetPosition(float x, float y) { pPos = PD::fvec2(x, y); }
  void SetSize(const PD::fvec2& size) { pSize = size; }
  void SetSize(float w, float h) { pSize = PD::fvec2(w, h); }
  void SetColor(const PD::Color& color) { pColor = color; }
  void SetRounding(float r) { pRounding = r; }

 private:
  PD::fvec2 pPos;
  PD::fvec2 pSize;
  PD::Color pColor;
  float pRounding = 0.f;
};
}  // namespace Ultra

}  // namespace PD