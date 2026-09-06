#pragma once

#include <pd/core/core.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Image : public ElementBase {
 public:
  Image() {}
  Image(PD::Li::Texture* tex, const PD::fvec2& pos, float rounding = 0.f,
        UltraAlignment align = 0)
      : pColor(0xffffffff), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = pos;
    SetTexture(tex);
  }
  Image(PD::Li::Texture* tex, float x, float y, float rounding = 0.f,
        UltraAlignment align = 0)
      : pColor(0xffffffff), pRounding(rounding) {
    this->pAlignment = align;
    this->pPos = PD::fvec2(x, y);
    SetTexture(tex);
  }
  ~Image() {}

  void Draw(PD::Li::Drawlist& l) override;

  void SetColor(const PD::Color& color) { pColor = color; }
  void SetRounding(float r) { pRounding = r; }
  void SetTexture(PD::Li::Texture* tex) {
    pTex = tex;
    SetSize(pTex->GetSize());
  }

 private:
  PD::Color pColor = 0xffffffff;
  PD::Li::Texture* pTex = nullptr;
  float pRounding = 0.f;
};
}  // namespace Ultra

}  // namespace PD