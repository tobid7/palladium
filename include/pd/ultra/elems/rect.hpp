#pragma once

#include <pd/core/core.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Rect : public ElementBase {
 public:
  Rect() {}
  Rect(const PD::fvec2& pos, const PD::fvec2& size, const PD::Color& color,
       float rounding = 0.f, UltraAlignment align = 0)
      : pColor(color), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = pos;
    this->pSize = size;
  }
  Rect(float x, float y, float w, float h, const PD::Color& color,
       float rounding = 0.f, UltraAlignment align = 0)
      : pColor(color), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = PD::fvec2(x, y);
    this->pSize = PD::fvec2(w, h);
  }
  ~Rect() {}

  void Draw(PD::Li::Drawlist& l) override;

  void SetColor(const PD::Color& color) { pColor = color; }
  void SetRounding(float r) { pRounding = r; }
  void SetLined(bool v) { pLined = v; }
  void SetThickness(int v) { pThickness = v; }

 private:
  PD::Color pColor;
  float pRounding = 0.f;
  bool pLined = false;
  int pThickness = 1.f;
};
}  // namespace Ultra

}  // namespace PD