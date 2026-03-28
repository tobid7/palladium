#include <pd/ultra/canvas.hpp>
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
PD_API void ElementBase::Update(Canvas& c) {
  if (RevisionUpdate(c.GetRevision())) {
    if (pParent) {
      pRenderspace = c.VTranslateObject(
          pParent->pRenderspace.TopLeft(),
          pRenderspace.BotRight() - pRenderspace.TopLeft(), pAlignment, true);
    } else {
      pRenderspace = c.VTranslateObject(pPos, pSize, pAlignment);
    }
  }
}
}  // namespace Ultra
}  // namespace PD