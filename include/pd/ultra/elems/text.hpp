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

  void SetPosition(const PD::fvec2& pos) { pPos = pos; }
  void SetPosition(float x, float y) { pPos = PD::fvec2(x, y); }
  void SetColor(const PD::Color& color) { pColor = color; }
  void SetText(const std::string& text) { pText = text; }

 private:
  PD::fvec2 pPos;
  PD::Color pColor;
  std::string pText;
};
}  // namespace Ultra

}  // namespace PD