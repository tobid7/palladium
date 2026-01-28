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

#include <pd/ui7/container/label.hpp>

namespace PD {
namespace UI7 {
PD_API void Label::Draw() {
  // Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  list->DrawTextEx(FinalPos(), label, io->Theme->Get(UI7Color_Text),
                   LiTextFlags_NoOOS, PD::fvec2(0, io->CurrentViewPort.w));
}

PD_API void Label::Update() {
  /**
   * Todo: This is a hacky workaround
   * Needs proper optimisation
   * Needs a max size (to support sligning dynaically by the window size)
   */
  if (io->WrapLabels) {
    this->label =
        io->Font->pWrapText(this->label, io->FontScale,
                            PD::fvec2(io->CurrentViewPort.z - FinalPos().x * 4,
                                      io->CurrentViewPort.w),
                            this->tdim);
    SetSize(tdim);
  }
}
}  // namespace UI7
}  // namespace PD