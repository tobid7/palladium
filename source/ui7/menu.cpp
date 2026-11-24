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
#include <pd/ui7/menu.hpp>

namespace PD {
namespace UI7 {
Menu::Menu(const ID& id, IO& io) : pLayout(id, io), pIO(io), pID(id) {
  TitleBarHeight = pIO.FontScale * pIO.Font->PixelHeight + pIO.MenuPadding.y;
  pLayout.WorkRect.y += TitleBarHeight;
  pLayout.Flags |= UI7LayoutFlags_UseClipRect;
  pLayout.CursorInit();
}

PD_API void Menu::Label(const std::string& label) {
  // Layout API
  auto r = pIO.LabelPool.Allocate();
  *r = UI7::Label(label, pIO);
  pLayout.AddObject(r);
}

PD_API bool Menu::Button(const std::string& label) {
  bool ret = false;
  u32 id =
      Strings::FastHash("btn" + label + std::to_string(pLayout.Objects.size()));
  Container* r = pLayout.FindObject(id);
  if (!r) {
    r = new UI7::Button(label, pIO);
    r->SetID(id);
  }
  pLayout.AddObject(r);
  if (!r->Skippable()) {
    ret = reinterpret_cast<UI7::Button*>(r)->IsPressed();
  }
  return ret;
}

PD_API void Menu::Checkbox(const std::string& label, bool& v) {
  u32 id =
      Strings::FastHash("cbx" + label + std::to_string(pLayout.Objects.size()));
  Container* r = pLayout.FindObject(id);
  if (!r) {
    r = new UI7::Checkbox(label, v, pIO);
    r->SetID(id);
  }
  pLayout.AddObject(r);
}

PD_API void Menu::Image(Li::Texture img, fvec2 size, Li::Rect uv) {
  auto r = pIO.ImagePool.Allocate();
  *r = UI7::Image(img, size, uv);
  pLayout.AddObject(r);
}

PD_API void Menu::ColorEdit(const std::string& label, u32& clr) {
  u32 id = Strings::FastHash("drd" + label);
  Container* r = pLayout.FindObject(id);
  if (!r) {
    r = new UI7::ColorEdit(label, &clr, pIO);
    r->SetID(id);
  }
  pLayout.AddObject(r);
}

PD_API void Menu::Separator() {
  // Dynamic Objects are very simple...
  DynObj* r = pIO.DynObjPool.Allocate();
  *r = UI7::DynObj(
      [=, this](UI7::IO* io, Li::Drawlist* l, UI7::Container* self) {
        l->DrawRectFilled(self->FinalPos(), self->GetSize(),
                          pIO.Theme.Get(UI7Color_TextDead));
      });
  // Set size before pushing (cause Cursor Move will require it)
  // WorkRect.z is already excluding the scrollbar
  r->SetSize(fvec2(std::max(0.f, pLayout.WorkRect.z - pIO.MenuPadding.x -
                                     pLayout.InitialCursorOffset.x),
                   1));
  pLayout.AddObjectEx(r, UI7LytAdd_FillWidth);
}

PD_API void Menu::SeparatorText(const std::string& label) {
  // Also note to use [=] instead of [&] to not undefined access label
  DynObj* r = pIO.DynObjPool.Allocate();
  *r = UI7::DynObj(
      [=, this](UI7::IO* io, Li::Drawlist* l, UI7::Container* self) {
        l->SetFont(self->GetFont());
        fvec2 size = self->GetSize();
        fvec2 tdim = io->Font->GetTextBounds(label.c_str(), io->FontScale);
        fvec2 pos = self->FinalPos();
        auto align = pLayout.GetAlignment();
        fvec2 rpos = pos;
        if (align & UI7Align_Center) {
          rpos.x = pos.x + (size.x - tdim.x) * 0.5f;
        } else if (align & UI7Align_Right) {
          rpos.x = pos.x + size.x - tdim.x;
        }
        float ly = rpos.y + tdim.y * 0.5f;
        if (!(align & UI7Align_Left)) {
          float w = rpos.x - io->FramePadding.x - pos.x;
          if (w > 0.f) {
            l->DrawRectFilled(fvec2(pos.x, ly), fvec2(w, 1),
                              io->Theme.Get(UI7Color_TextDead));
          }
        }
        if (!(align & UI7Align_Right)) {
          float sx = rpos.x + tdim.x + io->FramePadding.x;
          float w = pos.x + size.x - sx;
          if (w > 0.f) {
            l->DrawRectFilled(fvec2(sx, ly), fvec2(w, 1),
                              io->Theme.Get(UI7Color_TextDead));
          }
        }
        l->DrawText(rpos, label.c_str(), io->Theme.Get(UI7Color_Text));
      });
  // Set size before pushing (cause Cursor Move will require it)
  // WorkRect.z is already excluding the scrollbar
  r->SetSize(fvec2(std::max(0.f, pLayout.WorkRect.z - pIO.MenuPadding.x -
                                     pLayout.InitialCursorOffset.x),
                   pIO.Font->PixelHeight * pIO.FontScale));
  pLayout.AddObjectEx(r, UI7LytAdd_FillWidth);
  // label does count btw
  float tw = pIO.Font->GetTextBounds(label.c_str(), pIO.FontScale).x;
  pLayout.MaxPosition.x =
      std::max(pLayout.MaxPosition.x, r->GetPos().x + tw + pIO.ItemSpace.x);
}
PD_API void Menu::HandleFocus() {
  // Check if menu can be focused for Selective Menu Input API
  vec4 newarea = fvec4(pLayout.Pos, pLayout.Size);
  if (!pIsOpen) {
    newarea = fvec4(pLayout.Pos, fvec2(pLayout.Size.x, TitleBarHeight));
  }
  if ((PD::Hid::IsEvent(Hid::Event::Down, Hid::Gamepad::Touch) ||
       PD::Hid::IsEvent(Hid::Event::Down, Hid::Keyboard::MouseLeft)) &&
      Li::Math::InBounds(PD::Hid::MousePos(), newarea) &&
      !Li::Math::InBounds(PD::Hid::MousePos(),
                          pIO.InputHandler.FocusedMenuRect)) {
    pIO.InputHandler.FocusedMenu = pID;
  }
  if (pIO.InputHandler.FocusedMenu == pID) {
    pIO.InputHandler.FocusedMenuRect = newarea;
  }
}

PD_API fvec2 Menu::pTitleButtonSize() const {
  float s = std::max(TitleBarHeight - pIO.FramePadding.y * 2, 5.f);
  return fvec2(s, s);
}

PD_API int Menu::pNumTitleButtons() const {
  int n = 0;
  if (!(Flags & UI7MenuFlags_NoClose) && pIsShown) n++;
  if (Flags & UI7MenuFlags_Maximize) n++;
  if (Flags & UI7MenuFlags_Minimize) n++;
  return n;
}

PD_API bool Menu::pTitleButtonPos(TitleButton b, fvec2& pos) const {
  const bool shown[3] = {!(Flags & UI7MenuFlags_NoClose) && pIsShown != nullptr,
                         (Flags & UI7MenuFlags_Maximize) != 0,
                         (Flags & UI7MenuFlags_Minimize) != 0};
  if (!shown[b]) return false;
  int slot = 0;
  for (int i = 0; i < b; i++) {
    if (shown[i]) slot++;
  }
  fvec2 size = pTitleButtonSize();
  pos = fvec2(pLayout.Size.x - (slot + 1) * (size.x + pIO.FramePadding.x),
              pIO.FramePadding.y);
  return true;
}

PD_API void Menu::SetMaximized(bool v) {
  if (v == pIsMaximized) return;
  if (v) {
    pRestorePos = pLayout.Pos;
    pRestoreSize = pLayout.Size;
    pIsOpen = true;
  } else {
    pLayout.Pos = pRestorePos;
    pLayout.Size = pRestoreSize;
  }
  pIsMaximized = v;
}

PD_API void Menu::ScrollTo(const fvec2& pos, float seconds) {
  if (seconds <= 0.f) {
    pScrollAnimActive = false;
    pLayout.ScrollOffset = pos;
    return;
  }
  pScrollFrom = pLayout.ScrollOffset;
  pScrollTarget = pos;
  pScrollAnim.From(0.f).To(1.f).In(seconds).As(pScrollAnim.EaseOutQuad);
  pScrollAnimActive = true;
}

PD_API void Menu::pUpdateScrollAnim() {
  if (!pScrollAnimActive) return;
  // user can stop the animation of course
  if (pLayout.IsScrollDragged()) {
    pScrollAnimActive = false;
    return;
  }
  const fvec2& max = pLayout.GetScrollMax();
  fvec2 target(std::clamp(pScrollTarget.x, 0.f, max.x),
               std::clamp(pScrollTarget.y, 0.f, max.y));
  pScrollAnim.Update(pIO.Delta);
  float t = pScrollAnim.Get();
  pLayout.ScrollOffset = pScrollFrom + (target - pScrollFrom) * t;
  if (pScrollAnim.IsFinished()) {
    pLayout.ScrollOffset = target;
    pScrollAnimActive = false;
  }
}

PD_API void Menu::HandleTitlebarActions() {
  // Collapse
  if (!(Flags & UI7MenuFlags_NoCollapse)) {
    vec2 cpos = pLayout.Pos + pIO.FramePadding;
    if (pIO.InputHandler.DragObject(UI7::ID(pID.GetName() + "clbse"),
                                    fvec4(cpos, pTitleButtonSize()))) {
      if (pIO.InputHandler.DragReleased) {
        pIsOpen = !pIsOpen;
      }
    }
  }
  fvec2 bsize = pTitleButtonSize();
  fvec2 bpos;
  // Close Logic
  if (pTitleButtonPos(TitleButton_Close, bpos)) {
    if (pIO.InputHandler.DragObject(UI7::ID(pID.GetName() + "clse"),
                                    fvec4(pLayout.Pos + bpos, bsize))) {
      if (pIO.InputHandler.DragReleased) {
        *pIsShown = !(*pIsShown);
      }
    }
  }
  // Maximize Logic
  if (pTitleButtonPos(TitleButton_Maximize, bpos)) {
    if (pIO.InputHandler.DragObject(UI7::ID(pID.GetName() + "mxbt"),
                                    fvec4(pLayout.Pos + bpos, bsize))) {
      if (pIO.InputHandler.DragReleased) {
        SetMaximized(!pIsMaximized);
      }
    }
  }
  // Minimize Logic
  if (pTitleButtonPos(TitleButton_Minimize, bpos)) {
    if (pIO.InputHandler.DragObject(UI7::ID(pID.GetName() + "mnbt"),
                                    fvec4(pLayout.Pos + bpos, bsize))) {
      if (pIO.InputHandler.DragReleased) {
        pIsMinimized = true;
      }
    }
  }
  // Resize logic
  if (pCanResize()) {
    fvec2 grip(pIO.ResizeGripSize);
    vec2 cpos = pLayout.Pos + pLayout.Size - grip;
    if (pIO.InputHandler.DragObject(UI7::ID(pID.GetName() + "rszs"),
                                    fvec4(cpos, grip))) {
      fvec2 szs = pIO.InputHandler.DragMovedPos() + grip - pLayout.Pos;
      if (szs.x < 30) szs.x = 30;
      if (szs.y < 30) szs.y = 30;
      pLayout.Size = szs;
    }
  }
  // Menu Movement
  if (!(Flags & UI7MenuFlags_NoMove) || (Flags & UI7MenuFlags_Maximize)) {
    if (pIO.InputHandler.DragObject(
            pID.GetName() + "tmv",
            fvec4(pLayout.Pos, fvec2(pLayout.Size.x, TitleBarHeight)))) {
      if (pIO.InputHandler.DragDoubleRelease) {
        // maximize on double click (like on windows) if enabled
        if (Flags & UI7MenuFlags_Maximize) {
          SetMaximized(!pIsMaximized);
        } else {
          pIsOpen = !pIsOpen;
        }
      }
      if (!(Flags & UI7MenuFlags_NoMove) && !pIsMaximized) {
        pLayout.Pos = pIO.InputHandler.DragMovedPos();
        // Keep Window In Viewport
        // Maybe i need to add some operators to vec
        pLayout.Pos.x = std::clamp<float>(pLayout.Pos.x, -pLayout.Size.x + 10,
                                          pIO.CurrentViewPort.pSize.z - 10);
        pLayout.Pos.y =
            std::clamp<float>(pLayout.Pos.y, pIO.CurrentViewPort.pSize.y,
                              pIO.CurrentViewPort.pSize.w - 10);
      }
    }
  }
}

PD_API void Menu::DrawBaseLayout() {
  if (pIsOpen) {
    /** Resize Sym (Render on Top of Everything) */
    if (pCanResize()) {
      DynObj* r = pIO.DynObjPool.Allocate();
      *r = UI7::DynObj([](IO* io, Li::Drawlist* l, UI7::Container* self) {
        l->SetLayer(21);
        float g = io->ResizeGripSize;
        l->PathAdd(self->FinalPos() + self->GetSize() - fvec2(0, g));
        l->PathAdd(self->FinalPos() + self->GetSize());
        l->PathAdd(self->FinalPos() + self->GetSize() - fvec2(g, 0));
        l->PathFill(io->Theme.Get(UI7Color_Button));
        l->SetLayer(0);
      });
      r->SetSize(
          fvec2(pLayout.GetSize().x, pLayout.GetSize().y - TitleBarHeight));
      r->SetPos(fvec2(0, TitleBarHeight));
      pLayout.AddObjectEx(r, UI7LytAdd_NoCursorUpdate |
                                 UI7LytAdd_NoScrollHandle |
                                 UI7LytAdd_NoClipTitlebar);
    }

    /** Background */
    if (!(Flags & UI7MenuFlags_NoBackground)) {
      DynObj* r = pIO.DynObjPool.Allocate();
      *r = UI7::DynObj([](IO* io, Li::Drawlist* l, UI7::Container* self) {
        l->SetLayer(0);
        l->PathRectEx(self->FinalPos(), self->FinalPos() + self->GetSize(),
                      10.f, LiPathRectFlags_KeepTop | LiPathRectFlags_KeepBot);
        l->PathFill(io->Theme.Get(UI7Color_Background));
      });
      // Set size before pushing (cause Cursor Move will require it)
      r->SetSize(
          fvec2(pLayout.GetSize().x, pLayout.GetSize().y - TitleBarHeight));
      r->SetPos(fvec2(0, TitleBarHeight));
      pLayout.AddObjectEx(r, UI7LytAdd_NoCursorUpdate |
                                 UI7LytAdd_NoScrollHandle | UI7LytAdd_Front |
                                 UI7LytAdd_NoClipTitlebar);
    }
  }
  if (!(Flags & UI7MenuFlags_NoTitlebar)) {
    /** Bar/Title */
    pTitlebar();
    /** Collapse Sym */
    pCollapseSym();
    /** Titlebar buttons */
    fvec2 size = pTitleButtonSize();
    fvec2 bpos;
    u32 bclr = pIO.Theme.Get(UI7Color_FrameBackground);
    pLayout.DrawList.SetLayer(21);
    if (pTitleButtonPos(TitleButton_Close, bpos)) {
      fvec2 cpos = pLayout.Pos + bpos;
      pLayout.DrawList.DrawLine(cpos, cpos + size, bclr, 2);
      pLayout.DrawList.DrawLine(cpos + fvec2(0, size.y),
                                cpos + fvec2(size.x, 0), bclr, 2);
    }
    if (pTitleButtonPos(TitleButton_Maximize, bpos)) {
      fvec2 cpos = pLayout.Pos + bpos;
      if (pIsMaximized) {
        // Restore sym
        float o = std::floor(size.x * 0.3f);
        pLayout.DrawList.DrawRect(cpos + fvec2(o, 0), size - fvec2(o), bclr, 2);
        pLayout.DrawList.DrawRect(cpos + fvec2(0, o), size - fvec2(o), bclr, 2);
      } else {
        pLayout.DrawList.DrawRect(cpos, size, bclr, 2);
      }
    }
    if (pTitleButtonPos(TitleButton_Minimize, bpos)) {
      fvec2 cpos = pLayout.Pos + bpos;
      // center the minimize sym (win10/11)
      float cy = std::floor(size.y * 0.5f);
      pLayout.DrawList.DrawLine(cpos + fvec2(0, cy), cpos + fvec2(size.x, cy),
                                bclr, 2);
    }
    pLayout.DrawList.SetLayer(0);
  }
  // Menu border
  if (pIO.ShowMenuBorder) {
    DynObj* r = pIO.DynObjPool.Allocate();
    *r = UI7::DynObj([](IO* io, Li::Drawlist* l, UI7::Container* self) {
      l->SetLayer(22);  // Over everything
      fvec2 p = self->FinalPos() + fvec2(0.5f);
      fvec2 s = self->GetSize() - fvec2(1.f);
      l->PathRect(p, p + s, 0.f);
      l->PathStroke(io->Theme.Get(UI7Color_Border), 1, LiDrawFlags_Close);
      l->SetLayer(0);
    });
    r->SetSize(pIsOpen ? pLayout.GetSize()
                       : fvec2(pLayout.GetSize().x, TitleBarHeight));
    r->SetPos(0);
    pLayout.AddObjectEx(r, UI7LytAdd_NoCursorUpdate | UI7LytAdd_NoScrollHandle |
                               UI7LytAdd_NoClipTitlebar);
  }
}

PD_API void Menu::Update() {
  HandleFocus();
  const bool n3w = pLayout.Size == fvec2(0.f);
  if (pIsMaximized) {
    const ivec4& vp = pIO.CurrentViewPort.pSize;
    pLayout.Pos = fvec2(vp.x, vp.y);
    pLayout.Size = fvec2(vp.z - vp.x, vp.w - vp.y);
  } else if (pIsOpen && (n3w || Flags & UI7MenuFlags_AlwaysAutoSize)) {
    pLayout.Size = fvec2(pLayout.MaxPosition) + pIO.MenuPadding * 2;
  }
  // keep window sized by viewport (maybe the issue of this will be fixed if
  // were able to resize windows from all sides and corners in future)
  if (!pIsMaximized && n3w) {
    const ivec4& vp = pIO.CurrentViewPort.pSize;
    fvec2 avail(vp.z - std::max(pLayout.Pos.x, (float)vp.x),
                vp.w - std::max(pLayout.Pos.y, (float)vp.y));
    avail.x = std::max(avail.x, 50.f);
    avail.y = std::max(avail.y, TitleBarHeight + 20.f);
    pLayout.Size.x = std::min(pLayout.Size.x, avail.x);
    pLayout.Size.y = std::min(pLayout.Size.y, avail.y);
  }
  // Rebuild layout flags to take changes of menu flags
  pLayout.Flags = 0;
  if (!(Flags & UI7MenuFlags_NoClipRect))
    pLayout.Flags |= UI7LayoutFlags_UseClipRect;
  if (Flags & UI7MenuFlags_VtScrolling)
    pLayout.Flags |= UI7LayoutFlags_VtScrolling;
  if (Flags & UI7MenuFlags_HzScrolling)
    pLayout.Flags |= UI7LayoutFlags_HzScrolling;
  if (Flags & UI7MenuFlags_NoScrollbar)
    pLayout.Flags |= UI7LayoutFlags_NoScrollbar;
  pLayout.pSkipScrolling = !pIsOpen;
  pLayout.pScrollPadding = pCanResize() ? pIO.ResizeGripSize : 0.f;
  pLayout.WorkRect.x = pIO.MenuPadding.x;
  if (!(Flags & UI7MenuFlags_NoTitlebar)) {
    TitleBarHeight = pIO.FontScale * pIO.Font->PixelHeight + pIO.MenuPadding.y;
    pLayout.WorkRect.y = pIO.MenuPadding.y + TitleBarHeight;
    HandleTitlebarActions();
  } else {
    TitleBarHeight = 0.f;
    pLayout.WorkRect.y = pIO.MenuPadding.y;
  }
  pUpdateScrollAnim();
  // TODO: Add function to layout to fix out this the folowing problem
  // Make sure windows have fully rounded coords
  pLayout.Pos.x = std::floor(pLayout.Pos.x);
  pLayout.Pos.y = std::floor(pLayout.Pos.y);
  pLayout.Size.x = std::round(pLayout.Size.x);
  pLayout.Size.y = std::round(pLayout.Size.y);
  DrawBaseLayout();
  pLayout.Update();
}

PD_API bool Menu::BeginTreeNode(const ID& id) {
  // As of some notes this should work:
  auto n = pTreeNodes.find(id);
  if (n == pTreeNodes.end()) {
    pTreeNodes[id] = false;
    n = pTreeNodes.find(id);
  }
  fvec2 pos = pLayout.Cursor;
  fvec2 tdim = pIO.Font->GetTextBounds(id.GetName().c_str(), pIO.FontScale);
  fvec2 szs = tdim + fvec2(pIO.ItemSpace.x + 10, 0);

  if (n->second) {
    pLayout.InitialCursorOffset += 10.f;
  }

  // Object
  DynObj* r = pIO.DynObjPool.Allocate();
  *r = UI7::DynObj([=, this](IO* io, Li::Drawlist* l, Container* self) {
    l->SetFont(self->GetFont());
    fvec2 ts = self->FinalPos() + fvec2(0, 7);
    fvec2 pl[2] = {fvec2(10, 5), fvec2(0, 10)};
    if (n->second) {
      float t = pl[0].y;
      pl[0].y = pl[1].x;
      pl[1].x = t;
    }
    l->DrawTriangleFilled(ts, ts + pl[0], ts + pl[1],
                          io->Theme.Get(UI7Color_FrameBackground));

    l->DrawText(self->FinalPos() + fvec2(10 + io->ItemSpace.x, 0),
                id.GetName().c_str(), io->Theme.Get(UI7Color_Text));
  });
  /** Yes this new function handler was created for tree nodes */
  r->AddInputHandler([=, this](IO* io, Container* self) {
    if (io->InputHandler.DragObject(ID(pID.GetName() + id.GetName()),
                                    fvec4(self->FinalPos(), self->GetSize()))) {
      if (io->InputHandler.DragReleased) {
        n->second = !n->second;
      }
    }
  });
  r->SetPos(pos);
  r->SetSize(szs);
  /** Use Add Object as it is faster */
  pLayout.AddObject(r);

  return n->second;
}

PD_API void Menu::EndTreeNode() {
  pLayout.InitialCursorOffset.x -= 10.f;
  pLayout.Cursor.x -= 10.f;
  if (pLayout.InitialCursorOffset.x < 0.f) {
    pLayout.InitialCursorOffset.x = 0.f;
  }
}

PD_API void Menu::pCollapseSym() {
  if (!(Flags & UI7MenuFlags_NoCollapse)) {
    DynObj* r = pIO.DynObjPool.Allocate();
    *r = UI7::DynObj([=, this](UI7::IO* io, Li::Drawlist* l,
                               UI7::Container* self) {
      /** This sym actually requires layer 21 (i dont know why) */
      l->SetLayer(21);
      /**
       * Symbol (Position Swapping set by pIsOpen ? openpos : closepos;)
       */
      l->DrawTriangleFilled(
          self->FinalPos(),
          self->FinalPos() +
              fvec2(self->GetSize().x, pIsOpen ? 0 : self->GetSize().y * 0.5),
          self->FinalPos() +
              fvec2(pIsOpen ? self->GetSize().x * 0.5 : 0, self->GetSize().y),
          io->Theme.Get(UI7Color_FrameBackground));
      l->SetLayer(0);
    });
    r->SetSize(pTitleButtonSize());
    r->SetPos(pIO.FramePadding);
    pLayout.AddObjectEx(r, UI7LytAdd_NoCursorUpdate | UI7LytAdd_NoScrollHandle |
                               UI7LytAdd_NoClipTitlebar);
  }
}

PD_API void Menu::pTitlebar() {  // calculate lost space of left and right to
                                 // support center aligned title
  // (0.3.2 feature)
  float left = (Flags & UI7MenuFlags_NoCollapse)
                   ? pIO.FramePadding.x
                   : (pTitleButtonSize().x + (pIO.FramePadding.x * 2));
  float right =
      pNumTitleButtons() * (pTitleButtonSize().x + pIO.FramePadding.x) +
      pIO.FramePadding.x;
  DynObj* r = pIO.DynObjPool.Allocate();
  *r = UI7::DynObj(
      [=, this](UI7::IO* io, Li::Drawlist* l, UI7::Container* self) {
        l->SetFont(self->GetFont());
        l->SetLayer(20);
        /** Header Bar */
        l->DrawRectFilled(self->FinalPos(), self->GetSize(),
                          io->Theme.Get(UI7Color_Header));
        l->SetLayer(21);
        const std::string title = pID.GetName();
        float x = left;
        if (Flags & UI7MenuFlags_CenterTitle) {
          float tw = io->Font->GetTextBounds(title.c_str(), io->FontScale).x;
          // center
          x = (self->GetSize().x - tw) * 0.5f;
          x = std::min(x, self->GetSize().x - right - tw);
          x = std::max(x, left);
        }
        // clip the title for not draw into the buttons
        l->PushClipRect(fvec4(self->FinalPos().x + left, self->FinalPos().y,
                              std::max(self->GetSize().x - left - right, 0.f),
                              self->GetSize().y));
        l->DrawText(self->FinalPos() + fvec2(x, 2), title.c_str(),
                    io->Theme.Get(UI7Color_Text));
        l->PopClipRect();
        l->SetLayer(0);
      });
  r->SetSize(fvec2(pLayout.GetSize().x, TitleBarHeight));
  r->SetPos(0);
  pLayout.AddObjectEx(r, UI7LytAdd_NoCursorUpdate | UI7LytAdd_NoScrollHandle |
                             UI7LytAdd_NoClipTitlebar);
}
}  // namespace UI7
}  // namespace PD
