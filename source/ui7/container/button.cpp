#include <pd/ui7/container/button.hpp>

namespace PD {
namespace UI7 {
void Button::HandleInput(Hid::Ref inp) {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  /// Ensure it gets sed to false and stays if not pressed
  pressed = false;
  color = UI7Color_Button;
  Assert(screen.get(), "Screen is not set up!");
  if (screen->ScreenType() == Screen::Bottom) {
    if (inp->IsHeld(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPos(), vec4(pos, size))) {
      color = UI7Color_ButtonHovered;
    }
    if (inp->IsUp(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPosLast(), vec4(pos, size))) {
      color = UI7Color_ButtonActive;
      pressed = true;
    }
  }
  inp_done = true;
}
void Button::Draw() {
  Assert(ren.get() && list.get() && linked_theme,
         "Did you run Container::Init correctly?");
  ren->OnScreen(screen);
  list->AddRectangle(pos, size, linked_theme->Get(color));
  list->AddText(pos + size * 0.5 - tdim * 0.5, label,
                linked_theme->Get(UI7Color_Text));
}
}  // namespace UI7
}  // namespace PD