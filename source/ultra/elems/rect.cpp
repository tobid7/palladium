#include <pd/ultra/elems/rect.hpp>

namespace PD {
namespace Ultra {
PD_API void Rect::Draw(PD::Li::Drawlist& l) {
  l.PathRect(pPos, pPos + pSize, pRounding);
  if (pLined) {
    l.PathStroke(pColor, pThickness, LiDrawFlags_Close);
  } else {
    l.PathFill(pColor);
  }
}
}  // namespace Ultra
}  // namespace PD