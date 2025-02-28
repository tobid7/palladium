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

#include <pd/drivers/hid.hpp>
#include <pd/core/tween.hpp>
#include <pd/ui7/containers.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>

namespace PD {
namespace UI7 {
class Menu : public SmartCtor<Menu> {
 public:
  Menu(ID id, Theme::Ref tl, Hid::Ref h) {
    theme = tl;
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

  /// Objects ///

  /// @brief Render a Simple Label
  /// @param label The text to draw
  void Label(const std::string& label);
  /// @brief Render a Button
  /// @param label The buttons text
  /// @return if the button was pressed
  bool Button(const std::string& label);
  /// @brief Render a Checkbox
  /// @param label Label of the Checkbox
  /// @param v A value to update
  void Checkbox(const std::string& label, bool& v);
  /// @brief Render an Image
  /// @param img Texture reference of the image
  /// @param size a Custom Size if needed
  void Image(Texture::Ref img, vec2 size = 0.f);

  /// Basic API ///

  /// @brief Add the Next object to the same line
  void SameLine();
  /// @brief Add a Separator Line
  void Separator();
  /// @brief Render a Separator Line with a Text
  /// @todo determinate text position by current alignment
  /// @param label The Text to show
  void SeparatorText(const std::string& label);
  /// @brief Put the last Added Object into the Joinlist
  void Join();
  /// @brief Add the Last element to the join list
  /// and perform an alignment operation
  /// @param a Alignment Oeration(s)
  void JoinAlign(UI7Align a);
  /// @brief Align the Last Object
  /// @param a Alignment Operation
  void AfterAlign(UI7Align a);
  /// @brief Set a Temp alignment op for the next Object
  /// @param a Alignment Operation
  void NextAlign(UI7Align a) { tmpalign = a; }
  /// @brief Align Every Single Object by this operationset
  /// @param a Alignment
  void PushAlignment(UI7Align a) { alignment = a; }
  /// @brief Use default alignment
  void PopAlignment() { alignment = UI7Align_Default; }
  /// @brief Get a New Position depending on the Alignment
  /// @param pos Current Position
  /// @param size Object size
  /// @param view Viewport [position and size]
  /// @param a Alignment Operations
  /// @return new position based on the alignment
  static vec2 AlignPos(vec2 pos, vec2 size, vec4 view, UI7Align a);
  /// @brief Returns a Reference to the theme
  /// @return Reference to the base Theme of the context
  Theme::Ref GetTheme() { return theme; }
  /// @brief Directly return a Color by using the
  /// m->ThemeColor(UI7Color_Text) for example
  /// @param clr The Input UI7 Color
  /// @return The 32bit color value
  u32 ThemeColor(UI7Color clr) const { return theme->Get(clr); }

  /// API for Custom Objects

  /// @brief Handles the Position of Objects in Scrolling Menu
  /// @note As Containers have their own FUnc to handle this, this
  /// function is only useful to Render Live Objects whicch cannot be aligned
  /// by the internal Alignment Api
  /// @param pos position reference to write the new position to
  /// @param size size of the Object
  /// @return if the object can be skipped in rendering
  bool HandleScrolling(vec2& pos, const vec2& size);
  /// @brief Get the Cursor Position
  /// @return Cursor Pos
  vec2 Cursor() const { return cursor; }
  /// @brief Set the Cursor position
  /// @note The old Position can be restored with RestoreCursor
  /// @param v New Position
  void Cursor(const vec2& v) {
    bcursor = cursor;
    cursor = v;
  }
  /// @brief Restore to the last cursor Position
  void RestoreCursor() {
    cursor = bcursor;
    bcursor = vec2();
  }
  /// @brief Return if a Vertical Scrollbar exists
  bool HasVerticalScrollbar() { return scrollbar[1]; }
  /// @brief Return if a Horizontal Scrollbar exists
  bool HasHorizontalScrollbar() { return scrollbar[0]; }
  /// @brief Get the Titlebar height
  float TitleBarHeight() { return tbh; }
  /// @brief Set a Custom Titlebar heigt
  /// @note Could destroy some basic functionality
  void TitleBarHeight(float v) { tbh = v; }
  /// @brief Init the Cursor
  /// @note Useful when using with a Custom TitlebarHeight
  void CursorInit() { Cursor(vec2(5, tbh + 5)); }
  /// @brief Move the Cursor for new Object
  /// @param szs Size of the current Object
  void CursorMove(const vec2& szs);
  /// @brief Get the ViewArea of the Menu
  vec4 ViewArea() const { return view_area; }
  /// @brief Get the Main Area of the Menu
  /// (only relevant for input)
  vec4 MainArea() const { return main_area; }
  /// @brief Set a MainArea for input
  /// @param v Area where Objects can receive inputs
  void MainArea(const vec4& v) { main_area = v; }
  /// @brief Get The Scrolling offset
  vec2 ScrollOffset() const { return scrolling_off; }
  /// @brief Set a Scrolling offset
  /// @param v Custom Scrolling offset
  void ScrollOffset(const vec2& v) { scrolling_off = v; }
  /// @brief Get the Current Scrollmodification value
  vec2 ScrollMod() const { return scroll_mod; }
  /// @brief Animated Scroll to Position
  /// @param pos Destination Position
  void ScrollTo(vec2 pos) {
    scroll_anim.From(scrolling_off)
        .To(pos)
        .In(1.f)
        .As(scroll_anim.EaseInOutSine);
  }
  /// @brief Check if Still in ScrollAnimation
  bool IsAnimatedScroll() { return !scroll_anim.IsFinished(); }

  /// Objects API ///

  /// @brief Push an object to the current ListHandler
  /// @param obj Object reference to use
  /// @return Reference to the Object (from a time
  /// where ObjectPush(Container::New()) was used)
  Container::Ref ObjectPush(Container::Ref obj);
  /// @brief Search for an Object by an id
  /// @param id 32 Bit hash/id
  /// @return the found Object or nullptr
  Container::Ref FindIDObj(u32 id);

  /// Draw Lists ///

  /// @brief Background Layer Drawlist
  DrawList::Ref BackList() { return back; }
  /// @brief Setter for Background Layer Drawlist
  void BackList(DrawList::Ref v) { back = v; }
  /// @brief Main Layer Drawlist
  DrawList::Ref MainList() { return main; }
  /// @brief Setter for Main Layer Drawlist
  void MainList(DrawList::Ref v) { main = v; }
  /// @brief Foreground Layer Drawlist
  DrawList::Ref FrontList() { return front; }
  /// @brief Setter for Foreground Layer Drawlist
  void FrontList(DrawList::Ref v) { front = v; }

  /// Advanced ///

  /// @brief Display Debug Labels of the Menu
  void DebugLabels();

  /// Uneditable Stuff ///

  /// @brief Menu Name
  std::string GetName() const { return name; }
  /// @brief Menu ID [Hash of the Name]
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
  void ScrollMod(const vec2& v) { scroll_mod = v; }

  UI7Align GetAlignment() {
    if (tmpalign) {
      auto t = tmpalign;
      tmpalign = 0;
      return t;
    }
    return alignment;
  }

  /// Internal Processing
  void Update(float delta);

  /// This ability is crazy useful
  friend class Context;

  /// Data
  UI7Align alignment = UI7Align_Default;
  UI7Align tmpalign = 0;
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

  Container::Ref tmp_parent;

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
  Theme::Ref theme;

  // Input Reference
  Hid::Ref inp;

  // Animations System
  Tween<vec2> scroll_anim;
};
}  // namespace UI7
}  // namespace PD