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
#include <pd/ui7/container/label.hpp>

namespace PD {
namespace UI7 {
PD_UI7_API void ColorEdit::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  // Assert(screen.get(), "Screen is not set up!");
  // if (screen->ScreenType() == Screen::Bottom) {
  if (io->InputHandler->DragObject(this->GetID(), vec4(FinalPos(), size))) {
    if (io->InputHandler->DragReleased) {
      is_shown = !is_shown;
    }
  }
  //}
  inp_done = true;
}
PD_UI7_API void ColorEdit::Draw() {
  // Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  list->AddRectangle(FinalPos(), fvec2(20, 20), *color_ref);
  list->AddText(FinalPos() + fvec2(io->ItemSpace.x + 20, 0), label,
                io->Theme->Get(UI7Color_Text));
  if (is_shown) {
    if (!layout) {
      layout = Layout::New(GetID(), io);
    }
    layout->AddObject(PD::New<Label>("Hello World!", io));
    layout->Update();
    io->RegisterDrawList(GetID(), layout->GetDrawList());
  }
}

PD_UI7_API void ColorEdit::Update() {
  // Assert(io.get(), "Did you run Container::Init correctly?");
  this->SetSize(fvec2(tdim.x + io->ItemSpace.x + 20, 20));
}
}  // namespace UI7
}  // namespace PD