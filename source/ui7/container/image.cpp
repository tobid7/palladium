#include <pd/ui7/container/image.hpp>

namespace PD {
namespace UI7 {
void Image::Draw() {
  Assert(ren.get() && list.get() && linked_theme,
         "Did you run Container::Init correctly?");
  Assert(img.get(), "Image is nullptr!");
  ren->OnScreen(screen);
  list->AddImage(pos, img);
}
}  // namespace UI7
}  // namespace PD