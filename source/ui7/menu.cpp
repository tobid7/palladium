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
  Container::Ref r = ObjectPush(PD::New<UI7::Label>(label, io->Ren));
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
  CursorMove(r->GetSize());
  r->Init(io, main);
  r->HandleScrolling(scrolling_off, view_area);
}

bool UI7::Menu::Button(const std::string& label) {
  bool ret = false;
  u32 id = Strings::FastHash("btn" + label + std::to_string(count_btn++));
  Container::Ref r = FindIDObj(id);
  if (!r) {
    r = PD::New<UI7::Button>(label, io);
    r->SetID(id);
    r->Init(io, main);
  }
  ObjectPush(r);
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
  r->Update();
  CursorMove(r->GetSize());
  r->HandleScrolling(scrolling_off, view_area);
  if (!r->Skippable()) {
    ret = std::static_pointer_cast<UI7::Button>(r)->IsPressed();
  }
  return ret;
}

void UI7::Menu::Checkbox(const std::string& label, bool& v) {
  u32 id = Strings::FastHash("cbx" + label + std::to_string(count_cbx++));
  Container::Ref r = FindIDObj(id);
  if (!r) {
    r = PD::New<UI7::Checkbox>(label, v, io);
    r->SetID(id);
    r->Init(io, main);
  }
  ObjectPush(r);
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
  r->Update();
  CursorMove(r->GetSize());
  r->HandleScrolling(scrolling_off, view_area);
}

void UI7::Menu::Image(Texture::Ref img, vec2 size) {
  Container::Ref r = ObjectPush(PD::New<UI7::Image>(img, size));
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
  CursorMove(r->GetSize());
  r->Init(io, main);
  r->HandleScrolling(scrolling_off, view_area);
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
  t->Label(std::format("Max Size: {:.2f}, {:.2f}", m->max.x(), m->max.y()));
  t->Label(std::format("Pos: {:.2f}, {:.2f} Size: {:.2f}, {:.2f}",
                       m->view_area.x(), m->view_area.y(), m->view_area.z(),
                       m->view_area.w()));
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
    scrolling_off = scroll_anim;
  }
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    main->PushClipRect(vec4(view_area.x() + io->MenuPadding[0],
                            view_area.y() + tbh,
                            view_area.x() + view_area.z() - io->MenuPadding[0],
                            view_area.y() + view_area.w()));
  }
  std::vector<int> tbr;
  for (int i = 0; i < (int)objects.size(); i++) {
    auto& it = objects[i];
    if (it->GetID() != 0 && !FindIDObj(it->GetID())) {
      idobjs.push_back(it);
    }
    if (!it->Skippable()) {
      if (scroll_mod[1] == 0.f) {
        it->HandleInput(io->Inp);
      }
      /// Unlock Input after to ensure nothing is checked twice
      it->UnlockInput();
      it->Draw();
    }
  }
  for (int i = 0; i < (int)idobjs.size(); i++) {
    if (idobjs[i]->Removable()) {
      tbr.push_back(i);
    }
  }
  for (auto it : tbr) {
    idobjs.erase(idobjs.begin() + it);
  }
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    main->PopClipRect();
  }
  this->objects.clear();
}

void UI7::Menu::CursorMove(const vec2& size) {
  last_size = size;
  slcursor = cursor + vec2(size[0] + io->ItemSpace[0], 0);
  if (bslpos[1]) {
    cursor = vec2(io->MenuPadding[0], cursor[1] + bslpos[1] + io->ItemSpace[1]);
    bslpos = vec2();
  } else {
    cursor = vec2(io->MenuPadding[0] + icursoroff[0],
                  cursor[1] + size[1] + io->ItemSpace[1]);
  }
  max = vec2(slcursor[0], cursor[1]);
}

void UI7::Menu::PreHandler(UI7MenuFlags flags) {
  TT::Scope st("MPRE_" + name);
  TT::Beg("MUSR_" + name);
  count_btn = 0;
  count_cbx = 0;
  tbh = 0.f;
  CursorInit();
  main_area = view_area;
  this->flags = flags;
  this->scrolling[0] = flags & UI7MenuFlags_HzScrolling;
  this->scrolling[1] = flags & UI7MenuFlags_VtScrolling;
  has_touch = main->ren->CurrentScreen()->ScreenType() == Screen::Bottom;
  if (!(flags & UI7MenuFlags_NoBackground) && is_open) {
    back->AddRectangle(view_area.xy() + vec2(0, tbh),
                       view_area.zw() - vec2(0, tbh),
                       io->Theme->Get(UI7Color_Background));
  }
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    // Title bar setup and Rendering
    tbh = front->ren->TextScale() * 30.f;
    front->AddRectangle(view_area.xy(), vec2(view_area.z(), tbh),
                        io->Theme->Get(UI7Color_Header));
    vec2 tpos(io->MenuPadding[0],
              tbh * 0.5 - front->ren->GetTextDimensions(name).y() * 0.5);
    if (!(flags & UI7MenuFlags_NoCollapse)) {
      tpos.x() += 18;
      vec2 cpos = view_area.xy() + io->FramePadding;
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
      this->front->AddTriangle(cpos, cpos + positions[0], cpos + positions[1],
                               io->Theme->Get(clr));
    }
    LITextFlags tflags = LITextFlags_None;
    if (flags & UI7MenuFlags_CenterTitle) {
      tpos = 0;
      tflags = LITextFlags_AlignMid;
    }
    front->Layer(front->Layer() + 1);
    front->AddText(view_area.xy() + tpos, this->name,
                   io->Theme->Get(UI7Color_Text), tflags,
                   vec2(view_area.z(), tbh));
    main_area[1] = tbh;
    CursorInit();
    // Menu Movement
    if (!(flags & UI7MenuFlags_NoMove)) {
      if (has_touch &&
          io->DragObject(name + "tmv", vec4(view_area.xy() + vec2(18, 0),
                                            vec2(view_area.z(), tbh)))) {
        view_area =
            vec4(view_area.xy() + (io->DragPosition - io->DragLastPosition),
                 view_area.zw());
      }
    }
  }
  // Add a clip Rect for Separators
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    main->PushClipRect(vec4(view_area.x() + io->MenuPadding[0],
                            view_area.y() + tbh,
                            view_area.x() + view_area.z() - io->MenuPadding[0],
                            view_area.y() + view_area.w()));
  }
}

void UI7::Menu::PostHandler() {
  TT::Scope st("MPOS_" + name);
  if (!(flags & UI7MenuFlags_NoResize)) {
    if (has_touch &&
        io->DragObject(name + "rszs",
                       vec4(view_area.xy() + view_area.zw() - 20, 20))) {
      view_area = vec4(view_area.xy(), view_area.zw() + (io->DragPosition -
                                                         io->DragLastPosition));
    }
    // front->AddRectangle(view_area.xy() + view_area.zw() - 20, 20,
    // 0xffffffff); Not vidible dor some reason
    int l = front->Layer();
    front->Layer(l + 1);
    front->AddTriangle(10, vec2(10, 0), vec2(10, 0), 0xffffffff);
    front->Layer(l);
  }
  if (scrolling[1]) {
    scroll_allowed[1] = (max[1] > view_area.w() - io->MenuPadding[1]);
    if (max[1] < view_area.w() - io->MenuPadding[1]) {
      scrolling_off[1] = 0.f;
    }
    scrollbar[1] = scroll_allowed[1];

    if (scrollbar[1]) {
      /// Setup Some Variables hare [they are self described]
      int screen_w = view_area.z();
      int tsp = io->MenuPadding[1] + tbh;
      int slider_w = 4;
      int szs = view_area.w() - tsp - io->MenuPadding[1];
      /// Actually dont have a Horizontal bar yet
      if (scrollbar[0]) szs -= slider_w - 2;
      int lslider_h = 20;  // Dont go less heigt for the drag
      float slider_h = (szs - 4) * (float(szs - 4) / max[1]);
      /// Visual Slider Height (How it looks in the end)
      int vslider_h = std::clamp(slider_h, float(lslider_h), float(szs - 4));

      /// Check if we overscroll to the bottom and Auto scroll back...
      /// Probably schould use Tween ENgine here
      if (scrolling_off[1] > max[1] - view_area[3] && max[1] != 0.f &&
          max[1] >= view_area[3] - io->MenuPadding[1]) {
        scrolling_off[1] -= 3.f;
        if (scrolling_off[1] < max[1] - view_area[3]) {
          scrolling_off[1] = max[1] - view_area[3];
        }
      }

      /// Do the Same as above just for Overscroll back to the top
      if (scrolling_off[1] < 0) {
        scrolling_off[1] += 3.f;
        if (scrolling_off[1] > 0) {
          scrolling_off[1] = 0;
        }
      }

      /// Dont overscroll to much
      if (scrolling_off[1] < -40 ||
          scrolling_off[1] > max[1] - view_area[3] + 40) {
        scroll_mod[1] = 0.f;
      }

      /// The pain :(
      if (has_touch && !io->IsObjectDragged()) {
        vec2 tpos = io->Inp->TouchPos();
        if (io->Inp->IsDown(io->Inp->Touch)) {
          mouse = tpos;
        } else if (io->Inp->IsUp(io->Inp->Touch)) {
          mouse = vec2();
        }
        if (io->Inp->IsHeld(io->Inp->Touch)) {
          if (front->ren->InBox(tpos, vec4(view_area.xy() + main_area.xy(),
                                           main_area.zw()))) {
            if (scrolling_off[1] < max[1] - view_area[3] + 40 &&
                scrolling_off[1] > -40) {
              /// Cursor Mod
              float cm = mouse[1] - tpos[1];
              if (scroll_mod[1] <= 4.f && scroll_mod[1] >= -4.f && cm != 0) {
                scroll_mod[1] = cm;
              }
            }
            mouse = tpos;
          }
        }
      }

      /// Effect
      if (scroll_mod[1] != 0) {
        scrolling_off[1] += scroll_mod[1];
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
      }
      UI7Color sldr_drag = UI7Color_Button;
      /// Slider Dragging????
      /// Probably need a new API for this
      if (has_touch &&
          io->DragObject(name + "sldr", vec4(view_area.x() + screen_w - 12,
                                             view_area.y() + tsp, 8, szs)) &&
          !io->DragReleased) {
        sldr_drag = UI7Color_ButtonHovered;
        float drag_center = vslider_h / 2.0f;
        float drag_pos = std::clamp(
            static_cast<float>((io->DragPosition[1] - tsp - drag_center) /
                               (szs - vslider_h - 4)),
            0.0f, 1.0f);

        scrolling_off[1] = drag_pos * (max[1] - 240.f);
      }
      int srpos =
          tsp + std::clamp(float(szs - vslider_h - 4) *
                               (scrolling_off[1] / (max[1] - view_area[3])),
                           0.f, float(szs - vslider_h - 4));

      /// Rendering Stage
      front->AddRectangle(view_area.xy() + vec2(screen_w - 12, tsp),
                          vec2(slider_w * 2, szs),
                          io->Theme->Get(UI7Color_FrameBackground));
      front->AddRectangle(view_area.xy() + vec2(screen_w - 10, tsp + 2),
                          vec2(slider_w, szs - 4),
                          io->Theme->Get(UI7Color_FrameBackgroundHovered));
      front->AddRectangle(view_area.xy() + vec2(screen_w - 10, srpos + 2),
                          vec2(slider_w, vslider_h), io->Theme->Get(sldr_drag));
    }
  }
  // Remove the Clip Rect
  if (!(flags & UI7MenuFlags_NoClipRect)) {
    main->PopClipRect();
  }
  TT::End("MUSR_" + name);
}

void UI7::Menu::SameLine() {
  bslpos = last_size;
  cursor = slcursor;
}

void UI7::Menu::Separator() {
  vec2 pos = Cursor();
  vec2 size = vec2(view_area.z() - (scrollbar[1] ? 24 : 10), 1);
  CursorMove(size);
  if (HandleScrolling(pos, size)) {
    return;
  }
  main->AddRectangle(pos, size, io->Theme->Get(UI7Color_TextDead));
}

void UI7::Menu::SeparatorText(const std::string& label) {
  vec2 size = vec2(view_area.z() - (scrollbar[1] ? 24 : 10), 1);
  vec2 tdim = this->back->ren->GetTextDimensions(label);
  vec2 pos = Cursor();
  CursorMove(vec2(size.x(), tdim.y()));

  if (HandleScrolling(pos, size)) {
    return;
  }
  auto alignment = GetAlignment();
  vec2 rpos = AlignPos(pos, view_area.z() - (io->MenuPadding[0] * 2), view_area,
                       alignment);  // RenderPos
  /// Label pos for better overview
  vec2 lpos = rpos;
  if (alignment & UI7Align_Center) {
    lpos += vec2(
        (view_area.z() - (io->MenuPadding[0] * 2)) * 0.5 - tdim.x() * 0.5, 0);
  } else if (alignment & UI7Align_Right) {
    lpos = vec2(view_area.z() - (io->MenuPadding[0] * 2) - tdim.x(), rpos.y());
    if (scrolling[1]) {
      lpos.x() -= 8;
    }
  }
  if (!(alignment & UI7Align_Left)) {
    main->AddRectangle(rpos + vec2(0, tdim.y() * 0.5),
                       vec2(lpos.x() - rpos.x() - io->MenuPadding[0], size.y()),
                       io->Theme->Get(UI7Color_TextDead));
  }
  if (!(alignment & UI7Align_Right)) {
    main->AddRectangle(rpos + vec2(lpos.x() + tdim.x(), tdim.y() * 0.5),
                       vec2(size.x() - (lpos.x() + tdim.x()), size.y()),
                       io->Theme->Get(UI7Color_TextDead));
  }
  main->AddText(lpos, label, io->Theme->Get(UI7Color_Text), 0,
                vec2(view_area.z(), 20));
}

bool UI7::Menu::HandleScrolling(vec2& pos, const vec2& size) {
  if (scrolling[1]) {
    pos -= vec2(0, scrolling_off.y());
    if (!io->Ren->InBox(
            pos, size, vec4(view_area.xy(), view_area.xy() + view_area.zw()))) {
      return true;
    }
  }
  return false;
}

Container::Ref UI7::Menu::ObjectPush(Container::Ref obj) {
  this->objects.push_back(obj);
  obj->SetParent(this->tmp_parent);
  return obj;
}

Container::Ref UI7::Menu::FindIDObj(u32 id) {
  for (auto& it : idobjs) {
    if (it->GetID() == id) {
      return it;
    }
  }
  return nullptr;
}

void UI7::Menu::Join() {
  Assert(objects.size(), "Objects list is empty!");
  join.push_back(objects.back().get());
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
    off[0] = (view_area[0] + view_area[2] * 0.5) - (spos[0] + szs[0] * 0.5);
  }
  if (a & UI7Align_Mid) {
    off[1] = (view_area[1] + view_area[3] * 0.5) - (spos[1] + szs[1] * 0.5);
  }
  for (auto it : join) {
    it->SetPos(it->GetPos() + off);
  }
  join.clear();
}

vec2 UI7::Menu::AlignPos(vec2 pos, vec2 size, vec4 view, UI7Align a) {
  vec2 np = pos;
  if (a & UI7Align_Center) {
    np[0] = (view[0] + view[2] * 0.5) - (pos[0] + size[0] * 0.5);
  }
  if (a & UI7Align_Mid) {
    np[1] = (view[1] + view[3] * 0.5) - (pos[1] + size[1] * 0.5);
  }
  return np;
}

void UI7::Menu::AfterAlign(UI7Align a) {
  Container* ref = objects.back().get();
  vec2 p = ref->GetPos();
  vec2 s = ref->GetSize();
  vec2 np = p;
  if (a & UI7Align_Center) {
    np[0] = (view_area[0] + view_area[2] * 0.5) - (p[0] + s[0] * 0.5);
  }
  if (a & UI7Align_Mid) {
    np[1] = (view_area[1] + view_area[3] * 0.5) - (p[1] + s[1] * 0.5);
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
  vec2 pos = Cursor();
  vec2 tdim = io->Ren->GetTextDimensions(id.GetName());
  vec2 size = vec2(tdim.x() + 10 + io->ItemSpace[0], tdim.y());
  if (n->second) {
    icursoroff.x() += 10.f;
  }
  CursorMove(size);
  if (HandleScrolling(pos, size)) {
    return n->second;
  }
  vec2 ts = pos + vec2(0, 3);
  vec2 positions[2] = {
      vec2(10, 5),
      vec2(0, 10),
  };
  if (n->second) {
    float t = positions[0].y();
    positions[0].y() = positions[1].x();
    positions[1].x() = t;
  }
  main->AddTriangle(ts, ts + positions[0], ts + positions[1],
                    io->Theme->Get(UI7Color_FrameBackground));
  main->AddText(pos + vec2(10 + io->ItemSpace[0], 0), id.GetName(),
                io->Theme->Get(UI7Color_Text));
  if (has_touch && io->DragObject(name + id.GetName(), vec4(pos, size))) {
    if (io->DragReleased) {
      n->second = !n->second;
      if (!n->second) {
        icursoroff.x() -= 10.f;
        cursor.x() -= 10;
      }
    }
  }
  return n->second;
}
void UI7::Menu::EndTreeNode() {
  icursoroff.x() -= 10.f;
  cursor.x() -= 10;
  if (icursoroff.x() < 0.f) {
    icursoroff.x() = 0.f;
  }
}
}  // namespace UI7
}  // namespace PD