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

#include <algorithm>
#include <pd/ui7/containers.hpp>
#include <pd/ui7/layout.hpp>

namespace PD {
namespace UI7 {

PD_API Layout::~Layout() {
  // We all love managing memory i guess
  for (Container* obj : IDObjects) {
    delete obj;
  }
  IDObjects.clear();
}

PD_API void Layout::CursorInit() { Cursor = fvec2(WorkRect.x, WorkRect.y); }

PD_API void Layout::SameLine() {
  BackupCursor = LastObjSize;
  Cursor = SamelineCursor;
}

PD_API void Layout::CursorMove(const fvec2& size) {
  LastObjSize = size;
  SamelineCursor = Cursor + fvec2(size.x + IO.ItemSpace.x, 0);
  if (BeforeSameLine.y) {
    Cursor =
        fvec2(IO.MenuPadding.x, Cursor.y + BeforeSameLine.y + IO.ItemSpace.y);
    BeforeSameLine = 0.f;
  } else {
    Cursor = fvec2(IO.MenuPadding.x + InitialCursorOffset.x,
                   Cursor.y + size.y + IO.ItemSpace.y);
  }
  // Logical Issue here as x should use a max check
  MaxPosition = fvec2(std::max(MaxPosition.x, SamelineCursor.x), Cursor.y);
}

PD_API bool Layout::ObjectWorkPos(fvec2& movpos) {
  if (Scrolling[1]) {
    movpos.y -= ScrollOffset.y;
    if (!Li::Math::InBounds(
            movpos, LastObjSize,
            fvec4(WorkRect.x, WorkRect.y, WorkRect.x + WorkRect.z,
                  WorkRect.y + WorkRect.w))) {
      return true;
    }
  }
  return false;
}

PD_API void Layout::AddObject(Container* obj) {
  obj->Init(&IO, &DrawList);
  obj->SetPos(AlignPosition(Cursor, obj->GetSize(), WorkRect, GetAlignment()));
  obj->Update();
  CursorMove(obj->GetSize());
  obj->HandleScrolling(ScrollOffset, WorkRect);
  Objects.push_back(obj);
}

PD_API void Layout::AddObjectEx(Container* obj, u32 flags) {
  obj->Init(&IO, &DrawList);
  if (!(flags & UI7LytAdd_NoCursorUpdate)) {
    obj->SetPos(
        AlignPosition(Cursor, obj->GetSize(), WorkRect, GetAlignment()));
  }
  obj->Update();
  if (!(flags & UI7LytAdd_NoCursorUpdate)) {
    CursorMove(obj->GetSize());
  }
  if (!(flags & UI7LytAdd_NoScrollHandle)) {
    obj->HandleScrolling(ScrollOffset, WorkRect);
  }
  if (flags & UI7LytAdd_Front) {
    Objects.push_front(obj);
  } else {
    Objects.push_back(obj);
  }
}

PD_API Container* Layout::FindObject(u32 id) {
  for (auto& it : IDObjects) {
    if (it->GetID() == id) {
      return it;
    }
  }
  return nullptr;
}

PD_API fvec2 Layout::AlignPosition(fvec2 pos, fvec2 size, fvec4 area,
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

PD_API void Layout::HandleScrolling() {
  if (Flags & UI7LayoutFlags_VtScrolling) {
    bool allowed = MaxPosition.y > WorkRect.w;
    if (allowed) {
      if (PD::Hid::IsEvent(Hid::Event::Down, PD::Hid::Gamepad::Touch) ||
          PD::Hid::IsEvent(Hid::Event::Down, PD::Hid::Keyboard::MouseLeft)) {
        ScrollStart = ScrollOffset;
      }
      if (IO.InputHandler.DragObject(UI7::ID("sbg" + ID.GetName()),
                                     fvec4(Pos, fvec2(0.f)) + WorkRect)) {
        if (!IO.InputHandler.DragReleasedAW) {
          ScrollOffset.y =
              std::clamp(ScrollStart.y + IO.InputHandler.DragSourcePos.y -
                             IO.InputHandler.DragPosition.y,
                         -20.f, MaxPosition.y - WorkRect.w + 20.f);
        }
      }
    } else {
      ScrollOffset.y = 0.f;
    }

    if (ScrollOffset.y > MaxPosition.y - WorkRect.w) {
      ScrollOffset.y -= 1.5f;
      if (ScrollOffset.y < MaxPosition.y - WorkRect.w) {
        ScrollOffset.y = MaxPosition.y - WorkRect.w;
      }
    }
    if (ScrollOffset.y < 0) {
      ScrollOffset.y += 1.5f;
      if (ScrollOffset.y > 0) {
        ScrollOffset.y = 0;
      }
    }
  }
}

PD_API void Layout::Update() {
  if (Size == fvec2(0.f)) {
    Size = fvec2(MaxPosition) + IO.MenuPadding * 2;
  }
  for (auto& it : Objects) {
    if (it->GetID() != 0 && !FindObject(it->GetID())) {
      IDObjects.push_back(it);
    }
    if (!it->Skippable()) {
      it->SetPos(it->GetPos() + Pos);
      it->HandleInput();
      it->UnlockInput();
      if (Flags & UI7LayoutFlags_UseClipRect) {
        it->SetClipRect(fvec4(Pos.x, Pos.y, Size.x, Size.y));
      }
      it->PreDraw();
      it->Draw();
      it->PostDraw();
    }
  }

  for (auto it = IDObjects.begin(); it != IDObjects.end();) {
    if ((*it)->Removable()) {
      delete *it;
      it = IDObjects.erase(it);
    } else {
      it++;
    }
  }

  Objects.clear();
  WorkRect = fvec4(fvec2(WorkRect.x, WorkRect.y), Size - IO.MenuPadding);
  CursorInit();
  HandleScrolling();
}

/** SECTION CONTAINERS (STOLEN FROM FORMER MENU) */

PD_API void Layout::Label(const std::string& label) {
  // Layout API
  auto r = IO.LabelPool.Allocate();
  *r = UI7::Label(label, IO);
  // r->SetClipRect(fvec4(GetPosition(), GetSize()));
  AddObject(r);
}

PD_API bool Layout::Button(const std::string& label) {
  bool ret = false;
  u32 id = Strings::FastHash("btn" + label + std::to_string(Objects.size()));
  Container* r = FindObject(id);
  if (!r) {
    r = new UI7::Button(label, IO);
    r->SetID(id);
  }
  AddObject(r);
  if (!r->Skippable()) {
    ret = reinterpret_cast<UI7::Button*>(r)->IsPressed();
  }
  return ret;
}

PD_API void Layout::Checkbox(const std::string& label, bool& v) {
  u32 id = Strings::FastHash("cbx" + label + std::to_string(Objects.size()));
  Container* r = FindObject(id);
  if (!r) {
    r = new UI7::Checkbox(label, v, IO);
    r->SetID(id);
  }
  AddObject(r);
}

PD_API void Layout::Image(Li::Texture img, fvec2 size, Li::Rect uv) {
  auto r = IO.ImagePool.Allocate();
  *r = UI7::Image(img, size, uv);
  AddObject(r);
}

}  // namespace UI7
}  // namespace PD