/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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
PD_API void Checkbox::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  color = UI7Color_FrameBackground;
  /// Ensure it gets sed to false and stays if not pressed
  // Assert(screen.get(), "Screen is not set up!");
  // if (screen->ScreenType() == Screen::Bottom) {
  if (io->InputHandler.DragObject(this->GetID(), fvec4(FinalPos(), size))) {
    if (io->InputHandler.DragReleased) {
      color = UI7Color_FrameBackgroundHovered;
      usr_ref = !usr_ref;
    } else {
      color = UI7Color_FrameBackgroundHovered;
    }
  }
  //}
  inp_done = true;
}
PD_API void Checkbox::Draw() {
  // Assert(list.get() && io.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  list->SetFont(GetFont());
  list->PathRect(FinalPos(), FinalPos() + cbs, io->FrameRounding);
  list->PathFill(io->Theme.Get(color));
  if (usr_ref) {
    list->PathRect(FinalPos() + 2, FinalPos() + cbs - 2, io->FrameRounding);
    list->PathFill(io->Theme.Get(UI7Color_Checkmark));
  }
  list->DrawText(
      FinalPos() + fvec2(cbs.x + io->ItemSpace.x, cbs.y * 0.5 - tdim.y * 0.5),
      label.c_str(), io->Theme.Get(UI7Color_Text));
}

PD_API void Checkbox::Update() {
  // Assert(io.get(), "Did you run Container::Init correctly?");
  cbs = io->ItemRowHeight;
  this->SetSize(cbs + fvec2(tdim.x + io->ItemSpace.x, 0));
}
}  // namespace UI7
}  // namespace PD