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

#include <pd/ui7/container/dragdata.hpp>
#include <pd/ui7/container/label.hpp>
#include <type_traits>

namespace PD {
namespace UI7 {
// Setup Supported Datatypes (Probably making this Object
// header only to not care about datatype support)
template class DragData<float>;
template class DragData<int>;
template class DragData<double>;
template class DragData<u8>;
template class DragData<u16>;
template class DragData<u32>;
template class DragData<u64>;
template <typename T>
void DragData<T>::HandleInput() {
  /// Ensure to only check input once
  if (inp_done) {
    return;
  }
  Assert(screen.get(), "Screen is not set up!");
  if (screen->ScreenType() == Screen::Bottom) {
    float off_x = 0;
    for (size_t i = 0; i < elm_count; i++) {
      std::string p;
      if constexpr (std::is_floating_point_v<T>) {
        p = std::format("{:.{}f}", data[i], precision);
      } else {
        p = std::format("{}", data[i]);
      }
      vec2 tdim = io->Ren->GetTextDimensions(p);
      // Unsafe but is the fastest solution
      if (io->DragObject(
              this->GetID() + i + 1,
              vec4(FinalPos() + vec2(off_x, 0), tdim + io->FramePadding))) {
        data[i] = std::clamp(T(data[i] + (step * (io->DragPosition[0] -
                                                  io->DragLastPosition[0]))),
                             this->min, this->max);
      }
      off_x += tdim.x() + io->ItemSpace.x() + io->FramePadding.x();
    }
  }
  inp_done = true;
}

template <typename T>
void DragData<T>::Draw() {
  Assert(io.get() && list.get(), "Did you run Container::Init correctly?");
  io->Ren->OnScreen(screen);
  float off_x = 0.f;
  for (size_t i = 0; i < elm_count; i++) {
    std::string p;
    if constexpr (std::is_floating_point_v<T>) {
      p = std::format("{:.{}f}", data[i], precision);
    } else {
      p = std::format("{}", data[i]);
    }
    vec2 td = io->Ren->GetTextDimensions(p);
    list->AddRectangle(FinalPos() + vec2(off_x, 0), td + io->FramePadding,
                       io->Theme->Get(UI7Color_Button));
    list->Layer(list->Layer() + 1);
    list->AddText(FinalPos() + vec2(off_x, 0), p, io->Theme->Get(UI7Color_Text),
                  LITextFlags_AlignMid, td + io->FramePadding);
    list->Layer(list->Layer() - 1);
    off_x += td.x() + io->ItemSpace.x() + io->FramePadding.x();
  }
  list->AddText(FinalPos() + vec2(off_x, io->FramePadding.y() * 0.5), label,
                io->Theme->Get(UI7Color_Text));
}

template <typename T>
void DragData<T>::Update() {
  Assert(io.get(), "Did you run Container::Init correctly?");
  // Probably need to find a faster solution (caching sizes calculated here)
  float off_x = 0;
  for (size_t i = 0; i < elm_count; i++) {
    std::string p;
    if constexpr (std::is_floating_point_v<T>) {
      p = std::format("{:.{}f}", data[i], precision);
    } else {
      p = std::format("{}", data[i]);
    }
    vec2 tdim = io->Ren->GetTextDimensions(p);
    off_x += tdim.x() + io->ItemSpace.x() + io->FramePadding.x();
  }
  this->SetSize(vec2(tdim.x() + off_x, tdim.y() + io->FramePadding.y()));
}
}  // namespace UI7
}  // namespace PD