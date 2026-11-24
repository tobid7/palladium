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

// Helpers to access either x or y
float& Axis(fvec2& v, int a) { return a ? v.y : v.x; }
float Axis(const fvec2& v, int a) { return a ? v.y : v.x; }

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
    Cursor = fvec2(IO.MenuPadding.x + InitialCursorOffset.x,
                   Cursor.y + BeforeSameLine.y + IO.ItemSpace.y);
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
  if (Flags & UI7LayoutFlags_UseClipRect) {
    obj->SetClipRect(pContentClip());
  }
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
    float max_x = MaxPosition.x;
    CursorMove(obj->GetSize());
    if (flags & UI7LytAdd_FillWidth) {
      MaxPosition.x = max_x;
    }
  }
  if (!(flags & UI7LytAdd_NoScrollHandle)) {
    obj->HandleScrolling(ScrollOffset, WorkRect);
  }
  if (Flags & UI7LayoutFlags_UseClipRect &&
      !(flags & UI7LytAdd_NoGlobalClipping)) {
    if (flags & UI7LytAdd_NoClipTitlebar) {
      obj->SetClipRect(fvec4(Pos.x, Pos.y, Size.x, Size.y));
    } else {
      obj->SetClipRect(pContentClip());
    }
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

/** calculate the content clip in a func
 * to not throw this thing into the funcsmultiple times
 */
PD_API fvec4 Layout::pContentClip() const {
  float sbs = IO.ScrollbarSize + 1.f;
  return fvec4(Pos.x, Pos.y + WorkRect.y,
               Size.x - (pScrollbarVisible[1] ? sbs : 0.f),
               Size.y - WorkRect.y - (pScrollbarVisible[0] ? sbs : 0.f));
}

PD_API bool Layout::IsScrollDragged() const {
  u32 d = IO.InputHandler.DraggedObject;
  return d != 0 && (d == (u32)UI7::ID("sbg" + ID.GetName()) ||
                    pScrollbarDragged[0] || pScrollbarDragged[1]);
}

PD_API void Layout::pHandleScrollbar(int axis, const fvec2& view_end) {
  const float sbs = IO.ScrollbarSize;
  const int other = axis ^ 1;
  float start = axis ? WorkRect.y - IO.MenuPadding.y : 1.f;
  float endpad = pScrollPadding;
  if (pScrollbarVisible[other]) endpad = std::max(endpad, sbs + 1.f);
  float end = Axis(Size, axis) - 1.f - endpad;
  float len = end - start;
  if (len <= 4.f) return;
  float cross = Axis(Size, other) - sbs - 1.f;

  // vlen -> View Len
  float vlen = axis ? view_end.y - WorkRect.y : view_end.x - WorkRect.x;
  float ms = Axis(pScrollMax, axis);
  float ratio = vlen / (vlen + ms);
  float glen = std::clamp(len * ratio,
                          std::min(Axis(IO.MinSliderDragSize, axis), len), len);
  float fleb = len - glen;
  float t =
      ms > 0.f ? std::clamp(Axis(ScrollOffset, axis) / ms, 0.f, 1.f) : 0.f;
  float gpos = start + fleb * t;

  // input handling
  fvec2 tpos, tsize;
  Axis(tpos, axis) = start;
  Axis(tpos, other) = cross;
  Axis(tsize, axis) = len;
  Axis(tsize, other) = sbs;
  tpos += Pos;

  UI7::ID sid("sb" + std::to_string(axis) + ID.GetName());
  IO.InputHandler.DragObject(sid, fvec4(tpos, tsize));
  bool owned = IO.InputHandler.DraggedObject == (u32)sid;
  float mouse = Axis(PD::Hid::MousePos(), axis) - Axis(Pos, axis);
  pScrollbarDragged[axis] = owned;
  if (owned && fleb > 0.f) {
    // we always drag the centet
    float nt = std::clamp((mouse - start - glen * 0.5f) / fleb, 0.f, 1.f);
    Axis(ScrollOffset, axis) = nt * ms;
    gpos = start + fleb * nt;
  }

  bool hovered = IO.InputHandler.CurrentMenu == IO.InputHandler.FocusedMenu &&
                 Li::Math::InBounds(PD::Hid::MousePos(), fvec4(tpos, tsize));
  UI7Color thumb_clr = owned     ? UI7Color_ButtonActive
                       : hovered ? UI7Color_ButtonHovered
                                 : UI7Color_Button;

  // Rendering
  fvec2 gtl, gbr;
  Axis(gtl, axis) = gpos;
  Axis(gtl, other) = cross;
  Axis(gbr, axis) = gpos + glen;
  Axis(gbr, other) = cross + sbs;
  float rad = std::min(IO.FrameRounding, sbs * 0.5f);
  float grad = std::min(IO.FrameRounding, sbs * 0.5f - 1.f);
  DrawList.SetLayer(21);
  DrawList.PathRect(tpos, tpos + tsize, rad);
  DrawList.PathFill(IO.Theme.Get(UI7Color_FrameBackground));
  DrawList.PathRect(Pos + gtl + fvec2(1.f), Pos + gbr - fvec2(1.f),
                    std::max(grad, 0.f));
  DrawList.PathFill(IO.Theme.Get(thumb_clr));
  DrawList.SetLayer(0);
}

PD_API void Layout::HandleScrolling() {
  if (pSkipScrolling) {
    return;
  }
  const bool enabled[2] = {(Flags & UI7LayoutFlags_HzScrolling) != 0,
                           (Flags & UI7LayoutFlags_VtScrolling) != 0};
  const bool bars = !(Flags & UI7LayoutFlags_NoScrollbar);
  const float sbs = bars ? IO.ScrollbarSize + 1.f : 0.f;

  // Vertical check is required twice due to the horizontal bar is
  // taking a little space
  fvec2 view_end = Size - IO.MenuPadding;
  bool need[2] = {false, false};
  need[1] = enabled[1] && MaxPosition.y > view_end.y;
  if (need[1]) view_end.x -= sbs;
  need[0] = enabled[0] && MaxPosition.x > view_end.x;
  if (need[0]) {
    view_end.y -= sbs;
    if (!need[1] && enabled[1] && MaxPosition.y > view_end.y) {
      need[1] = true;
      view_end.x -= sbs;
    }
  }
  WorkRect.z = view_end.x;
  WorkRect.w = view_end.y;
  pScrollMax = fvec2(std::max(0.f, MaxPosition.x - view_end.x),
                     std::max(0.f, MaxPosition.y - view_end.y));
  for (int a = 0; a < 2; a++) {
    pScrollbarVisible[a] = need[a] && bars;
    if (!pScrollbarVisible[a]) pScrollbarDragged[a] = false;
  }

  // Handle input of scrollbars first (io drag order und so)
  for (int a = 0; a < 2; a++) {
    if (pScrollbarVisible[a]) pHandleScrollbar(a, view_end);
  }

  // Drag the content (if no button or anything is there=)
  if (need[0] || need[1]) {
    if (PD::Hid::IsEvent(Hid::Event::Down, PD::Hid::Gamepad::Touch) ||
        PD::Hid::IsEvent(Hid::Event::Down, PD::Hid::Keyboard::MouseLeft)) {
      ScrollStart = ScrollOffset;
    }
    if (IO.InputHandler.DragObject(
            UI7::ID("sbg" + ID.GetName()),
            fvec4(Pos + fvec2(WorkRect.x, WorkRect.y),
                  fvec2(WorkRect.z - WorkRect.x, WorkRect.w - WorkRect.y)))) {
      if (!IO.InputHandler.DragReleasedAW) {
        for (int a = 0; a < 2; a++) {
          if (!need[a]) continue;
          Axis(ScrollOffset, a) = std::clamp(
              Axis(ScrollStart, a) + Axis(IO.InputHandler.DragSourcePos, a) -
                  Axis(IO.InputHandler.DragPosition, a),
              -20.f, Axis(pScrollMax, a) + 20.f);
        }
      }
    }
  }

  // delta based overscroll spring back
  float spring = IO.OverScrollMod * IO.Delta;
  for (int a = 0; a < 2; a++) {
    float& off = Axis(ScrollOffset, a);
    if (!need[a]) {
      off = 0.f;
      continue;
    }
    float ms = Axis(pScrollMax, a);
    if (off > ms) {
      off = std::max(off - spring, ms);
    } else if (off < 0.f) {
      off = std::min(off + spring, 0.f);
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
  if (!pSkipScrolling) {
    MaxPosition = 0.f;
  }
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