#include <pd/ultra/container.hpp>
#include <pd/ultra/elems/text.hpp>

namespace PD {
namespace Ultra {
PD_API void Text::Draw(PD::Li::Drawlist& l) {
  if (!pFont) return;
  l.SetFont(pFont);
  l.DrawText(pRenderspace.TopLeft(), pText.c_str(), pColor);
}

PD_API void Text::Update() {
  if (!pFont) return;
  if (!pParent) ElementBase::Update();
  pRenderspace = pParent->GetCanvas().VTranslateObject(
      pParent->GetTopLeft() + pPos,
      pFont->GetTextBounds(pText.c_str(),
                           pParent->GetCanvas().VTranslateFontscale(pScale)),
      pAlignment, true);
}
}  // namespace Ultra
}  // namespace PD