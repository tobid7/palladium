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

#include <pd/core/sys.hpp>
#include <pd/core/timetrace.hpp>
#include <pd/ui7/menu.hpp>

namespace PD {
namespace UI7 {
void UI7::Menu::Label(const std::string& label) {
  // Layout API
  auto r = PD::New<UI7::Label>(label, io->Ren);
  Layout->AddObject(r);
}

bool UI7::Menu::Button(const std::string& label) {
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

void UI7::Menu::ColorEdit(const std::string& label, u32* color) {
  u32 id = Strings::FastHash("cle" + label + std::to_string(count_btn++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::ColorEdit>(label, color, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

void UI7::Menu::DragFloat(const std::string& label, float* data,
                          size_t num_elms) {
  u32 id = Strings::FastHash("dfl" + label + std::to_string(count_btn++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::DragData<float>>(label, data, num_elms, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

void UI7::Menu::Checkbox(const std::string& label, bool& v) {
  u32 id = Strings::FastHash("cbx" + label + std::to_string(count_cbx++));
  Container::Ref r = Layout->FindObject(id);
  if (!r) {
    r = PD::New<UI7::Checkbox>(label, v, io);
    r->SetID(id);
  }
  Layout->AddObject(r);
}

void UI7::Menu::Image(Texture::Ref img, vec2 size, LI::Rect uv) {
  Container::Ref r = PD::New<UI7::Image>(img, size, uv);
  Layout->AddObject(r);
}

void UI7::Menu::DebugLabels(Menu::Ref m, Menu::Ref t) {
  if (!m) {
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
          Sys::GetTraceRef("MUSR_" + m->name)->GetProtocol()->GetAverage()));
}

void UI7::Menu::Update(float delta) {
  TT::Scope st("MUPT_" + name);
  scroll_anim.Update(delta);
  if (!scroll_anim.IsFinished()) {
    Layout->ScrollOffset = scroll_anim;
  }
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PushClipRect(
        vec4(Layout->Pos.x() + io->MenuPadding[0], Layout->Pos.y() + tbh,
             Layout->Pos.x() + Layout->Size.x() - io->MenuPadding[0],
             Layout->Pos.y() + Layout->Size.y()));
  }
  Layout->GetDrawList()->Layer(10);
  Layout->Update();
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PopClipRect();
  }
  PostScrollHandler();
}

void UI7::Menu::PreHandler(UI7MenuFlags flags) {
  TT::Scope st("MPRE_" + name);
  // No touch means no Input System
  UI7Color header = has_touch ? UI7Color_HeaderDead : UI7Color_Header;
  DrawList::Ref list = Layout->GetDrawList();
  // Check if menu can be focused for Selective Menu Input API
  vec4 newarea = vec4(Layout->Pos, Layout->Size);
  if (!is_open) {
    newarea = vec4(Layout->Pos, vec2(Layout->Size.x(), tbh));
  }
  if (has_touch && io->Inp->IsDown(io->Inp->Touch) &&
      io->Ren->InBox(io->Inp->TouchPos(), newarea) &&
      !io->Ren->InBox(io->Inp->TouchPos(), io->FocusedMenuRect)) {
    io->FocusedMenu = id;
  }
  if (io->FocusedMenu == id) {
    io->FocusedMenuRect = newarea;
    header = UI7Color_Header;
  }
  // Resetup [updating] variables
  count_btn = 0;
  count_cbx = 0;
  tbh = 0.f;
  this->flags = flags;
  Layout->Scrolling[1] = flags & UI7MenuFlags_VtScrolling;
  has_touch = io->Ren->CurrentScreen()->ScreenType() == Screen::Bottom;
  if (!(flags & UI7MenuFlags_NoBackground) && is_open) {
    list->Layer(0);
    list->AddRectangle(Layout->Pos + vec2(0, tbh), Layout->Size - vec2(0, tbh),
                       io->Theme->Get(UI7Color_Background));
  }
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    // Title bar setup and Rendering
    tbh = io->Ren->TextScale() * 30.f;
    list->Layer(20);
    list->AddRectangle(Layout->Pos, vec2(Layout->Size.x(), tbh),
                       io->Theme->Get(header));
    vec2 tpos(io->MenuPadding[0],
              tbh * 0.5 - io->Ren->GetTextDimensions(name).y() * 0.5);
    if (!(flags & UI7MenuFlags_NoCollapse)) {
      tpos[0] += 18;
    }
    LITextFlags tflags = LITextFlags_None;
    if (flags & UI7MenuFlags_CenterTitle) {
      tpos = 0;
      tflags = LITextFlags_AlignMid;
    }
    list->Layer(list->Layer() + 1);
    if (!(flags & UI7MenuFlags_NoClipRect)) {
      int extra = is_shown != nullptr && !(flags & UI7MenuFlags_NoClose)
                      ? (20 + io->ItemSpace.x())
                      : 0;
      Layout->DrawList->PushClipRect(
          vec4(Layout->Pos, vec2(Layout->Pos.x() + Layout->Size.x() - extra,
                                 Layout->Pos.y() + tbh)));
    }
    list->AddText(Layout->Pos + tpos, this->name, io->Theme->Get(UI7Color_Text),
                  tflags, vec2(Layout->Size.x(), tbh));
    if (!(flags & UI7MenuFlags_NoClipRect)) {
      Layout->DrawList->PopClipRect();
    }
    Layout->WorkRect[1] = io->MenuPadding[1] + tbh;
    Layout->CursorInit();
    CollapseHandler();
    CloseButtonHandler();
    MoveHandler();
  }
  // Add a clip Rect for Separators
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PushClipRect(
        vec4(Layout->Pos.x() + io->MenuPadding[0], Layout->Pos.y() + tbh,
             Layout->Pos.x() + Layout->Size.x() - io->MenuPadding[0],
             Layout->Pos.y() + Layout->Size.y()));
  }
  list->Layer(10);
  TT::Beg("MUSR_" + name);
}

void UI7::Menu::PostHandler() {
  TT::Scope st("MPOS_" + name);
  TT::End("MUSR_" + name);
  ResizeHandler();
  if (Layout->Scrolling[1]) {
    scroll_allowed[1] =
        (Layout->MaxPosition[1] > Layout->Size.y() - io->MenuPadding[1]);
    if (Layout->MaxPosition[1] < Layout->Size.y() - io->MenuPadding[1]) {
      Layout->ScrollOffset[1] = 0.f;
    }
    scrollbar[1] = scroll_allowed[1];

    if (scrollbar[1]) {
      /// Setup Some Variables hare [they are self described]
      int screen_w = Layout->Size.x();
      int tsp = io->MenuPadding[1] + tbh;
      int slider_w = 4;
      int szs = Layout->Size.y() - tsp - io->MenuPadding[1];
      /// Actually dont have a Horizontal bar yet
      if (scrollbar[0]) szs -= slider_w - 2;
      int lslider_h =
          io->MinSliderDragSize.y();  // Dont go less heigt for the drag
      float slider_h = (szs - 4) * (float(szs - 4) / Layout->MaxPosition[1]);
      /// Visual Slider Height (How it looks in the end)
      int vslider_h = std::clamp(slider_h, float(lslider_h), float(szs - 4));

      /// Check if we overscroll to the bottom and Auto scroll back...
      /// Probably schould use Tween ENgine here
      if (Layout->ScrollOffset[1] > Layout->MaxPosition[1] - Layout->Size.y() &&
          Layout->MaxPosition[1] != 0.f &&
          Layout->MaxPosition[1] >= Layout->Size.y() - io->MenuPadding[1]) {
        Layout->ScrollOffset[1] -= io->OverScrollMod * io->Delta;
        if (Layout->ScrollOffset[1] <
            Layout->MaxPosition[1] - Layout->Size.y()) {
          Layout->ScrollOffset[1] = Layout->MaxPosition[1] - Layout->Size.y();
        }
      }

      /// Do the Same as above just for Overscroll back to the top
      if (Layout->ScrollOffset[1] < 0) {
        Layout->ScrollOffset[1] += io->OverScrollMod * io->Delta;
        if (Layout->ScrollOffset[1] > 0) {
          Layout->ScrollOffset[1] = 0;
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
          io->DragObject(name + "sldr", vec4(Layout->Pos.x() + screen_w - 12,
                                             Layout->Pos.y() + tsp, 8, szs)) &&
          !io->DragReleasedAW) {
        sldr_drag = UI7Color_ButtonHovered;
        float drag_center = vslider_h / 2.0f;
        float drag_pos = std::clamp(
            static_cast<float>(
                ((io->DragPosition[1] - Layout->Pos.y()) - tsp - drag_center) /
                (szs - vslider_h - 4)),
            0.0f, 1.0f);

        Layout->ScrollOffset[1] =
            drag_pos * (Layout->MaxPosition[1] - Layout->Size.y());
      }
      int srpos =
          tsp + std::clamp(float(szs - vslider_h - 4) *
                               (Layout->ScrollOffset[1] /
                                (Layout->MaxPosition[1] - Layout->Size.y())),
                           0.f, float(szs - vslider_h - 4));

      /// Rendering Stage
      auto list = Layout->DrawList;
      list->Layer(20);
      list->AddRectangle(Layout->Pos + vec2(screen_w - 12, tsp),
                         vec2(slider_w * 2, szs),
                         io->Theme->Get(UI7Color_FrameBackground));
      list->AddRectangle(Layout->Pos + vec2(screen_w - 10, tsp + 2),
                         vec2(slider_w, szs - 4),
                         io->Theme->Get(UI7Color_FrameBackgroundHovered));
      list->AddRectangle(Layout->Pos + vec2(screen_w - 10, srpos + 2),
                         vec2(slider_w, vslider_h), io->Theme->Get(sldr_drag));
    }
  }
  // Remove the Clip Rect
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    Layout->DrawList->PopClipRect();
  }
}

void UI7::Menu::Separator() {
  vec2 pos = Layout->Cursor;
  vec2 size = vec2(Layout->Size.x() - (scrollbar[1] ? 24 : 10), 1);
  Layout->CursorMove(size);
  if (Layout->ObjectWorkPos(pos)) {
    return;
  }
  Layout->GetDrawList()->AddRectangle(Layout->Pos + pos, size,
                                      io->Theme->Get(UI7Color_TextDead));
}

void UI7::Menu::SeparatorText(const std::string& label) {
  vec2 size = vec2(Layout->Size.x() - (scrollbar[1] ? 24 : 10), 1);
  vec2 tdim = io->Ren->GetTextDimensions(label);
  vec2 pos = Layout->Cursor;
  Layout->CursorMove(vec2(size.x(), tdim.y()));

  if (Layout->ObjectWorkPos(pos)) {
    return;
  }
  auto alignment = Layout->GetAlignment();
  vec2 rpos = Layout->AlignPosition(Layout->Pos + pos, tdim,
                                    vec4(Layout->Pos, Layout->Size), alignment);

  if (!(alignment & UI7Align_Left)) {
    Layout->GetDrawList()->AddRectangle(
        rpos + vec2(-(rpos[0] - Layout->Pos.x() - io->MenuPadding[0]),
                    tdim.y() * 0.5),
        vec2(rpos[0] - Layout->Pos.x() - io->MenuPadding[0] -
                 io->FramePadding[0],
             size.y()),
        io->Theme->Get(UI7Color_TextDead));
  }
  if (!(alignment & UI7Align_Right)) {
    Layout->GetDrawList()->AddRectangle(
        rpos + vec2(tdim.x() + io->FramePadding[0], tdim.y() * 0.5),
        vec2(size.x() - (tdim.x() + io->FramePadding[0]), size.y()),
        io->Theme->Get(UI7Color_TextDead));
  }
  Layout->GetDrawList()->AddText(rpos, label, io->Theme->Get(UI7Color_Text), 0,
                                 vec2(Layout->Size.x(), 20));
}

void UI7::Menu::Join() {
  Assert(Layout->Objects.size(), "Objects list is empty!");
  join.push_back(Layout->Objects.back().get());
}

void UI7::Menu::JoinAlign(UI7Align a) {
  if (a == 0) {
    a = UI7Align_Default;
  }
  this->Join();

  vec2 spos = join.front()->GetPos();
  vec2 szs = join.back()->GetPos() + join.back()->GetSize() - spos;
  for (auto it : join) {
    szs.x() =
        std::max(szs.x(), it->GetPos().x() + it->GetSize().x() - spos.x());
  }
  vec2 off;
  if (a & UI7Align_Center) {
    off[0] =
        (Layout->Pos.x() + Layout->Size.x() * 0.5) - (spos[0] + szs[0] * 0.5);
  }
  if (a & UI7Align_Mid) {
    off[1] =
        (Layout->Pos.y() + Layout->Size.y() * 0.5) - (spos[1] + szs[1] * 0.5);
  }
  for (auto it : join) {
    it->SetPos(it->GetPos() + off);
  }
  join.clear();
}

void UI7::Menu::AfterAlign(UI7Align a) {
  Container* ref = Layout->Objects.back().get();
  vec2 p = ref->GetPos();
  vec2 s = ref->GetSize();
  vec2 np = p;
  if (a & UI7Align_Center) {
    np[0] = (Layout->Pos.x() + Layout->Size.x() * 0.5) - (p[0] + s[0] * 0.5);
  }
  if (a & UI7Align_Mid) {
    np[1] = (Layout->Pos.y() + Layout->Size.y() * 0.5) - (p[1] + s[1] * 0.5);
  }
  ref->SetPos(np);
}

void UI7::Menu::CreateParent() {
  Assert(!tmp_parent, "There is already an existing Parent container!");
  tmp_parent = Container::New();
  tmp_parent->SetPos(0);
  tmp_parent->SetSize(0);
}

bool UI7::Menu::BeginTreeNode(const UI7::ID& id) {
  auto n = tree_nodes.find((u32)id);
  if (n == tree_nodes.end()) {
    tree_nodes[(u32)id] = false;
    n = tree_nodes.find((u32)id);
  }
  vec2 pos = Layout->Cursor;
  vec2 tdim = io->Ren->GetTextDimensions(id.GetName());
  vec2 size = vec2(tdim.x() + 10 + io->ItemSpace[0], tdim.y());
  if (n->second) {
    Layout->InitialCursorOffset.x() += 10.f;
  }
  Layout->CursorMove(size);
  if (Layout->ObjectWorkPos(pos)) {
    return n->second;
  }
  vec2 ts = Layout->Pos + pos + vec2(0, 3);
  vec2 positions[2] = {
      vec2(10, 5),
      vec2(0, 10),
  };
  if (n->second) {
    float t = positions[0].y();
    positions[0].y() = positions[1].x();
    positions[1].x() = t;
  }
  Layout->GetDrawList()->AddTriangle(ts, ts + positions[0], ts + positions[1],
                                     io->Theme->Get(UI7Color_FrameBackground));
  Layout->GetDrawList()->AddText(
      Layout->Pos + pos + vec2(10 + io->ItemSpace[0], 0), id.GetName(),
      io->Theme->Get(UI7Color_Text));
  if (has_touch &&
      io->DragObject(name + id.GetName(), vec4(Layout->Pos + pos, size))) {
    if (io->DragReleased) {
      n->second = !n->second;
      if (!n->second) {
        Layout->InitialCursorOffset.x() -= 10;
        Layout->Cursor.x() -= 10;
      }
    }
  }
  return n->second;
}

void UI7::Menu::EndTreeNode() {
  Layout->InitialCursorOffset.x() -= 10.f;
  Layout->Cursor.x() -= 10.f;
  if (Layout->InitialCursorOffset.x() < 0.f) {
    Layout->InitialCursorOffset.x() = 0.f;
  }
}

void UI7::Menu::CloseButtonHandler() {
  // Close Logic
  if (!(flags & UI7MenuFlags_NoClose) && is_shown != nullptr) {
    vec2 cpos =
        vec2(Layout->Pos.x() + Layout->Size.x() - 12 - io->FramePadding.x(),
             Layout->Pos.y() + io->FramePadding.y());

    UI7Color clr = UI7Color_FrameBackground;
    if (has_touch &&
        io->DragObject(UI7::ID(name + "clse"), vec4(cpos, vec2(12)))) {
      if (io->DragReleased) {
        *is_shown = !(*is_shown);
      }
      clr = UI7Color_FrameBackgroundHovered;
    }
    Layout->GetDrawList()->AddLine(cpos, cpos + 12, io->Theme->Get(clr), 2);
    Layout->GetDrawList()->AddLine(cpos + vec2(0, 12), cpos + vec2(12, 0),
                                   io->Theme->Get(clr), 2);
  }
}

void UI7::Menu::ResizeHandler() {
  if (!(flags & UI7MenuFlags_NoResize)) {
    if (has_touch &&
        io->DragObject(name + "rszs",
                       vec4(Layout->Pos + Layout->Size - 20, 20))) {
      vec2 szs = Layout->Size + (io->DragPosition - io->DragLastPosition);
      if (szs.x() < 30) szs[0] = 30;
      if (szs.y() < 30) szs[1] = 30;
      Layout->Size = szs;
    }
    Layout->DrawList->AddTriangle(Layout->Pos + Layout->Size,
                                  Layout->Pos + Layout->Size - vec2(0, 10),
                                  Layout->Pos + Layout->Size - vec2(10, 0),
                                  io->Theme->Get(UI7Color_FrameBackground));
    // front->AddRectangle(Layout->Pos + Layout->Size - 20, 20,
    // 0xffffffff); Not vidible dor some reason
    // int l = front->Layer();
    // front->Layer(l + 1);
    // front->AddTriangle(10, vec2(10, 0), vec2(10, 0), 0xffffffff);
    // front->Layer(l);
  }
}

void UI7::Menu::MoveHandler() {
  // Menu Movement
  if (!(flags & UI7MenuFlags_NoMove)) {
    if (has_touch &&
        io->DragObject(name + "tmv",
                       vec4(Layout->Pos, vec2(Layout->Size.x(), tbh)))) {
      if (io->DragDoubleRelease) {
        is_open = !is_open;
      }
      Layout->Pos = Layout->Pos + (io->DragPosition - io->DragLastPosition);
    }
  }
}

void UI7::Menu::CollapseHandler() {
  // Collapse logic
  if (!(flags & UI7MenuFlags_NoCollapse)) {
    vec2 cpos = Layout->Pos + io->FramePadding;
    UI7Color clr = UI7Color_FrameBackground;
    if (has_touch &&
        io->DragObject(UI7::ID(name + "clbse"), vec4(cpos, vec2(18, tbh)))) {
      if (io->DragReleased) {
        is_open = !is_open;
      }
      clr = UI7Color_FrameBackgroundHovered;
    }
    vec2 positions[2] = {
        vec2(12, 6),
        vec2(0, 12),
    };
    if (is_open) {
      float t = positions[0].y();
      positions[0].y() = positions[1].x();
      positions[1].x() = t;
    }
    Layout->GetDrawList()->AddTriangle(
        cpos, cpos + positions[0], cpos + positions[1], io->Theme->Get(clr));
  }
}

void UI7::Menu::PostScrollHandler() {
  if (has_touch && io->DragObject(id, vec4(Layout->Pos, Layout->Size)) &&
      Layout->Scrolling[1] && flags & UI7MenuFlags_VtScrolling &&
      Layout->MaxPosition.y() - Layout->Size.y() + io->MenuPadding[1] > 0) {
    if (io->DragReleased) {
      // scroll_mod = (io->DragPosition - io->DragLastPosition);
    } else {
      Layout->ScrollOffset[1] = std::clamp(
          Layout->ScrollOffset[1] -
              (io->DragPosition.y() - io->DragLastPosition.y()),
          -40.f, (Layout->MaxPosition.y() - Layout->Size.y()) + 40.f);
    }
  }
}
}  // namespace UI7
}  // namespace PD