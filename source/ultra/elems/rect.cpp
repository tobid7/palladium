#include <pd/ultra/elems/rect.hpp>

namespace PD {
namespace Ultra {
PD_API void Rect::Draw(PD::Li::Drawlist& l) {
  l.PathRect(pPos, pPos + pSize, pRounding);
  l.PathFill(pColor);
}
}  // namespace Ultra
}  // namespace PD