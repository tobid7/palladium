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

#include <pd/ui7/container/coloredit.hpp>

namespace PD {
namespace UI7 {
void ColorEdit::HandleInput(Hid::Ref inp) {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  Assert(screen.get(), "Screen is not set up!");
  if (screen->ScreenType() == Screen::Bottom) {
    if (io->DragObject(this->GetID(), vec4(FinalPos(), size))) {
      if (io->DragReleased) {
      }
    }
  }
  inp_done = true;
}
void ColorEdit::Draw() {
  Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  io->Ren->OnScreen(screen);
  list->AddRectangle(FinalPos(), vec2(20, 20), *color_ref);
  list->AddText(FinalPos() + vec2(io->ItemSpace.x() + 20, 0), label,
                io->Theme->Get(UI7Color_Text));
}

void ColorEdit::Update() {
  Assert(io.get(), "Did you run Container::Init correctly?");
  this->SetSize(vec2(tdim.x() + io->ItemSpace.x() + 20, 20));
}
}  // namespace UI7
}  // namespace PD