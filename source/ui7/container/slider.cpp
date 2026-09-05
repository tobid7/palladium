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
#include <pd/ui7/container/slider.hpp>
#include <type_traits>

namespace PD {
namespace UI7 {
// Setup Supported Datatypes (Probably making this Object
// header only to not care about datatype support)
template class PD_API Slider<float>;
template class PD_API Slider<int>;
template class PD_API Slider<double>;
template class PD_API Slider<u8>;
template class PD_API Slider<u16>;
template class PD_API Slider<u32>;
template class PD_API Slider<u64>;
template <typename T>
PD_API void Slider<T>::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  // Assert(screen.get(), "Screen is not set up!");
  std::string p;
  if constexpr (std::is_floating_point_v<T>) {
    p = std::format("{:.{}f}", *data, precision);
  } else {
    p = std::format("{}", *data);
  }
  // Unsafe but is the fastest solution
  float xps = FinalPos().x;
  if (io->InputHandler.DragObject(
          this->GetID(),
          fvec4(FinalPos() + fvec2(2, 0), fvec2(width, GetSize().y)))) {
    if (!io->InputHandler.DragReleasedAW) {
      *data = std::clamp(
          T(max * (std::clamp(io->InputHandler.DragLastPosition.x - xps, 0.f,
                              width) /
                   width)),
          this->min, this->max);
    }
  }
  inp_done = true;
}

template <typename T>
PD_API void Slider<T>::Draw() {
  // Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  // io->Ren->OnScreen(screen);
  std::string p;
  if constexpr (std::is_floating_point_v<T>) {
    p = std::format("{:.{}f}", *data, precision);
  } else {
    p = std::format("{}", *data);
  }
  fvec2 td = io->Font->GetTextBounds(p.c_str(), io->FontScale);
  list->PathRect(FinalPos(), FinalPos() + fvec2(width, td.y) + io->FramePadding,
                 io->FrameRounding);
  list->PathFill(io->Theme.Get(UI7Color_Button));
  list->PathRect(FinalPos() + 2 + PD::fvec2(slp, 0),
                 FinalPos() + fvec2(slp + slw - 2, td.y - 2) + io->FramePadding,
                 io->FrameRounding);
  list->PathFill(io->Theme.Get(UI7Color_ButtonActive));
  // list->LayerUp();
  list->DrawTextEx(FinalPos(), p.c_str(), io->Theme.Get(UI7Color_Text),
                   LiTextFlags_AlignMid, fvec2(width, td.y) + io->FramePadding);
  // list->LayerDown();
  list->DrawText(FinalPos() + fvec2(width + io->FramePadding.x * 2.f,
                                    io->FramePadding.y * 0.5),
                 label.c_str(), io->Theme.Get(UI7Color_Text));
}

template <typename T>
PD_API void Slider<T>::Update() {
  // Assert(io.get(), "Did you run Container::Init correctly?");
  //  Probably need to find a faster solution (caching sizes calculated here)
  slw = std::clamp(static_cast<float>(width / max), 3.f, width);
  slp = static_cast<float>((float)*data / (float)max) * (width - slw);
  fvec2 tdim = io->Font->GetTextBounds(label.c_str(), io->FontScale);

  this->SetSize(
      fvec2(width + tdim.x + io->ItemSpace.x * 2, tdim.y + io->FramePadding.y));
}
}  // namespace UI7
}  // namespace PD