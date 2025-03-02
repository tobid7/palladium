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

#include <pd/core/tween.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/ui7/containers.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>

namespace PD {
namespace UI7 {
/** Menu Class for UI7 */
class Menu : public SmartCtor<Menu> {
 public:
  /**
   * Menu COnstructor (Unly used by UI7::Context)
   * @param id ID of the Menu
   * @param tl Theme Reference
   * @param h Input Driver Reference
   */
  Menu(ID id, Theme::Ref tl, Hid::Ref h) {
    /// Setup the Input Data
    theme = tl;
    this->inp = h;
    this->id = id;
    this->name = id.GetName();
    /// Set Default Values here
    scrolling[0] = false;
    scrolling[1] = false;
    scrollbar[0] = false;
    scrollbar[1] = false;
    scroll_allowed[0] = false;
    scroll_allowed[1] = false;
  }
  ~Menu() = default;

  // Objects

  /**
   * Render a Simple Label
   * @param label The text to draw
   */
  void Label(const std::string& label);
  /**
   * Render a Button
   * @param label The buttons text
   * @return if the button was pressed
   */
  bool Button(const std::string& label);
  /**
   * Render a Checkbox
   * @param label Label of the Checkbox
   * @param v A value to update
   */
  void Checkbox(const std::string& label, bool& v);
  /**
   * Render an Image
   * @param img Texture reference of the image
   * @param size a Custom Size if needed
   */
  void Image(Texture::Ref img, vec2 size = 0.f);

  // Basic API

  /** Add the Next Objext to the same line */
  void SameLine();
  /** Add a Separator Line */
  void Separator();
  /**
   * Render a Separator Line with a Text
   * @todo determinate text position by current alignment
   * @param label The Text to show
   */
  void SeparatorText(const std::string& label);
  /** Put the last Added Object into the Joinlist */
  void Join();
  /**
   * Add the Last element to the join list
   * and perform an alignment operation
   * @param a Alignment Oeration(s)
   */
  void JoinAlign(UI7Align a);
  /**
   * Align the Last Object
   * @param a Alignment Operation
   */
  void AfterAlign(UI7Align a);
  /**
   * Set a Temp alignment op for the next Object
   * @param a Alignment Operation
   */
  void NextAlign(UI7Align a) { tmpalign = a; }
  /**
   * Align Every Single Object by this operationset
   * @param a Alignment
   */
  void PushAlignment(UI7Align a) { alignment = a; }
  /** Use default alignment */
  void PopAlignment() { alignment = UI7Align_Default; }
  /**
   * Get a New Position depending on the Alignment
   * @param pos Current Position
   * @param size Object size
   * @param view Viewport [position and size]
   * @param a Alignment Operations
   * @return new position based on the alignment
   */
  static vec2 AlignPos(vec2 pos, vec2 size, vec4 view, UI7Align a);
  /**
   * Returns a Reference to the theme
   * @return Reference to the base Theme of the context
   */
  Theme::Ref GetTheme() { return theme; }
  /**
   * Directly return a Color by using the
   * m->ThemeColor(UI7Color_Text) for example
   * @param clr The Input UI7 Color
   * @return The 32bit color value
   */
  u32 ThemeColor(UI7Color clr) const { return theme->Get(clr); }

  // API for Custom Objects

  /**
   * Handles the Position of Objects in Scrolling Menu
   * @note As Containers have their own FUnc to handle this, this
   * function is only useful to Render Live Objects whicch cannot be aligned
   * by the internal Alignment Api
   * @param pos position reference to write the new position to
   * @param size size of the Object
   * @return if the object can be skipped in rendering
   */
  bool HandleScrolling(vec2& pos, const vec2& size);
  /**
   * Get the Cursor Position
   * @return Cursor Pos
   */
  vec2 Cursor() const { return cursor; }
  /**
   * Set the Cursor position
   * @note The old Position can be restored with RestoreCursor
   * @param v New Position
   */
  void Cursor(const vec2& v) {
    bcursor = cursor;
    cursor = v;
  }
  /** Restore to the last cursor Position */
  void RestoreCursor() {
    cursor = bcursor;
    bcursor = vec2();
  }
  /** Return if a Vertical Scrollbar exists */
  bool HasVerticalScrollbar() { return scrollbar[1]; }
  /** Return if a Horizontal Scrollbar exists */
  bool HasHorizontalScrollbar() { return scrollbar[0]; }
  /** Get the Titlebar height */
  float TitleBarHeight() { return tbh; }
  /**
   * Set a Custom Titlebar heigt
   * @note Could destroy some basic functionality
   */
  void TitleBarHeight(float v) { tbh = v; }
  /**
   * Init the Cursor
   * @note Useful when using with a Custom TitlebarHeight
   */
  void CursorInit() { Cursor(vec2(5, tbh + 5)); }
  /**
   * Move the Cursor for new Object
   * @param szs Size of the current Object
   */
  void CursorMove(const vec2& szs);
  /** Get the ViewArea of the Menu */
  vec4 ViewArea() const { return view_area; }
  /**
   * Get the Main Area of the Menu
   * (only relevant for input)
   */
  vec4 MainArea() const { return main_area; }
  /**
   * Set a MainArea for input
   * @param v Area where Objects can receive inputs
   */
  void MainArea(const vec4& v) { main_area = v; }
  /** Get The Scrolling offset */
  vec2 ScrollOffset() const { return scrolling_off; }
  /**
   * Set a Scrolling offset
   * @param v Custom Scrolling offset
   */
  void ScrollOffset(const vec2& v) { scrolling_off = v; }
  /** Get the Current Scrollmodification value */
  vec2 ScrollMod() const { return scroll_mod; }
  /**
   * Animated Scroll to Position
   * @param pos Destination Position
   */
  void ScrollTo(vec2 pos) {
    scroll_anim.From(scrolling_off)
        .To(pos)
        .In(1.f)
        .As(scroll_anim.EaseInOutSine);
  }
  /** Check if Still in ScrollAnimation */
  bool IsAnimatedScroll() { return !scroll_anim.IsFinished(); }

  // Objects API

  /**
   * Push an object to the current ListHandler
   * @param obj Object reference to use
   * @return Reference to the Object (from a time
   * where ObjectPush(Container::New()) was used)
   */
  Container::Ref ObjectPush(Container::Ref obj);
  /**
   * Search for an Object by an id
   * @param id 32 Bit hash/id
   * @return the found Object or nullptr
   */
  Container::Ref FindIDObj(u32 id);
  /**
   * Create a Parent Container to move and edit all sub
   * instances at once
   */
  void CreateParent();
  /** Destory the parent container (if one active) */
  void DestroyParent() { tmp_parent = nullptr; }

  // Draw Lists

  /** Background Layer Drawlist */
  DrawList::Ref BackList() { return back; }
  /** Setter for Background Layer Drawlist */
  void BackList(DrawList::Ref v) { back = v; }
  /** Main Layer Drawlist */
  DrawList::Ref MainList() { return main; }
  /** Setter for Main Layer Drawlist */
  void MainList(DrawList::Ref v) { main = v; }
  /** Foreground Layer Drawlist */
  DrawList::Ref FrontList() { return front; }
  /** Setter for Foreground Layer Drawlist */
  void FrontList(DrawList::Ref v) { front = v; }

  // Advanced

  /** Display Debug Labels of the Menu */
  void DebugLabels();

  // Uneditable Stuff

  /** Menu Name */
  std::string GetName() const { return name; }
  /** Menu ID [Hash of the Name] */
  u32 GetID() const { return id; }

 private:
  // Advanced Handlers

  /**
   * Setup for the Menu
   * @param flags Menu Flags
   */
  void PreHandler(UI7MenuFlags flags);
  /** Handle things like scrolling */
  void PostHandler();

  // Basic Settings

  /**
   * Set Backup Cursor
   * @param v Position
   */
  void BackupCursor(const vec2& v) { bcursor = v; }
  /** Get Sameline Cursor */
  vec2 SameLineCursor() const { return slcursor; }
  /**
   * Set Sameline Cursor
   * @param v Position
   */
  void SameLineCursor(const vec2& v) { slcursor = v; }
  /**
   * Set View Area
   * @param v vec4 containing pos and size
   */
  void ViewArea(const vec4& v) { view_area = v; }
  /**
   * Set Scroll Modification
   * @param v Mod
   */
  void ScrollMod(const vec2& v) { scroll_mod = v; }

  /** Get the Alignment for Current State */
  UI7Align GetAlignment() {
    /// if temp alignment is used then return it and
    /// reset tmpalign
    if (tmpalign) {
      auto t = tmpalign;
      tmpalign = 0;
      return t;
    }
    return alignment;
  }

  /** Internal Processing */
  void Update(float delta);

  // This ability is crazy useful
  friend class Context;

  // Data

  // Default Alignment for all Objects
  UI7Align alignment = UI7Align_Default;
  UI7Align tmpalign = 0;   ///< Temp Alignment [only used once]
  UI7MenuFlags flags = 0;  ///< Menu Flags
  u32 id;                  ///< Menu ID
  std::string name;        ///< Menu Name
  vec2 cursor;             ///< Current Cursor Position
  vec2 bcursor;            ///< Backup Cursor
  vec2 slcursor;           ///< Sameline Cursor
  vec4 view_area;          ///< view Area
  vec4 main_area;          ///< Main Area [Input related]
  vec2 scrolling_off;      ///< Scrolling Position
  bool scrolling[2];       ///< Is Hz or Vt Scrolling Enabled
  vec2 scroll_mod;         ///< Scroll Modificator
  float tbh;               ///< Titlebar height
  bool scrollbar[2];       ///< Is Hz or Vt Scrollbar rendered
  bool scroll_allowed[2];  ///< Is Hz or Vt Scrolling Alowed
  bool has_touch;          ///< Menu has touch (depends on screen)

  Container::Ref tmp_parent;  ///< Parent Container (for better alignment etc)

  // Objects API

  std::vector<Container::Ref> objects;  ///< Current frame Objects
  std::vector<Container::Ref> idobjs;   ///< Objects using an ID
  std::vector<Container*> join;         ///< List of Combined Objects
  int count_btn = 0;                    ///< Count for Button ID Prefix
  int count_cbx = 0;                    ///< Cound for Checkbox ID Prefix

  // DrawLists

  DrawList::Ref back;   ///< Background Drawlist
  DrawList::Ref main;   ///< Main Drawlist
  DrawList::Ref front;  ///< Foreground Drawlist

  vec2 max;        ///< Max Position
  vec2 mouse;      ///< Mouse/Touch Position
  vec2 bslpos;     ///< Before Sameline Position
  vec2 last_size;  ///< Last Object Size

  // Theme
  Theme::Ref theme;

  // Input Reference
  Hid::Ref inp;

  // Animations System

  Tween<vec2> scroll_anim;  ///< for Scroll to Animation
};
}  // namespace UI7
}  // namespace PD