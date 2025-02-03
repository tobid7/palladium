#include <pd/common/sys.hpp>
#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
void Container::HandleScrolling(vec2 scrolling, vec4 viewport) {
  if (last_use != 0 && Sys::GetTime() - last_use > 5000) {
    rem = true;
  }
  last_use = Sys::GetTime();
  pos -= vec2(0, scrolling.y());
  if (!LI::Renderer::InBox(pos, size, viewport)) {
    skippable = true;
  }
}
}  // namespace UI7
}  // namespace PD