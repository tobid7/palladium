#pragma once

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

#include <pd/core/common.hpp>
#include <pd/ui7/container/container.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/io.hpp>

namespace PD {
namespace UI7 {
class Layout : public PD::SmartCtor<Layout> {
 public:
  Layout(const ID& id, IO::Ref io) : ID(id) {
    this->IO = io;
    DrawList = UI7::DrawList::New(io->Ren);
    Scrolling[0] = false;
    Scrolling[1] = false;
    CursorInit();
    Pos = vec2(0, 0);
    Size = vec2(320, 240);
    WorkRect = vec4(IO->MenuPadding, Size - (vec2(2) * IO->MenuPadding));
  }
  ~Layout() = default;

  const std::string& GetName() const { return ID.GetName(); }
  const UI7::ID& GetID() const { return this->ID; }

  const vec2& GetPosition() const { return Pos; }
  void SetPosition(const vec2& v) { Pos = v; }
  const vec2& GetSize() const { return Size; }
  void SetSize(const vec2& v) { Size = v; }

  UI7::DrawList::Ref GetDrawList() { return DrawList; }

  void CursorInit();
  void SameLine();
  void CursorMove(const vec2& size);

  bool ObjectWorkPos(vec2& movpos);

  void AddObject(Container::Ref obj);
  Container::Ref FindObject(u32 id);
  void ClearIDObjects() { IDObjects.clear(); }

  vec2 AlignPosition(vec2 pos, vec2 size, vec4 area, UI7Align alignment);

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

 private:
  friend class Menu;
  friend class Context;
  // Base Components
  UI7::ID ID;
  UI7::IO::Ref IO;
  UI7::DrawList::Ref DrawList;

  // Positioning
  vec2 Pos;
  vec2 Size;
  UI7Align Alignment = UI7Align_Default;
  UI7Align TempAlign;

  // Cursor
  vec2 Cursor;
  vec2 InitialCursorOffset;
  vec2 BackupCursor;
  vec2 SamelineCursor;
  vec2 BeforeSameLine;
  vec2 LastObjSize;
  vec2 MaxPosition;
  vec4 WorkRect;

  // Scrolling
  vec2 ScrollOffset;
  bool Scrolling[2];

  // Objects
  std::vector<Container::Ref> Objects;
  std::vector<Container::Ref> IDObjects;
};
}  // namespace UI7
}  // namespace PD