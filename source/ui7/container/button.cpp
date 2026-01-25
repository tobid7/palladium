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

#include <pd/ui7/container/button.hpp>

namespace PD {
namespace UI7 {
PD_UI7_API void Button::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  /// Ensure it gets sed to false and stays if not pressed
  pressed = false;
  color = UI7Color_Button;
  // Assert(screen.get(), "Screen is not set up!");
  // if (screen->ScreenType() == Screen::Bottom) {
  if (io->InputHandler->DragObject(this->GetID(), fvec4(FinalPos(), size))) {
    if (io->InputHandler->DragReleased) {
      color = UI7Color_ButtonActive;
      pressed = true;
    } else {
      color = UI7Color_ButtonHovered;
    }
  }
  //}
  inp_done = true;
}
PD_UI7_API void Button::Draw() {
  // Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  list->PathRect(FinalPos(), FinalPos() + size, io->FrameRounding);
  list->PathFill(io->Theme->Get(color));
  list->LayerUp();
  list->DrawText(FinalPos() + size * 0.5 - tdim * 0.5, label,
                 io->Theme->Get(UI7Color_Text));
  list->LayerDown();
}

PD_UI7_API void Button::Update() {
  // Assert(io.get(), "Did you run Container::Init correctly?");
  this->SetSize(tdim + io->FramePadding);
}
}  // namespace UI7
}  // namespace PD