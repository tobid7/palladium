#pragma once

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

#include <pd/core/core.hpp>
#include <pd/pd_p_api.hpp>
#include <pd/ui7/containers.hpp>
#include <pd/ui7/io.hpp>
#include <pd/ui7/layout.hpp>

#include "pd/ui7/container/dragdata.hpp"

namespace PD {
namespace UI7 {
class PD_API Menu {
 public:
  Menu(const UI7::ID& id, UI7::IO::Ref pIO);
  ~Menu() {}

  PD_SHARED(Menu);

  /**
   * Render a Simple Label
   * @param label The text to draw
   */
  void Label(const std::string& label);
  template <typename... Args>
  void Label(std::format_string<Args...> s, Args&&... args) {
    Label(std::format(s, std::forward<Args>(args)...));
  }
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
  void Image(Li::Texture::Ref img, fvec2 size = 0.f, Li::Rect uv = fvec4(0));
  /**
   * Render a Drag Object witth any supported type:
   * [`int`, `float`, `double`, `u8`, `u16`, `u32`]
   * @param label Name of the Drag
   * @param data Reference to Data Object (can be multiple as well)
   * @param num_elements Defien the number of Elements in the Data addr
   * @param precission Difine the Format string len for float/double
   */
  template <typename T>
  void DragData(const std::string& label, T* data, size_t num_elms = 1,
                T min = std::numeric_limits<T>::min(),
                T max = std::numeric_limits<T>::max(), T step = 1,
                int precision = 1) {
    u32 id = Strings::FastHash("drd" + label + std::to_string((uintptr_t)data));
    Container::Ref r = pLayout->FindObject(id);
    if (!r) {
      r = UI7::DragData<T>::New(label, data, num_elms, pIO, min, max, step,
                                precision);
      // Isnt This exactly the same line???
      // r = UI7::DragData<T>::New(label, data, num_elms, pIO, min, max, step,
      //                          precision);
      r->SetID(id);
    }
    pLayout->AddObject(r);
  }
  template <typename T>
  void Slider(const std::string& label, T* data,
              T min = std::numeric_limits<T>::min(),
              T max = std::numeric_limits<T>::max(), int precision = 1) {
    u32 id = Strings::FastHash("drd" + label + std::to_string((uintptr_t)data));
    Container::Ref r = pLayout->FindObject(id);
    if (!r) {
      r = UI7::Slider<T>::New(label, data, pIO, min, max, precision);
      r->SetID(id);
    }
    pLayout->AddObject(r);
  }
  void ColorEdit(const std::string& label, u32& clr);
  void SameLine() { pLayout->SameLine(); }
  void Separator();
  void SeparatorText(const std::string& label);
  bool BeginTreeNode(const ID& id);
  void EndTreeNode();

  void HandleFocus();
  void HandleScrolling();
  void HandleTitlebarActions();
  void DrawBaseLayout();

  void AddObject(PD::UI7::Container::Ref obj) { pLayout->AddObject(obj); }
  void AddObjectEx(PD::UI7::Container::Ref obj, PD::u32 flags) {
    pLayout->AddObjectEx(obj, flags);
  }
  Container::Ref FindObject(u32 id) { return pLayout->FindObject(id); }

  void Update();

  void SetSize(PD::fvec2 size) { pLayout->SetSize(size); }
  void SetPosition(PD::fvec2 pos) { pLayout->SetPosition(pos); }

  /** Data Section */

  UI7MenuFlags Flags = 0;
  Layout::Ref pLayout;
  IO::Ref pIO;
  ID pID;
  bool* pIsShown = nullptr;
  bool pIsOpen = true;
  std::unordered_map<u32, bool> pTreeNodes;
  fvec2 TempScrollXY;

  float TitleBarHeight = 0.f;
};
}  // namespace UI7
}  // namespace PD
