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

#include <pd/core/core.hpp>
#include <pd/ui7/menu.hpp>

namespace PD {
namespace UI7 {
PD_UI7_API void UI7::Menu::Label(const std::string& label) {
  // Layout API
  auto r = PD::New<UI7::Label>(label, io);
  Layout->AddObject(r);
}

PD_UI7_API bool UI7::Menu::Button(const std::string& label) {
  bool ret = false;
  u32 id = Strings::FastHash("btn" + label + std::to_string(count_btn++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::Button>(label, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
  if (!r->Skippable()) {
    ret = std::static_pointer_cast<UI7::Button>(r)->IsPressed();
  }
  return ret;
}

PD_UI7_API void UI7::Menu::ColorEdit(const std::string& label, u32* color) {
  u32 id = Strings::FastHash("cle" + label + std::to_string(count_btn++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::ColorEdit>(label, color, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

PD_UI7_API void UI7::Menu::DragFloat(const std::string& label, float* data,
                                     size_t num_elms) {
  u32 id = Strings::FastHash("dfl" + label + std::to_string(count_btn++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::DragData<float>>(label, data, num_elms, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

PD_UI7_API void UI7::Menu::Checkbox(const std::string& label, bool& v) {
  u32 id = Strings::FastHash("cbx" + label + std::to_string(count_cbx++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::Checkbox>(label, v, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

PD_UI7_API void UI7::Menu::Image(LI::Texture::Ref img, fvec2 size,
                                 LI::Rect uv) {
  Container::Ref r = PD::New<UI7::Image>(img, size, uv);
  Layout->AddObject(r);
}

PD_UI7_API void UI7::Menu::DebugLabels(Menu::Ref m, Menu::Ref t) {
  /*if (!m) {
    return;
  }
  if (t == nullptr) {
    t = m;
  }
  std::stringstream s;
  s << "Name: " << m->name << " [";
  s << std::hex << std::setw(8) << std::setfill('0') << m->id;
  s << std::dec << "]";
  t->Label(s.str());
  t->Label(std::format("Max Size: {:.2f}, {:.2f}", m->Layout->MaxPosition.x(),
                       m->Layout->MaxPosition.y()));
  t->Label(std::format("Pos: {:.2f}, {:.2f} Size: {:.2f}, {:.2f}",
                       m->Layout->Pos.x(), m->Layout->Pos.y(),
                       m->Layout->Size.x(), m->Layout->Size.y()));
  t->Label(std::format("Flags: {:#08x}", m->flags));
  t->Label(
      "Pre: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MPRE_" + m->name)->GetProtocol()->GetAverage()));
  t->Label(
      "Post: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MPOS_" + m->name)->GetProtocol()->GetAverage()));
  t->Label(
      "Update: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MUPT_" + m->name)->GetProtocol()->GetAverage()));
  t->Label(
      "MUser: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MUSR_" + m->name)->GetProtocol()->GetAverage()));*/
}

PD_UI7_API void UI7::Menu::Update(float delta) {
  TT::Scope st("MUPT_" + name);
  MenuFocusHandler();
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    CollapseHandler();
    CloseButtonHandler();
    MoveHandler();
  }
  scroll_anim.Update(delta);
  if (!scroll_anim.IsFinished()) {
    Layout->ScrollOffset = scroll_anim;
  }
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PushClipRect(
        fvec4(Layout->Pos.x + io->MenuPadding.x, Layout->Pos.y + tbh,
              Layout->Size.x - io->MenuPadding.x, Layout->Size.y - tbh));
  }
  Layout->DrawList->Layer = 10;
  Layout->Update();
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PopClipRect();
  }
  PostScrollHandler();
}

PD_UI7_API void UI7::Menu::PreHandler(UI7MenuFlags flags) {
  TT::Scope st("MPRE_" + name);
  // No touch means no Input System
  if (!has_touch) {
    header = UI7Color_Header;
  }
  if (io->InputHandler->FocusedMenu == id) {
    header = UI7Color_Header;
  }
  DrawList::Ref list = Layout->GetDrawList();
  // Resetup [updating] variables
  count_btn = 0;
  count_cbx = 0;
  tbh = 0.f;
  this->flags = flags;
  Layout->Scrolling[1] = flags & UI7MenuFlags_VtScrolling;
  has_touch = true;  // io->Ren->CurrentScreen()->ScreenType() ==
                     // Screen::Bottom;
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    // Title bar setup and Rendering
    tbh = io->FontScale * io->Font->DefaultPixelHeight;
    list->Layer = 20;
    list->AddRectangle(Layout->Pos, fvec2(Layout->Size.x, tbh),
                       io->Theme->Get(header));
    fvec2 tpos(
        io->MenuPadding.x,
        tbh * 0.5 - io->Font->GetTextBounds(name, io->FontScale).y * 0.5);
    if (!(flags & UI7MenuFlags_NoCollapse)) {
      tpos.x += 18;
    }
    // LITextFlags tflags = LITextFlags_None;
    if (flags & UI7MenuFlags_CenterTitle) {
      tpos = 0;
      // tflags = LITextFlags_AlignMid;
    }
    list->Layer++;
    if (!(flags & UI7MenuFlags_NoClipRect)) {
      int extra = is_shown != nullptr && !(flags & UI7MenuFlags_NoClose)
                      ? (20 + io->ItemSpace.x)
                      : 0;
      Layout->DrawList->PushClipRect(
          fvec4(Layout->Pos, fvec2(Layout->Size.x - extra, tbh)));
    }
    list->AddText(Layout->Pos + tpos, this->name, io->Theme->Get(UI7Color_Text),
                  0, fvec2(Layout->Size.x, tbh));
    if (!(flags & UI7MenuFlags_NoClipRect)) {
      Layout->DrawList->PopClipRect();
    }

    /// Close Button Rendering
    if (!(flags & UI7MenuFlags_NoClose) && is_shown) {
      fvec2 size = tbh - io->FramePadding.y * 2;  // Fixed quad size
      // Need to clamp this way as the math lib lacks a less and greater
      // operator in vec2 (don't checked if it would make sense yet)
      size.x = std::clamp(size.x, 5.f, std::numeric_limits<float>::max());
      size.y = std::clamp(size.y, 5.f, std::numeric_limits<float>::max());
      // Probably should fix the minsize to be locked on y
      fvec2 cpos =
          fvec2(Layout->Pos.x + Layout->Size.x - size.x - io->FramePadding.x,
                Layout->Pos.y + io->FramePadding.y);
      Layout->DrawList->AddLine(cpos, cpos + size,
                                io->Theme->Get(UI7Color_FrameBackground), 2);
      Layout->DrawList->AddLine(cpos + fvec2(0, size.y),
                                cpos + fvec2(size.x, 0),
                                io->Theme->Get(UI7Color_FrameBackground), 2);
      /*fvec2 cpos =
          fvec2(Layout->Pos.x + Layout->Size.x - 12 - io->FramePadding.x,
                Layout->Pos.y + io->FramePadding.y);
      Layout->GetDrawList()->AddLine(cpos, cpos + 12,
                                     io->Theme->Get(clr_close_btn), 2);
      Layout->GetDrawList()->AddLine(cpos + fvec2(0, 12), cpos + fvec2(12, 0),
                                     io->Theme->Get(clr_close_btn), 2);*/
    }
    /// Collapse Triangle Rendering
    if (!(flags & UI7MenuFlags_NoCollapse)) {
      Layout->DrawList->Layer = 21;
      /** Fixed Size */
      fvec2 size = tbh - io->FramePadding.y * 2;
      fvec2 cpos = Layout->Pos + io->FramePadding;
      /** Symbol (Position Swapping set by pIsOpen ? openpos : closepos;) */
      Layout->DrawList->AddTriangleFilled(
          cpos, cpos + fvec2(size.x, is_open ? 0 : size.y * 0.5),
          cpos + fvec2(is_open ? size.x * 0.5 : 0, size.y),
          io->Theme->Get(UI7Color_FrameBackground));
      Layout->DrawList->Layer = 20;
      /*fvec2 cpos = Layout->Pos + io->FramePadding;
      fvec2 positions[2] = {
          fvec2(12, 6),
          fvec2(0, 12),
      };
      if (is_open) {
        float t = positions[0].y;
        positions[0].y = positions[1].x;
        positions[1].x = t;
      }
      Layout->GetDrawList()->AddTriangleFilled(
          cpos, cpos + positions[0], cpos + positions[1],
          io->Theme->Get(clr_collapse_tri));*/
    }
    Layout->WorkRect.y = io->MenuPadding.y + tbh;
    Layout->CursorInit();
  }
  if (!(flags & UI7MenuFlags_NoBackground) && is_open) {
    list->Layer = 0;
    list->AddRectangle(Layout->Pos + fvec2(0, tbh),
                       Layout->Size - fvec2(0, tbh),
                       io->Theme->Get(UI7Color_Background));
  }
  if (io->ShowMenuBorder) {
    vec2 bsize = Layout->Size;
    if (!is_open) {
      bsize.y = tbh;
    }
    list->Layer = 20;
    list->AddRect(Layout->Pos, bsize, io->Theme->Get(UI7Color_Border));
  }
  // Add a clip Rect for Separators
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PushClipRect(
        fvec4(Layout->Pos.x + io->MenuPadding.x, Layout->Pos.y + tbh,
              Layout->Size.x - io->MenuPadding.x, Layout->Size.y - tbh));
  }
  list->Layer = 10;
  TT::Beg("MUSR_" + name);
}

PD_UI7_API void UI7::Menu::PostHandler() {
  TT::Scope st("MPOS_" + name);
  TT::End("MUSR_" + name);
  // Remove the Clip Rect
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PopClipRect();
  }
  ResizeHandler();
  if (Layout->Scrolling[1]) {
    scroll_allowed[1] =
        (Layout->MaxPosition.y > Layout->Size.y - io->MenuPadding.y);
    if (Layout->MaxPosition.y < Layout->Size.y - io->MenuPadding.y) {
      Layout->ScrollOffset.y = 0.f;
    }
    scrollbar[1] = scroll_allowed[1];

    if (scrollbar[1]) {
      /// Setup Some Variables hare [they are self described]
      int screen_w = Layout->Size.x;
      int tsp = io->MenuPadding.y + tbh;
      int slider_w = 4;
      int szs = Layout->Size.y - tsp - io->MenuPadding.y;
      /// Actually dont have a Horizontal bar yet
      if (scrollbar[0]) szs -= slider_w - 2;
      int lslider_h =
          io->MinSliderDragSize.y;  // Dont go less heigt for the drag
      float slider_h = (szs - 4) * (float(szs - 4) / Layout->MaxPosition.y);
      /// Visual Slider Height (How it looks in the end)
      int vslider_h = std::clamp(slider_h, float(lslider_h), float(szs - 4));

      /// Check if we overscroll to the bottom and Auto scroll back...
      /// Probably schould use Tween ENgine here
      if (Layout->ScrollOffset.y > Layout->MaxPosition.y - Layout->Size.y &&
          Layout->MaxPosition.y != 0.f &&
          Layout->MaxPosition.y >= Layout->Size.y - io->MenuPadding.y) {
        Layout->ScrollOffset.y -= io->OverScrollMod * io->Delta;
        if (Layout->ScrollOffset.y < Layout->MaxPosition.y - Layout->Size.y) {
          Layout->ScrollOffset.y = Layout->MaxPosition.y - Layout->Size.y;
        }
      }

      /// Do the Same as above just for Overscroll back to the top
      if (Layout->ScrollOffset.y < 0) {
        Layout->ScrollOffset.y += io->OverScrollMod * io->Delta;
        if (Layout->ScrollOffset.y > 0) {
          Layout->ScrollOffset.y = 0;
        }
      }

      /// Effect
      /*if (scroll_mod[1] != 0) {
        Layout->ScrollOffset[1] += scroll_mod[1];
      }
      if (scroll_mod[1] < 0.f) {
        scroll_mod[1] += 0.4f;
        if (scroll_mod[1] > 0.f) {
          scroll_mod[1] = 0;
        }
      }
      if (scroll_mod[1] > 0.f) {
        scroll_mod[1] -= 0.4f;
        if (scroll_mod[1] < 0.f) {
          scroll_mod[1] = 0;
        }
      }*/
      UI7Color sldr_drag = UI7Color_Button;
      /// Slider Dragging????
      /// Probably need a new API for this
      if (has_touch &&
          io->InputHandler->DragObject(name + "sldr",
                                       fvec4(Layout->Pos.x + screen_w - 12,
                                             Layout->Pos.y + tsp, 8, szs)) &&
          !io->InputHandler->DragReleasedAW) {
        sldr_drag = UI7Color_ButtonHovered;
        float drag_center = vslider_h / 2.0f;
        float drag_pos =
            std::clamp(static_cast<float>(
                           ((io->InputHandler->DragPosition.y - Layout->Pos.y) -
                            tsp - drag_center) /
                           (szs - vslider_h - 4)),
                       0.0f, 1.0f);

        Layout->ScrollOffset.y =
            drag_pos * (Layout->MaxPosition.y - Layout->Size.y);
      }
      int srpos =
          tsp + std::clamp(float(szs - vslider_h - 4) *
                               (Layout->ScrollOffset.y /
                                (Layout->MaxPosition.y - Layout->Size.y)),
                           0.f, float(szs - vslider_h - 4));

      /// Rendering Stage
      auto list = Layout->DrawList;
      list->Layer = 20;
      list->AddRectangle(Layout->Pos + fvec2(screen_w - 12, tsp),
                         fvec2(slider_w * 2, szs),
                         io->Theme->Get(UI7Color_FrameBackground));
      list->AddRectangle(Layout->Pos + fvec2(screen_w - 10, tsp + 2),
                         fvec2(slider_w, szs - 4),
                         io->Theme->Get(UI7Color_FrameBackgroundHovered));
      list->AddRectangle(Layout->Pos + fvec2(screen_w - 10, srpos + 2),
                         fvec2(slider_w, vslider_h), io->Theme->Get(sldr_drag));
    }
  }
}

PD_UI7_API void UI7::Menu::Separator() {
  // Dynamic Objects are very simple...
  Container::Ref r = PD::New<UI7::DynObj>(
      [=, this](UI7::IO::Ref io, UI7::DrawList::Ref l, UI7::Container* self) {
        l->AddRect(self->FinalPos(), self->GetSize(),
                   io->Theme->Get(UI7Color_TextDead));
      });
  // Set size before pushing (cause Cursor Move will require it)
  r->SetSize(fvec2(Layout->Size.x - 10, 1));
  Layout->AddObject(r);
  /*return;
  vec2 pos = Layout->Cursor;
  vec2 size = fvec2(Layout->Size.x - (scrollbar[1] ? 24 : 10), 1);
  Layout->CursorMove(size);
  if (Layout->ObjectWorkPos(pos)) {
    return;
  }
  Layout->GetDrawList()->AddRectangle(Layout->Pos + pos, size,
                                      io->Theme->Get(UI7Color_TextDead));*/
}

PD_UI7_API void UI7::Menu::SeparatorText(const std::string& label) {
  // Also note to use [=, this] instead of [&] to not undefined access label
  Container::Ref r = PD::New<UI7::DynObj>(
      [=, this](UI7::IO::Ref io, UI7::DrawList::Ref l, UI7::Container* self) {
        fvec2 size = self->GetSize();
        fvec2 tdim = io->Font->GetTextBounds(label, io->FontScale);
        fvec2 pos = self->FinalPos();
        auto align = Layout->GetAlignment();
        vec2 rpos = Layout->AlignPosition(
            pos, tdim, fvec4(Layout->Pos, Layout->Size), align);
        if (!(align & UI7Align_Left)) {
          l->AddRectangle(fvec2(rpos.x + io->FramePadding.x, tdim.y * 0.5),
                          fvec2(pos.x - rpos.x - io->MenuPadding.x, 1),
                          io->Theme->Get(UI7Color_TextDead));
        }
        if (!(align & UI7Align_Right)) {
          l->AddRectangle(
              pos + fvec2(tdim.x + io->FramePadding.x, tdim.y * 0.5),
              fvec2(size.x - tdim.x - io->MenuPadding.x, 1),
              io->Theme->Get(UI7Color_TextDead));
        }
        l->AddText(rpos, label, io->Theme->Get(UI7Color_Text), 0,
                   fvec2(Layout->Size.x, self->GetSize().y));
      });
  // Set size before pushing (cause Cursor Move will require it)
  r->SetSize(fvec2(Layout->Size.x - 10, io->Font->PixelHeight * io->FontScale));
  Layout->AddObject(r);
  return;
  fvec2 size = fvec2(Layout->Size.x - (scrollbar[1] ? 24 : 10), 1);
  fvec2 tdim = io->Font->GetTextBounds(label, io->FontScale);
  fvec2 pos = Layout->Cursor;
  Layout->CursorMove(fvec2(size.x, tdim.y));

  if (Layout->ObjectWorkPos(pos)) {
    return;
  }
  auto alignment = Layout->GetAlignment();
  vec2 rpos = Layout->AlignPosition(Layout->Pos + pos, tdim,
                                    vec4(Layout->Pos, Layout->Size), alignment);

  if (!(alignment & UI7Align_Left)) {
    Layout->GetDrawList()->AddRectangle(
        rpos +
            fvec2(-(rpos.x - Layout->Pos.x - io->MenuPadding.x), tdim.y * 0.5),
        fvec2(rpos.x - Layout->Pos.x - io->MenuPadding.x - io->FramePadding.x,
              size.y),
        io->Theme->Get(UI7Color_TextDead));
  }
  if (!(alignment & UI7Align_Right)) {
    Layout->GetDrawList()->AddRectangle(
        rpos + fvec2(tdim.x + io->FramePadding.x, tdim.y * 0.5),
        fvec2(size.x - (tdim.x + io->FramePadding.x), size.y),
        io->Theme->Get(UI7Color_TextDead));
  }
  Layout->GetDrawList()->AddText(rpos, label, io->Theme->Get(UI7Color_Text), 0,
                                 fvec2(Layout->Size.x, 20));
}

PD_UI7_API void UI7::Menu::Join() {
  // Assert(Layout->Objects.size(), "Objects list is empty!");
  join.push_back(Layout->Objects.Back().get());
}

PD_UI7_API void UI7::Menu::JoinAlign(UI7Align a) {
  if (a == 0) {
    a = UI7Align_Default;
  }
  this->Join();

  fvec2 spos = join.front()->GetPos();
  fvec2 szs = join.back()->GetPos() + join.back()->GetSize() - spos;
  for (auto it : join) {
    szs.x = std::max(szs.x, it->GetPos().x + it->GetSize().x - spos.x);
  }
  fvec2 off;
  if (a & UI7Align_Center) {
    off.x = (Layout->Pos.x + Layout->Size.x * 0.5) - (spos.x + szs.x * 0.5);
  }
  if (a & UI7Align_Mid) {
    off.y = (Layout->Pos.y + Layout->Size.y * 0.5) - (spos.y + szs.y * 0.5);
  }
  for (auto it : join) {
    it->SetPos(it->GetPos() + off);
  }
  join.clear();
}

PD_UI7_API void UI7::Menu::AfterAlign(UI7Align a) {
  Container* ref = Layout->Objects.Back().get();
  fvec2 p = ref->GetPos();
  fvec2 s = ref->GetSize();
  fvec2 np = p;
  if (a & UI7Align_Center) {
    np.x = (Layout->Pos.x + Layout->Size.x * 0.5) - (p.x + s.x * 0.5);
  }
  if (a & UI7Align_Mid) {
    np.y = (Layout->Pos.y + Layout->Size.y * 0.5) - (p.y + s.y * 0.5);
  }
  ref->SetPos(np);
}

PD_UI7_API void UI7::Menu::CreateParent() {
  // Assert(!tmp_parent, "There is already an existing Parent container!");
  tmp_parent = Container::New();
  tmp_parent->SetPos(0);
  tmp_parent->SetSize(0);
}

PD_UI7_API bool UI7::Menu::BeginTreeNode(const UI7::ID& id) {
  auto n = tree_nodes.find((u32)id);
  if (n == tree_nodes.end()) {
    tree_nodes[(u32)id] = false;
    n = tree_nodes.find((u32)id);
  }
  fvec2 pos = Layout->Cursor;
  fvec2 tdim = io->Font->GetTextBounds(id.GetName(), io->FontScale);
  fvec2 size = fvec2(tdim.x + 10 + io->ItemSpace.x, tdim.y);
  if (n->second) {
    Layout->InitialCursorOffset.x += 10.f;
  }
  Layout->CursorMove(size);
  if (Layout->ObjectWorkPos(pos)) {
    return n->second;
  }
  fvec2 ts = Layout->Pos + pos + fvec2(0, 3);
  fvec2 positions[2] = {
      fvec2(10, 5),
      fvec2(0, 10),
  };
  if (n->second) {
    float t = positions[0].y;
    positions[0].y = positions[1].x;
    positions[1].x = t;
  }
  Layout->GetDrawList()->AddTriangleFilled(
      ts, ts + positions[0], ts + positions[1],
      io->Theme->Get(UI7Color_FrameBackground));
  Layout->GetDrawList()->AddText(
      Layout->Pos + pos + fvec2(10 + io->ItemSpace.x, 0), id.GetName(),
      io->Theme->Get(UI7Color_Text));
  if (has_touch && io->InputHandler->DragObject(
                       name + id.GetName(), vec4(Layout->Pos + pos, size))) {
    if (io->InputHandler->DragReleased) {
      n->second = !n->second;
      if (!n->second) {
        Layout->InitialCursorOffset.x -= 10;
        Layout->Cursor.x -= 10;
      }
    }
  }
  return n->second;
}

PD_UI7_API void UI7::Menu::EndTreeNode() {
  Layout->InitialCursorOffset.x -= 10.f;
  Layout->Cursor.x -= 10.f;
  if (Layout->InitialCursorOffset.x < 0.f) {
    Layout->InitialCursorOffset.x = 0.f;
  }
}

PD_UI7_API void UI7::Menu::CloseButtonHandler() {
  // Close Logic
  if (!(flags & UI7MenuFlags_NoClose) && is_shown != nullptr) {
    vec2 cpos = fvec2(Layout->Pos.x + Layout->Size.x - 12 - io->FramePadding.x,
                      Layout->Pos.y + io->FramePadding.y);

    clr_close_btn = UI7Color_FrameBackground;
    if (has_touch && io->InputHandler->DragObject(UI7::ID(name + "clse"),
                                                  fvec4(cpos, fvec2(12)))) {
      if (io->InputHandler->DragReleased) {
        *is_shown = !(*is_shown);
      }
      clr_close_btn = UI7Color_FrameBackgroundHovered;
    }
  }
}

PD_UI7_API void UI7::Menu::ResizeHandler() {
  if (!(flags & UI7MenuFlags_NoResize)) {
    if (has_touch &&
        io->InputHandler->DragObject(
            name + "rszs", fvec4(Layout->Pos + Layout->Size - 20, 20))) {
      vec2 szs = Layout->Size + (io->InputHandler->DragPosition -
                                 io->InputHandler->DragLastPosition);
      if (szs.x < 30) szs.x = 30;
      if (szs.y < 30) szs.y = 30;
      Layout->Size = szs;
    }
    Layout->DrawList->Layer = 21;
    Layout->DrawList->AddTriangleFilled(
        Layout->Pos + Layout->Size, Layout->Pos + Layout->Size - fvec2(0, 15),
        Layout->Pos + Layout->Size - fvec2(15, 0),
        io->Theme->Get(UI7Color_FrameBackground));
  }
}

PD_UI7_API void UI7::Menu::MoveHandler() {
  // Menu Movement
  if (!(flags & UI7MenuFlags_NoMove)) {
    if (has_touch &&
        io->InputHandler->DragObject(
            name + "tmv", fvec4(Layout->Pos, fvec2(Layout->Size.x, tbh)))) {
      if (io->InputHandler->DragDoubleRelease) {
        is_open = !is_open;
      }
      Layout->Pos = Layout->Pos + (io->InputHandler->DragPosition -
                                   io->InputHandler->DragLastPosition);
    }
  }
}

PD_UI7_API void UI7::Menu::CollapseHandler() {
  // Collapse logic
  if (!(flags & UI7MenuFlags_NoCollapse)) {
    vec2 cpos = Layout->Pos + io->FramePadding;
    clr_collapse_tri = UI7Color_FrameBackground;
    if (has_touch &&
        io->InputHandler->DragObject(UI7::ID(name + "clbse"),
                                     fvec4(cpos, fvec2(18, tbh)))) {
      if (io->InputHandler->DragReleased) {
        is_open = !is_open;
      }
      clr_collapse_tri = UI7Color_FrameBackgroundHovered;
    }
  }
}

PD_UI7_API void UI7::Menu::PostScrollHandler() {
  if (has_touch &&
      io->InputHandler->DragObject(id, vec4(Layout->Pos, Layout->Size)) &&
      Layout->Scrolling[1] && flags & UI7MenuFlags_VtScrolling &&
      Layout->MaxPosition.y - Layout->Size.y + io->MenuPadding.y > 0) {
    if (io->InputHandler->DragReleased) {
      // scroll_mod = (io->DragPosition - io->DragLastPosition);
    } else {
      Layout->ScrollOffset.y = std::clamp(
          Layout->ScrollOffset.y - (io->InputHandler->DragPosition.y -
                                    io->InputHandler->DragLastPosition.y),
          -40.f, (Layout->MaxPosition.y - Layout->Size.y) + 40.f);
    }
  }
}

PD_UI7_API void UI7::Menu::MenuFocusHandler() {
  // Check if menu can be focused for Selective Menu Input API
  vec4 newarea = vec4(Layout->Pos, Layout->Size);
  if (!is_open) {
    newarea = fvec4(Layout->Pos, fvec2(Layout->Size.x, tbh));
  }
  if (has_touch && io->Inp->IsDown(io->Inp->Touch) &&
      io->Ren->InBox(io->Inp->TouchPos(), newarea) &&
      !io->Ren->InBox(io->Inp->TouchPos(), io->InputHandler->FocusedMenuRect)) {
    io->InputHandler->FocusedMenu = id;
  }
  if (io->InputHandler->FocusedMenu == id) {
    io->InputHandler->FocusedMenuRect = newarea;
  }
}
}  // namespace UI7
}  // namespace PD