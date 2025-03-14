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
void Layout::CursorInit() { Cursor = WorkRect.xy(); }

void Layout::SameLine() {
  BackupCursor = LastObjSize;
  Cursor = SamelineCursor;
}

void Layout::CursorMove(const vec2& size) {
  LastObjSize = size;
  SamelineCursor = Cursor + vec2(size[0] + IO->ItemSpace[0], 0);
  if (BeforeSameLine[1]) {
    Cursor = vec2(IO->MenuPadding[0],
                  Cursor[1] + BeforeSameLine[1] + IO->ItemSpace[1]);
    BeforeSameLine = 0.f;
  } else {
    Cursor = vec2(IO->MenuPadding[0] + InitialCursorOffset[0],
                  Cursor[1] + size[1] + IO->ItemSpace[1]);
  }
  // Logical Issue here as x should use a max check
  MaxPosition = vec2(SamelineCursor[0], Cursor[1]);
}

bool Layout::ObjectWorkPos(vec2& movpos) {
  if (Scrolling[1]) {
    movpos[1] -= ScrollOffset[1];
    if (!IO->Ren->InBox(movpos, LastObjSize,
                        vec4(WorkRect.xy(), WorkRect.xy() + WorkRect.zw()))) {
      return true;
    }
  }
  return false;
}

void Layout::AddObject(Container::Ref obj) {
  obj->Init(IO, DrawList);
  obj->SetPos(AlignPosition(Cursor, obj->GetSize(), WorkRect, GetAlignment()));
  obj->Update();
  CursorMove(obj->GetSize());
  obj->HandleScrolling(ScrollOffset, WorkRect);
  Objects.push_back(obj);
}

Container::Ref Layout::FindObject(u32 id) {
  for (auto& it : IDObjects) {
    if (it->GetID() == id) {
      return it;
    }
  }
  return nullptr;
}

vec2 Layout::AlignPosition(vec2 pos, vec2 size, vec4 area, UI7Align alignment) {
  vec2 p = pos;
  if (alignment & UI7Align_Center) {
    p[0] = (area[0] + area[2]) * 0.5 - (pos[0] - area[0] + size[0] * 0.5);
  } else if (alignment & UI7Align_Right) {
  }
  if (alignment & UI7Align_Mid) {
    p[1] = (area[1] + area[3]) * 0.5 - (pos[1] - area[1] + size[1] * 0.5);
  } else if (alignment & UI7Align_Bottom) {
  }
  return p;
}

void Layout::Update() {
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
  Objects.clear();
  WorkRect = vec4(WorkRect.xy(), Size - IO->MenuPadding);
  CursorInit();
}
}  // namespace UI7
}  // namespace PD