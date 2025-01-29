#include <pd/common/sys.hpp>
#include <pd/common/timetrace.hpp>
#include <pd/ui7/menu.hpp>

//////////////////////////////
//////// OBJECT SETUP ////////
////// Setup Variables ///////
///////// Move Cursor ////////
//// Check Scrolling State ///
/////// Handle Controls //////
////////// Render ////////////
//////////////////////////////

namespace PD {
namespace UI7 {
void UI7::Menu::Label(const std::string& label) {
  vec2 size = this->back->GetRenderer()->GetTextDimensions(label);
  vec2 pos = Cursor();
  CursorMove(size - vec2(0, 4));  // Fix to make gap not to large

  if (HandleScrolling(pos, size)) {
    return;
  }

  /// To Draw a Label above the head bar you should
  /// use the m->GetFrontList() instead

  main->AddText(pos, label, linked_theme->Get(UI7Color_Text), 0,
                vec2(view_area.z(), 20));
}

bool UI7::Menu::Button(const std::string& label) {
  bool ret = false;
  auto tszs = this->back->GetRenderer()->GetTextDimensions(label);
  vec2 size = tszs + vec2(8, 4);
  vec2 pos = Cursor();
  UI7Color clr = UI7Color_Button;
  CursorMove(size);
  /////// SCROLLING HANDLER HERE ////////
  if (HandleScrolling(pos, size)) {
    return false;
  }
  /// CONTROLS ///
  if (has_touch) {
    if (inp->IsHeld(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPos(), vec4(pos, size))) {
      clr = UI7Color_ButtonHovered;
    }
    if (inp->IsUp(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPosLast(), vec4(pos, size))) {
      clr = UI7Color_ButtonActive;
      ret = true;
    }
  }
  /// Rendering ///
  main->AddRectangle(pos, size, linked_theme->Get(clr));
  main->AddText(pos + size * 0.5 - tszs * 0.5, label,
                linked_theme->Get(UI7Color_Text));
  return ret;
}

void UI7::Menu::Checkbox(const std::string& label, bool& v) {
  vec2 pos = Cursor();
  vec2 tdim = front->ren->GetTextDimensions(label);
  vec2 cbs(18);
  vec2 size = cbs + vec2(tdim.x() + 5, 0);
  CursorMove(size);

  if (HandleScrolling(pos, size)) {
    return;
  }

  UI7Color cbbg = UI7Color_FrameBackground;

  if (has_touch) {
    if (inp->IsHeld(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPos(), vec4(pos, size))) {
      cbbg = UI7Color_FrameBackgroundHovered;
    }
    if (inp->IsUp(inp->Touch) &&
        LI::Renderer::InBox(inp->TouchPosLast(), vec4(pos, size))) {
      cbbg = UI7Color_FrameBackgroundHovered;
      v = !v;
    }
  }

  main->AddRectangle(pos, cbs, linked_theme->Get(cbbg));
  if (v) {
    main->AddRectangle(pos + 2, cbs - 4, linked_theme->Get(UI7Color_Checkmark));
  }
  main->AddText(pos + vec2(cbs.x() + 5, cbs.y() * 0.5 - tdim.y() * 0.5), label,
                linked_theme->Get(UI7Color_Text));
}

void UI7::Menu::Image(Texture::Ref img, vec2 size) {
  /// Variable Setup Stage ///
  size = size == 0.f ? img->GetSize() : size;
  vec2 pos = Cursor();
  CursorMove(size);
  /// Scrolling Handler ///
  if (HandleScrolling(pos, size)) {
    return;
  }
  /// Rendering Stage ///
  main->AddImage(pos, img, size);
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
  this->back->BaseLayer(30);
  this->back->Process();
  this->main->BaseLayer(40);
  this->main->Process();
  this->front->BaseLayer(50);
  this->front->Process();
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
  Cursor(vec2(5, 5));
  this->flags = flags;
  this->scrolling[0] = flags & UI7MenuFlags_HzScrolling;
  this->scrolling[1] = flags & UI7MenuFlags_VtScrolling;
  has_touch = main->GetRenderer()->CurrentScreen() == Screen::Bottom;
  if (!(flags & UI7MenuFlags_NoBackground)) {
    back->AddRectangle(0, view_area.zw(), UI7Color_Background);
  }
  if (!(flags & UI7MenuFlags_NoTitlebar)) {
    tbh = front->GetRenderer()->TextScale() * 30.f;
    front->AddRectangle(0, vec2(view_area.z(), tbh),
                        linked_theme->Get(UI7Color_Header));
    vec2 tpos(5, tbh * 0.5 - front->ren->GetTextDimensions(name).y() * 0.5);
    LITextFlags tflags = LITextFlags_None;
    if (flags & UI7MenuFlags_CenterTitle) {
      tpos = 0;
      tflags = LITextFlags_AlignMid;
    }
    front->AddText(tpos, this->name, linked_theme->Get(UI7Color_Text), tflags,
                   vec2(view_area.z(), tbh));
    Cursor(vec2(5, tbh + 5));
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
          if (!front->ren->InBox(tpos, vec4(view_area[2] - 13, tbh + 5, 8,
                                            view_area[3] - tbh - 10))) {
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

      int srpos =
          tsp + std::clamp(float(szs - vslider_h - 4) *
                               (scrolling_off[1] / (max[1] - view_area[3])),
                           0.f, float(szs - vslider_h - 4));

      /// Rendering Stage
      front->AddRectangle(vec2(screen_w - 12, tsp), vec2(slider_w * 2, szs),
                          linked_theme->Get(UI7Color_FrameBackground));
      front->AddRectangle(vec2(screen_w - 10, tsp + 2), vec2(slider_w, szs - 4),
                          linked_theme->Get(UI7Color_FrameBackgroundHovered));
      front->AddRectangle(vec2(screen_w - 10, srpos + 2),
                          vec2(slider_w, vslider_h),
                          linked_theme->Get(UI7Color_Button));
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
  main->AddRectangle(pos, size, linked_theme->Get(UI7Color_TextDead));
}

void UI7::Menu::SeparatorText(const std::string& label) {
  vec2 size = vec2(view_area.z() - (scrollbar[1] ? 24 : 10), 1);
  vec2 tdim = this->back->GetRenderer()->GetTextDimensions(label);
  vec2 pos = Cursor();
  CursorMove(vec2(size.x(), tdim.y() - 4));  // Fix to make gap not to large

  if (HandleScrolling(pos, size)) {
    return;
  }
  /// Label pos for better overview
  vec2 lpos = pos + vec2((view_area.z() - 10) * 0.5 - tdim.x() * 0.5, 0);
  main->AddRectangle(pos + vec2(0, tdim.y() * 0.5),
                     vec2(lpos.x() - pos.x() - 5, size.y()),
                     linked_theme->Get(UI7Color_TextDead));
  main->AddRectangle(pos + vec2(lpos.x() + tdim.x(), tdim.y() * 0.5),
                     vec2(size.x() - (lpos.x() + tdim.x()), size.y()),
                     linked_theme->Get(UI7Color_TextDead));
  main->AddText(lpos, label, linked_theme->Get(UI7Color_Text), 0,
                vec2(view_area.z(), 20));
}

bool UI7::Menu::HandleScrolling(vec2& pos, const vec2& size) {
  if (scrolling[1]) {
    vec2 p = pos;
    pos -= vec2(0, scrolling_off.y());
    if (pos.y() > view_area.w() ||
        (pos.y() + size.y() < tbh - 5 && p.y() > tbh)) {
      return true;
    }
  }
  return false;
}
}  // namespace UI7
}  // namespace PD