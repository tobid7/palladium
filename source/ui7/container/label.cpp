#include <pd/ui7/container/label.hpp>

namespace PD {
namespace UI7 {
void Label::Draw() {
  Assert(ren.get() && list.get() && linked_theme,
         "Did you run Container::Init correctly?");
  ren->OnScreen(screen);
  list->AddText(pos, label, linked_theme->Get(UI7Color_Text));
}
}  // namespace UI7
}  // namespace PD