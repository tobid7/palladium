#pragma once

#include <pd/core/core.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
class PD_API Text : public ElementBase {
 public:
  Text() {}
  ~Text() {}

  void Draw(PD::Li::Drawlist& l) override;
  void Update() override;

  void SetColor(const PD::Color& color) { pColor = color; }
  void SetText(const std::string& text) { pText = text; }
  void SetFont(PD::Li::Font& font) { pFont = &font; }
  void SetScale(float scale) { pScale = scale; }

 private:
  PD::Color pColor;
  float pScale = 1.f;
  std::string pText;
  PD::Li::Font* pFont = nullptr;
};
}  // namespace Ultra

}  // namespace PD