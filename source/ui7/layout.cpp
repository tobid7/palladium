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

#include <pd/ui7/layout.hpp>

namespace PD {
namespace UI7 {
PD_UI7_API void Layout::CursorInit() { Cursor = fvec2(WorkRect.x, WorkRect.y); }

PD_UI7_API void Layout::SameLine() {
  BackupCursor = LastObjSize;
  Cursor = SamelineCursor;
}

PD_UI7_API void Layout::CursorMove(const fvec2& size) {
  LastObjSize = size;
  SamelineCursor = Cursor + fvec2(size.x + IO->ItemSpace.x, 0);
  if (BeforeSameLine.y) {
    Cursor =
        fvec2(IO->MenuPadding.x, Cursor.y + BeforeSameLine.y + IO->ItemSpace.y);
    BeforeSameLine = 0.f;
  } else {
    Cursor = fvec2(IO->MenuPadding.x + InitialCursorOffset.x,
                   Cursor.y + size.y + IO->ItemSpace.y);
  }
  // Logical Issue here as x should use a max check
  MaxPosition = fvec2(std::max(MaxPosition.x, SamelineCursor.x), Cursor.y);
}

PD_UI7_API bool Layout::ObjectWorkPos(fvec2& movpos) {
  if (Scrolling[1]) {
    movpos.y -= ScrollOffset.y;
    if (!IO->Ren->InBox(movpos, LastObjSize,
                        fvec4(WorkRect.x, WorkRect.y, WorkRect.x + WorkRect.z,
                              WorkRect.y + WorkRect.w))) {
      return true;
    }
  }
  return false;
}

PD_UI7_API void Layout::AddObject(Container::Ref obj) {
  obj->Init(IO, DrawList);
  obj->SetPos(AlignPosition(Cursor, obj->GetSize(), WorkRect, GetAlignment()));
  obj->Update();
  CursorMove(obj->GetSize());
  obj->HandleScrolling(ScrollOffset, WorkRect);
  Objects.PushBack(obj);
}

PD_UI7_API void Layout::AddObjectEx(Container::Ref obj, u32 flags) {
  obj->Init(IO, DrawList);
  if (!(flags & 1)) {
    obj->SetPos(
        AlignPosition(Cursor, obj->GetSize(), WorkRect, GetAlignment()));
  }
  obj->Update();
  if (!(flags & 1)) {
    CursorMove(obj->GetSize());
  }
  if (!(flags & 2)) {
    obj->HandleScrolling(ScrollOffset, WorkRect);
  }
  if (!(flags & 4)) {
    Objects.PushFront(obj);
  } else {
    Objects.PushBack(obj);
  }
}

PD_UI7_API Container::Ref Layout::FindObject(u32 id) {
  for (auto& it : IDObjects) {
    if (it->GetID() == id) {
      return it;
    }
  }
  return nullptr;
}

PD_UI7_API fvec2 Layout::AlignPosition(fvec2 pos, fvec2 size, fvec4 area,
                                       UI7Align alignment) {
  vec2 p = pos;
  if (alignment & UI7Align_Center) {
    p.x = (area.x + area.z) * 0.5 - (pos.x - area.x + size.x * 0.5);
  } else if (alignment & UI7Align_Right) {
  }
  if (alignment & UI7Align_Mid) {
    p.y = (area.y + area.w) * 0.5 - (pos.y - area.y + size.y * 0.5);
  } else if (alignment & UI7Align_Bottom) {
  }
  return p;
}

PD_UI7_API void Layout::Update() {
  for (auto& it : Objects) {
    if (it->GetID() != 0 && !FindObject(it->GetID())) {
      IDObjects.push_back(it);
    }
    if (!it->Skippable()) {
      it->SetPos(it->GetPos() + Pos);
      it->HandleInput();
      it->UnlockInput();
      it->Draw();
    }
  }
  std::vector<size_t> tbr;
  for (size_t i = 0; i < IDObjects.size(); i++) {
    if (IDObjects[i]->Removable()) {
      tbr.push_back(i);
    }
  }
  for (auto& it : tbr) {
    IDObjects.erase(IDObjects.begin() + it);
  }
  Objects.Clear();
  WorkRect = fvec4(fvec2(WorkRect.x, WorkRect.y), Size - IO->MenuPadding);
  CursorInit();
}
}  // namespace UI7
}  // namespace PD