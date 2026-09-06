#include <pd/ultra/elems/image.hpp>

namespace PD {
namespace Ultra {
PD_API void Image::Draw(PD::Li::Drawlist& l) {
  l.BindTexture(*pTex);
  l.PathRect(pRenderspace.TopLeft(), pRenderspace.BotRight(), pRounding);
  l.PathFill(pColor);
}
}  // namespace Ultra
}  // namespace PD