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
  Container::Ref r =
      ObjectPush(PD::New<UI7::Label>(label, Cursor(), this->back->ren));
  r->SetPos(AlignPos(r->GetPos(), r->GetSize(), view_area, GetAlignment()));
  CursorMove(r->GetSize());
  r->Init(main->ren, main, theme);
  r->HandleScrolling(scrolling_off, view_area);
}

bool UI7::Menu::Button(const std::string& label) {
  bool ret = false;
  u32 id = Strings::FastHash("btn" + label + std::to_string(count_btn++));
  Container::Ref r = FindIDObj(id);
  if (!r) {
    r = PD::New<UI7::Button>(label, Cursor(), this->back->ren);
    r->SetID(id);
    r->Init(main->ren, main, theme);
  }
  ObjectPush(r);
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
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
    r = PD::New<UI7::Checkbox>(label, Cursor(), v, this->back->ren);
    r->SetID(id);
    r->Init(main->ren, main, theme);
  }
  ObjectPush(r);
  r->SetPos(AlignPos(Cursor(), r->GetSize(), view_area, GetAlignment()));
  CursorMove(r->GetSize());
  r->HandleScrolling(scrolling_off, view_area);
}

void UI7::Menu::Image(Texture::Ref img, vec2 size) {
  Container::Ref r =
      ObjectPush(PD::New<UI7::Image>(img, Cursor(), this->back->ren, size));
  r->SetPos(AlignPos(r->GetPos(), r->GetSize(), view_area, GetAlignment()));
  CursorMove(r->GetSize());
  r->Init(main->ren, main, theme);
  r->HandleScrolling(scrolling_off, view_area);
}

void UI7::Menu::DebugLabels() {
  std::stringstream s;
  s << "Name: " << name << " [";
  s << std::hex << std::setw(8) << std::setfill('0') << id;
  s << std::dec << "]";
  this->Label(s.str());
  this->Label(
      "Pre: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MPRE_" + name)->GetProtocol()->GetAverage()));
  this->Label(
      "Post: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MPOS_" + name)->GetProtocol()->GetAverage()));
  this->Label(
      "Update: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MUPT_" + name)->GetProtocol()->GetAverage()));
  this->Label(
      "MUser: " +
      Strings::FormatNanos(
          Sys::GetTraceRef("MUSR_" + name)->GetProtocol()->GetAverage()));
}

void UI7::Menu::Update(float delta) {
  TT::Scope st("MUPT_" + name);
  scroll_anim.Update(delta);
  if (!scroll_anim.IsFinished()) {
    scrolling_off = scroll_anim;
  }
  main->PushClipRect(vec4(5, tbh, view_area.z() - 12, view_area.w()));
  std::vector<int> tbr;
  for (int i = 0; i < (int)objects.size(); i++) {
    auto& it = objects[i];
    if (it->GetID() != 0 && !FindIDObj(it->GetID())) {
      idobjs.push_back(it);
    }
    if (!it->Skippable()) {
      if (scroll_mod[1] == 0.f) {
        it->HandleInput(inp);
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
  main->PopClipRect();
  this->back->Process();
  this->main->Process();
  this->front->Process();
  this->objects.clear();
}

void UI7::Menu::CursorMove(const vec2& size) {
  last_size = size;
  slcursor = cursor + vec2(size[0] + 5, 0);
  if (bslpos[1]) {
    cursor = vec2(5, cursor[1] + bslpos[1] + 5);
    bslpos = vec2();
  } else {
    cursor = vec2(5, cursor[1] + size[1] + 5);
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
  if (!(flags & UI7MenuFlags_NoBackground)) {
    back->AddRectangle(0, view_area.zw(), theme->Get(UI7Color_Background));
  }
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    tbh = front->ren->TextScale() * 30.f;
    front->AddRectangle(0, vec2(view_area.z(), tbh),
                        theme->Get(UI7Color_Header));
    vec2 tpos(5, tbh * 0.5 - front->ren->GetTextDimensions(name).y() * 0.5);
    LITextFlags tflags = LITextFlags_None;
    if (flags & UI7MenuFlags_CenterTitle) {
      tpos = 0;
      tflags = LITextFlags_AlignMid;
    }
    front->AddText(tpos, this->name, theme->Get(UI7Color_Text), tflags,
                   vec2(view_area.z(), tbh));
    main_area[1] = tbh;
    CursorInit();
  }
}

void UI7::Menu::PostHandler() {
  TT::Scope st("MPOS_" + name);
  if (scrolling[1]) {
    scroll_allowed[1] = (max[1] > 235);
    scrollbar[1] = scroll_allowed[1];

    if (scrollbar[1]) {
      /// Setup Some Variables hare [they are self described]
      int screen_w = view_area.z();
      int tsp = 5 + tbh;
      int slider_w = 4;
      int szs = view_area.w() - tsp - 5;
      /// Actually dont have a Horizontal bar yet
      if (scrollbar[0]) szs -= slider_w - 2;
      int lslider_h = 20;  // Dont go less heigt for the drag
      float slider_h = (szs - 4) * (float(szs - 4) / max[1]);
      /// Visual Slider Height (How it looks in the end)
      int vslider_h = std::clamp(slider_h, float(lslider_h), float(szs - 4));

      /// Check if we overscroll to the bottom and Auto scroll back...
      /// Probably schould use Tween ENgine here
      if (scrolling_off[1] > max[1] - view_area[3] && max[1] != 0.f &&
          max[1] >= view_area[3] - 5) {
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
      if (has_touch) {
        vec2 tpos = inp->TouchPos();
        if (inp->IsDown(inp->Touch)) {
          mouse = tpos;
        } else if (inp->IsUp(inp->Touch)) {
          mouse = vec2();
        }
        if (inp->IsHeld(inp->Touch)) {
          if (front->ren->InBox(tpos, main_area)) {
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

      /// Slider Dragging????
      /// Probably need a new API for this
      auto tp = inp->TouchPos();
      if (inp->IsHeld(inp->Touch) &&
          LI::Renderer::InBox(tp, vec4(screen_w - 12, tsp, 8, szs))) {
        float drag_center = vslider_h / 2.0f;
        float drag_pos =
            std::clamp(static_cast<float>((tp[1] - tsp - drag_center) /
                                          (szs - vslider_h - 4)),
                       0.0f, 1.0f);

        scrolling_off[1] = drag_pos * (max[1] - 240.f);
      }

      int srpos =
          tsp + std::clamp(float(szs - vslider_h - 4) *
                               (scrolling_off[1] / (max[1] - view_area[3])),
                           0.f, float(szs - vslider_h - 4));

      /// Rendering Stage
      front->AddRectangle(vec2(screen_w - 12, tsp), vec2(slider_w * 2, szs),
                          theme->Get(UI7Color_FrameBackground));
      front->AddRectangle(vec2(screen_w - 10, tsp + 2), vec2(slider_w, szs - 4),
                          theme->Get(UI7Color_FrameBackgroundHovered));
      front->AddRectangle(vec2(screen_w - 10, srpos + 2),
                          vec2(slider_w, vslider_h),
                          theme->Get(UI7Color_Button));
    }
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
  main->AddRectangle(pos, size, theme->Get(UI7Color_TextDead));
}

void UI7::Menu::SeparatorText(const std::string& label) {
  vec2 size = vec2(view_area.z() - (scrollbar[1] ? 24 : 10), 1);
  vec2 tdim = this->back->ren->GetTextDimensions(label);
  vec2 pos = Cursor();
  CursorMove(vec2(size.x(), tdim.y() - 4));  // Fix to make gap not to large

  if (HandleScrolling(pos, size)) {
    return;
  }
  auto alignment = GetAlignment();
  vec2 rpos =
      AlignPos(pos, view_area.z() - 10, view_area, alignment);  // RenderPos
  /// Label pos for better overview
  vec2 lpos = rpos;
  if (alignment & UI7Align_Center) {
    lpos += vec2((view_area.z() - 10) * 0.5 - tdim.x() * 0.5, 0);
  } else if (alignment & UI7Align_Right) {
    lpos = vec2(view_area.z() - 10 - tdim.x(), rpos.y());
    if (scrolling[1]) {
      lpos.x() -= 8;
    }
  }
  if (!(alignment & UI7Align_Left)) {
    main->AddRectangle(pos + vec2(0, tdim.y() * 0.5),
                       vec2(lpos.x() - pos.x() - 5, size.y()),
                       theme->Get(UI7Color_TextDead));
  }
  if (!(alignment & UI7Align_Right)) {
    main->AddRectangle(pos + vec2(lpos.x() + tdim.x(), tdim.y() * 0.5),
                       vec2(size.x() - (lpos.x() + tdim.x()), size.y()),
                       theme->Get(UI7Color_TextDead));
  }
  main->AddText(lpos, label, theme->Get(UI7Color_Text), 0,
                vec2(view_area.z(), 20));
}

bool UI7::Menu::HandleScrolling(vec2& pos, const vec2& size) {
  if (scrolling[1]) {
    pos -= vec2(0, scrolling_off.y());
    if (pos.y() > view_area.w() || (pos.y() + size.y() < view_area.y())) {
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
}  // namespace UI7
}  // namespace PD