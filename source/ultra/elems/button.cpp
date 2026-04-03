#include <pd/drivers/drivers.hpp>
#include <pd/ultra/container.hpp>
#include <pd/ultra/elems/button.hpp>

namespace PD {
namespace Ultra {
PD_API void Button::Draw(PD::Li::Drawlist& l) {
  float r = pRounding;
  if (pParent) r = pParent->GetCanvas().VTranslateSize(r).x;
  l.PathRect(pRenderspace.TopLeft(), pRenderspace.BotRight(), r);
  if (pLined) {
    l.PathStroke(pRenderColor, pThickness, LiDrawFlags_Close);
  } else {
    l.PathFill(pRenderColor);
  }
  if (!pFont) return;  // discard here if we dont have a font
  PD::fvec2 off = 0.f;
  if (pParent) off = pParent->GetCanvas().VTranslateSize(pAsp) * 0.5;
  l.SetFont(pFont);
  l.SetFontscale(pParent->GetCanvas().VTranslateFontscale(pFontScale));
  l.DrawText(pRenderspace.TopLeft() + off, pText.c_str(), pTextColor);
}

PD_API void Button::Update() {
  pRenderColor = pColor;
  if (!pParent || !pFont) ElementBase::Update();
  PD::fvec2 size = pSize;
  if (size == PD::fvec2(0)) {
    size = pFont->GetTextBounds(
               pText.c_str(),
               pParent->GetCanvas().VTranslateFontscale(pFontScale)) +
           pParent->GetCanvas().VTranslateSize(pAsp);
  } else {
    size = pParent->GetCanvas().VTranslateSize(size);
  }
  pRenderspace = pParent->GetCanvas().VTranslateObject(
      pParent->GetTopLeft() + pPos, size, pAlignment, true);
  if (PD::Li::Math::InSpace(PD::Hid::MousePos(), pRenderspace)) {
    pRenderColor = pHovered;
  } else {
    pRenderColor = pColor;
  }
  UpdateInput();
}
}  // namespace Ultra
}  // namespace PD