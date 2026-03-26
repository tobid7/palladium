#include <pd/ultra/elems/text.hpp>

namespace PD {
namespace Ultra {
PD_API void Text::Draw(PD::Li::Drawlist& l) {
  l.DrawText(pPos, pText.c_str(), pColor);
}
}  // namespace Ultra
}  // namespace PD