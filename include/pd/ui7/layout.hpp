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

#include <pd/core/core.hpp>
#include <pd/ui7/container/container.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/input_api.hpp>
#include <pd/ui7/pd_p_api.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
class PD_UI7_API Layout {
 public:
  Layout(const ID& id, IO::Ref io) : ID(id) {
    this->IO = io;
    DrawList = Li::DrawList::New();
    DrawList->SetFont(IO->Font);
    DrawList->SetFontScale(io->FontScale);
    Scrolling[0] = false;
    Scrolling[1] = false;
    CursorInit();
    Pos = fvec2(io->CurrentViewPort.x, io->CurrentViewPort.y);
    Size = 0;
    WorkRect = fvec4(IO->MenuPadding, Size - (fvec2(2) * IO->MenuPadding));
  }
  ~Layout() = default;

  PD_SHARED(Layout);

  const std::string GetName() const { return ID.GetName(); }
  const UI7::ID& GetID() const { return this->ID; }

  const fvec2& GetPosition() const { return Pos; }
  void SetPosition(const fvec2& v) { Pos = v; }
  const fvec2& GetSize() const { return Size; }
  void SetSize(const fvec2& v) { Size = v; }

  Li::DrawList::Ref GetDrawList() { return DrawList; }

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
  void AddObjectEx(Container::Ref obj, u32 Flags);
  /**
   * Fast Function to Add Object in Layout SPace like
   * button Label images etc
   */
  void AddObject(Container::Ref obj);
  Container::Ref FindObject(u32 id);
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

  void Update();

  fvec2 DbgScrollOffset() { return ScrollOffset; }

 private:
  friend class Menu;
  friend class Context;
  friend class ReMenu;
  // Base Components
  UI7::ID ID;
  UI7::IO::Ref IO;
  Li::DrawList::Ref DrawList;

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
  std::list<Container::Ref> Objects;
  std::vector<Container::Ref> IDObjects;
};
}  // namespace UI7
}  // namespace PD