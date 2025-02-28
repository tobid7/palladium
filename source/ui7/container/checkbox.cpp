/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

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
  Assert(ren.get() && list.get() && theme,
         "Did you run Container::Init correctly?");
  ren->OnScreen(screen);
  list->AddRectangle(pos, cbs, theme->Get(color));
  if (usr_ref) {
    list->AddRectangle(pos + 2, cbs - 4, theme->Get(UI7Color_Checkmark));
  }
  list->AddText(pos + vec2(cbs.x() + 5, cbs.y() * 0.5 - tdim.y() * 0.5), label,
                theme->Get(UI7Color_Text));
}
}  // namespace UI7
}  // namespace PD