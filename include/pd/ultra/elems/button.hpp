#pragma once

#include <pd/core/core.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Button : public ElementBase {
 public:
  Button() {}
  Button(const PD::fvec2& pos, const PD::fvec2& size, const PD::Color& color,
         float rounding = 0.f, UltraAlignment align = 0)
      : pColor(color), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = pos;
    this->pSize = size;
  }
  Button(float x, float y, float w, float h, const PD::Color& color,
         float rounding = 0.f, UltraAlignment align = 0)
      : pColor(color), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = PD::fvec2(x, y);
    this->pSize = PD::fvec2(w, h);
  }
  ~Button() {}

  void Draw(PD::Li::Drawlist& l) override;
  void Update() override;
  void SetColor(const PD::Color& color) { pColor = color; }
  void SetFocusedColor(const PD::Color& color) { pHovered = color; }
  void SetTextColor(const PD::Color& color) { pTextColor = color; }
  void SetRounding(float r) { pRounding = r; }
  void SetLined(bool v) { pLined = v; }
  void SetThickness(int v) { pThickness = v; }
  void SetText(const std::string& text) { pText = text; }
  void SetAutoSizePadding(const PD::fvec2& size) { pAsp = size; }

  // Discard these funcs
  void OnFocus(EventFunc func) override {}
  void OnUnFocus(EventFunc func) override {}

 private:
  std::string pText;
  PD::Color pRenderColor;
  PD::Color pColor;
  PD::Color pHovered;
  PD::Color pTextColor;
  float pRounding = 0.f;
  bool pLined = false;
  int pThickness = 1.f;
  // Auto-Size-Padding
  PD::fvec2 pAsp = PD::fvec2(30, 10);
};
}  // namespace Ultra

}  // namespace PD