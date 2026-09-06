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
#include <pd/ui7/container/container.hpp>
#include <pd/ui7/container/dragdata.hpp>
#include <pd/ui7/container/slider.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/input_api.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
class Context;
class PD_API Layout {
 public:
  Layout(const ID& id, IO& io) : ID(id), IO(io) {
    DrawList.SetFont(IO.Font);
    DrawList.SetFontscale(io.FontScale);
    Scrolling[0] = false;
    Scrolling[1] = false;
    CursorInit();
    Pos = fvec2(io.CurrentViewPort.pSize.x, io.CurrentViewPort.pSize.y);
    Size = 0;
    WorkRect = fvec4(IO.MenuPadding, Size - (fvec2(2) * IO.MenuPadding));
  }
  ~Layout();

  /** SECTION CONTAINERS */
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
  void Image(Li::Texture img, fvec2 size = 0.f, Li::Rect uv = fvec4(0));
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
    Container* r = FindObject(id);
    if (!r) {
      r = new UI7::DragData<T>(label, data, num_elms, this->IO, min, max, step,
                               precision);
      r->SetID(id);
    }
    AddObject(r);
  }
  template <typename T>
  void Slider(const std::string& label, T* data,
              T min = std::numeric_limits<T>::min(),
              T max = std::numeric_limits<T>::max(), int precision = 1) {
    u32 id = Strings::FastHash("drd" + label + std::to_string((uintptr_t)data));
    Container* r = FindObject(id);
    if (!r) {
      r = new UI7::Slider<T>(label, data, this->IO, min, max, precision);
      r->SetID(id);
    }
    AddObject(r);
  }

  /** SECTION OTHERSTUFF */

  const std::string GetName() const { return ID.GetName(); }
  const UI7::ID& GetID() const { return this->ID; }

  const fvec2& GetPosition() const { return Pos; }
  void SetPosition(const fvec2& v) { Pos = v; }
  const fvec2& GetSize() const { return Size; }
  void SetSize(const fvec2& v) { Size = v; }

  Li::Drawlist& GetDrawList() { return DrawList; }

  void CursorInit();
  void SameLine();
  void CursorMove(const fvec2& size);

  bool ObjectWorkPos(fvec2& movpos);

  /**
   * Extended Object Add Func to Add Object in Front or disable
   * Position by cursor as well as cursor update...
   * Should only be used in special cases as the
   * AddObject function is faster
   * Using Flags for its features cause dont want to have too much args
   */
  void AddObjectEx(Container* obj, u32 Flags);
  /**
   * Fast Function to Add Object in Layout SPace like
   * button Label images etc
   */
  void AddObject(Container* obj);
  Container* FindObject(u32 id);
  void ClearIDObjects() { IDObjects.clear(); }

  fvec2 AlignPosition(fvec2 pos, fvec2 size, fvec4 area, UI7Align alignment);

  /** Get the Alignment for Current State */
  UI7Align GetAlignment() {
    /// if temp alignment is used then return it and
    /// reset tmpalign
    if (TempAlign) {
      auto t = TempAlign;
      TempAlign = 0;
      return t;
    }
    return Alignment;
  }

  void SetAlign(UI7Align a) { Alignment = a; }
  void NextAlign(UI7Align a) { TempAlign = a; }

  void HandleScrolling();

  void Update();

  fvec2 DbgScrollOffset() { return ScrollOffset; }

 private:
  friend class Menu;
  friend class PD::UI7::Context;
  friend class ReMenu;
  // Base Components
  UI7::ID ID;
  UI7::IO& IO;
  Li::Drawlist DrawList;
  UI7LayoutFlags Flags;

  // Positioning
  fvec2 Pos;
  fvec2 Size;
  UI7Align Alignment = UI7Align_Default;
  UI7Align TempAlign;

  // Cursor
  fvec2 Cursor;
  fvec2 InitialCursorOffset;
  fvec2 BackupCursor;
  fvec2 SamelineCursor;
  fvec2 BeforeSameLine;
  fvec2 LastObjSize;
  fvec2 MaxPosition;
  fvec4 WorkRect;

  // Scrolling (Only theoretical)
  // Rendering must be done by the Objective that uses the Lyt
  fvec2 ScrollOffset;
  fvec2 ScrollStart;
  bool Scrolling[2];

  // Objects
  std::list<Container*> Objects;
  std::vector<Container*> IDObjects;
};
}  // namespace UI7
}  // namespace PD