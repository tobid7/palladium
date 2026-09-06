#include <pd/drivers/drivers.hpp>
#include <pd/ultra/container.hpp>
#include <pd/ultra/elems/element.hpp>

namespace PD {
namespace Ultra {
PD_API bool ElementBase::RevisionUpdate(PD::u32 req) {
  if (req != pCanvasRev) {
    pCanvasRev = req;
    return true;
  } else {
    return false;
  }
}
PD_API void ElementBase::Update() {
  if (!pParent)
    pRenderspace = PD::fvec4(pPos, pPos + pSize);
  else
    pRenderspace = pParent->GetCanvas().VTranslateObject(
        pParent->GetTopLeft() + pPos, pSize, pAlignment);
  UpdateInput();
  /*pRenderspace = PD::fvec4(pParent->GetTopLeft() + pPos,
                           pParent->GetTopLeft() + pPos + pSize);*/
}

PD_API void ElementBase::UpdateInput() {
  if (PD::Li::Math::InSpace(
          PD::Hid::MousePos(),
          PD::fvec4(pRenderspace.TopLeft(), pRenderspace.BotRight()))) {
    if (pHover && !pFocued) {
      pHover();
      pFocued = true;
    }
    if (PD::Hid::IsEvent(PD::Hid::Event::Up, PD::Hid::Gamepad::Touch)) {
      if (pPress) {
        pPress();
      }
    }
  } else {
    if (pUnHover && pFocued) {
      pUnHover();
      pFocued = false;
    }
  }
}
}  // namespace Ultra
}  // namespace PD