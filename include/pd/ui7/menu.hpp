#pragma once

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

#include <pd/controls/hid.hpp>
#include <pd/maths/tween.hpp>
#include <pd/ui7/containers.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>

namespace PD {
namespace UI7 {
class Menu : public SmartCtor<Menu> {
 public:
  Menu(ID id, Theme* tl, Hid::Ref h) {
    linked_theme = tl;
    this->inp = h;
    this->id = id;
    this->name = id.GetName();
    scrolling[0] = false;
    scrolling[1] = false;
    scrollbar[0] = false;
    scrollbar[1] = false;
    scroll_allowed[0] = false;
    scroll_allowed[1] = false;
  };
  ~Menu() {}

  /// Objects
  void Label(const std::string& label);
  bool Button(const std::string& label);
  void Checkbox(const std::string& label, bool& v);
  void Image(Texture::Ref img, vec2 size = 0.f);

  /// Basic API
  void SameLine();
  void Separator();
  void SeparatorText(const std::string& label);
  void Join();
  /// @brief Horizontal Center Joined objects
  void JoinOpHzCenter();
  void AfterAlignCenter();

  /// API for Custom Objects
  bool HandleScrolling(vec2& pos, const vec2& size);
  vec2 Cursor() const { return cursor; }
  void Cursor(const vec2& v) {
    bcursor = cursor;
    cursor = v;
  }
  void RestoreCursor() {
    cursor = bcursor;
    bcursor = vec2();
  }
  bool HasVerticalScrollbar() { return scrollbar[1]; }
  bool HasHorizontalScrollbar() { return scrollbar[0]; }
  float TitleBarHeight() { return tbh; }
  /// @brief Set a Custom Titlebar heigt
  /// @note Could destroy some basic functionality
  void TitleBarHeight(float v) { tbh = v; }
  /// @brief Init the Cursor
  /// @note Useful when using with a Custom TitlebarHeight
  void CursorInit() { Cursor(vec2(5, tbh + 5)); }
  void CursorMove(const vec2& szs);

  vec4 ViewArea() const { return view_area; }
  vec4 MainArea() const { return main_area; }
  void MainArea(const vec4& v) { main_area = v; }
  vec2 ScrollOffset() const { return scrolling_off; }
  void ScrollTo(vec2 pos) {
    scroll_anim.From(scrolling_off)
        .To(pos)
        .In(1.f)
        .As(scroll_anim.EaseInOutSine);
  }

  /// Objects API
  Container::Ref ObjectPush(Container::Ref obj);
  Container::Ref FindIDObj(u32 id);

  /// Draw Lists
  DrawList::Ref BackList() { return back; }
  void BackList(DrawList::Ref v) { back = v; }
  DrawList::Ref MainList() { return main; }
  void MainList(DrawList::Ref v) { main = v; }
  DrawList::Ref FrontList() { return front; }
  void FrontList(DrawList::Ref v) { front = v; }

  /// Advanced
  void DebugLabels();

  /// Uneditable Stuff
  std::string GetName() const { return name; }
  u32 GetID() const { return id; }

 private:
  /// Advanced Handlers
  void PreHandler(UI7MenuFlags flags);
  void PostHandler();
  /// Basic Settings
  void BackupCursor(const vec2& v) { bcursor = v; }
  vec2 SameLineCursor() const { return slcursor; }
  void SameLineCursor(const vec2& v) { slcursor = v; }
  void ViewArea(const vec4& v) { view_area = v; }
  void ScrollOffset(const vec2& v) { scrolling_off = v; }
  vec2 ScrollMod() const { return scroll_mod; }
  void ScrollMod(const vec2& v) { scroll_mod = v; }

  /// Internal Processing
  void Update(float delta);

  /// This ability is crazy useful
  friend class Context;

  /// Data
  UI7MenuFlags flags = 0;
  u32 id;
  std::string name;
  vec2 cursor;
  vec2 bcursor;
  vec2 slcursor;
  vec4 view_area;
  vec4 main_area;
  vec2 scrolling_off;
  bool scrolling[2];
  vec2 scroll_mod;
  float tbh;
  bool scrollbar[2];
  bool scroll_allowed[2];
  bool has_touch;

  Menu::Ref submenu;

  /// Objects API
  std::vector<Container::Ref> objects;
  std::vector<Container::Ref> idobjs;
  std::vector<Container*> join;
  int count_btn = 0;
  int count_cbx = 0;

  // DrawLists
  DrawList::Ref back;
  DrawList::Ref main;
  DrawList::Ref front;

  vec2 max;
  vec2 mouse;
  vec2 bslpos;
  vec2 last_size;

  // Theme
  Theme* linked_theme;

  // Input Reference
  Hid::Ref inp;

  // Animations System
  Tween<vec2> scroll_anim;
};
}  // namespace UI7
}  // namespace PD