#include <pd/ui7/container/checkbox.hpp>

namespace PD {
namespace UI7 {
void Checkbox::HandleInput(Hid::Ref inp) {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  color = UI7Color_FrameBackground;
  /// Ensure it gets sed to false and stays if not pressed
  Assert(screen.get(), "Screen is not set up!");
  if (screen->ScreenType() == Screen::Bottom) {
    if (inp->IsHeld(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPos(), vec4(pos, size))) {
      color = UI7Color_FrameBackgroundHovered;
    }
    if (inp->IsUp(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPosLast(), vec4(pos, size))) {
      color = UI7Color_FrameBackgroundHovered;
      usr_ref = !usr_ref;
    }
  }
  inp_done = true;
}
void Checkbox::Draw() {
  Assert(ren.get() && list.get() && linked_theme,
         "Did you run Container::Init correctly?");
  ren->OnScreen(screen);
  list->AddRectangle(pos, cbs, linked_theme->Get(color));
  if (usr_ref) {
    list->AddRectangle(pos + 2, cbs - 4, linked_theme->Get(UI7Color_Checkmark));
  }
  list->AddText(pos + vec2(cbs.x() + 5, cbs.y() * 0.5 - tdim.y() * 0.5), label,
                linked_theme->Get(UI7Color_Text));
}
}  // namespace UI7
}  // namespace PD