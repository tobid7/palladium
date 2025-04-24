#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/ui7/layout.hpp>

namespace PD {
namespace UI7 {
/**
 * ReMenu (Should get something like MenuBase or so)
 * to define basic functionality and extend it by using this as a
 * template class
 */
class PD_UI7_API ReMenu {
 public:
  ReMenu(const UI7::ID& id, UI7::IO::Ref io) : pID(id) {
    pLayout = UI7::Layout::New(id, io);
    pIO = io;
    TitleBarHeight = io->FontScale * 30.f;
    pLayout->WorkRect.y += TitleBarHeight;
    pLayout->CursorInit();
  }
  ~ReMenu() = default;
  /** Using the Legacy version here */
  PD_SMART_CTOR(ReMenu)

  /**
   * Render a Simple Label
   * @param label The text to draw
   */
  void Label(const std::string& label);
  /**
   * Render a Button
   * @param label The buttons text
   * @return if the button was pressed
   */
  bool Button(const std::string& label);
  /**
   * Render a Checkbox
   * @param label Label of the Checkbox
   * @param v A value to update
   */
  void Checkbox(const std::string& label, bool& v);
  /**
   * Render an Image
   * @param img Texture reference of the image
   * @param size a Custom Size if needed
   */
  void Image(LI::Texture::Ref img, fvec2 size = 0.f, LI::Rect uv = fvec4(0));
  template <typename T>
  void DragData(const std::string& label, T* data, size_t num_elms = 1,
                T min = std::numeric_limits<T>::min(),
                T max = std::numeric_limits<T>::max(), T step = 1,
                int precision = 1) {
    u32 id = Strings::FastHash("drd" + label + std::to_string((uintptr_t)data));
    Container::Ref r = pLayout->FindObject(id);
    if (!r) {
      r = PD::New<UI7::DragData<T>>(label, data, num_elms, pIO, min, max, step,
                                    precision);
      r->SetID(id);
    }
    pLayout->AddObject(r);
  }
  void Sameline() { pLayout->SameLine(); }
  void Separator();
  void SeparatorText(const std::string& label);

  void HandleFocus();
  void HandleScrolling();
  void HandleTitlebarActions();
  void DrawBaseLayout();

  void Update();

  UI7MenuFlags Flags = 0;
  UI7::Layout::Ref pLayout;
  UI7::IO::Ref pIO;
  UI7::ID pID;
  bool* pIsShown = nullptr;
  bool pIsOpen = true;

  float TitleBarHeight = 0.f;
};
}  // namespace UI7
}  // namespace PD