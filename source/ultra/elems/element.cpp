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
}  // namespace Ultra
}  // namespace PD