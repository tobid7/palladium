#include <pd/ultra/elems/rect.hpp>

namespace PD {
namespace Ultra {
PD_API void Rect::Draw(PD::Li::Drawlist& l) {
  l.PathRect(pRenderspace.TopLeft(), pRenderspace.BotRight(), pRounding);
  l.PathFill(pColor);
  l.DrawText(pRenderspace.BotRight(), std::to_string(pCanvasRev).c_str(),
             0xff0000ff);
}
}  // namespace Ultra
}  // namespace PD