#include <pd/ui7/container/container.hpp>

namespace PD {
namespace UI7 {
void Container::HandleScrolling(vec2 scrolling, vec4 viewport) {
  pos -= vec2(0, scrolling.y());
  if (!LI::Renderer::InBox(pos, size, viewport)) {
    skippable = true;
  }
}
}  // namespace UI7
}  // namespace PD