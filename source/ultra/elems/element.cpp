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
  if (!pParent) pRenderspace = PD::fvec4(pPos, pPos + pSize);
  pRenderspace = PD::fvec4(pParent->GetTopLeft() + pPos,
                           pParent->GetTopLeft() + pPos + pSize);
}
}  // namespace Ultra
}  // namespace PD