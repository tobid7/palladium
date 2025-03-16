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
#include <pd/ui7/io.hpp>
#include <pd/ui7/layout.hpp>

namespace PD {
namespace UI7 {
/** Menu Class for UI7 */
class Menu : public SmartCtor<Menu> {
 public:
  /**
   * Menu COnstructor (Unly used by UI7::Context)
   * @param id ID of the Menu
   * @param io IO Config Reference
   */
  Menu(ID id, UI7::IO::Ref io) {
    /// Setup the Input Data
    this->io = io;
    this->id = id;
    this->name = id.GetName();
    /// Set Default Values here
    scrollbar[0] = false;
    scrollbar[1] = false;
    scroll_allowed[0] = false;
    scroll_allowed[1] = false;
    Layout = UI7::Layout::New(id, io);
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
  void Image(Texture::Ref img, vec2 size = 0.f, LI::Rect uv = vec4(0));

  /**
   * Color Edit Object that opens a popup editor if clicked
   * @param label Name of the Edit field
   * @param color Color reference to edit
   */
  void ColorEdit(const std::string& label, u32* color);

  void DragFloat(const std::string& label, float* data, size_t num_elms);
  template <typename T>
  void DragData(const std::string& label, T* data, size_t num_elms = 1,
                T min = std::numeric_limits<T>::min(),
                T max = std::numeric_limits<T>::max(), T step = 1,
                int precision = 1) {
    u32 id = Strings::FastHash("drd" + label + std::to_string((u32)data));
    Container::Ref r = Layout->FindObject(id);
    if (!r) {
      r = PD::New<UI7::DragData<T>>(label, data, num_elms, io, min, max, step,
                                    precision);
      r->SetID(id);
    }
    Layout->AddObject(r);
  }
  // Basic API

  /**
   * Create a Tree Node
   * @param id String ID of the Node
   * @return node open or not
   */
  bool BeginTreeNode(const UI7::ID& id);
  /**
   * End a Tree Node
   */
  void EndTreeNode();

  /** Add the Next Objext to the same line */
  void SameLine() { Layout->SameLine(); }
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
  void NextAlign(UI7Align a) { Layout->NextAlign(a); }
  /**
   * Align Every Single Object by this operationset
   * @param a Alignment
   */
  void PushAlignment(UI7Align a) { Layout->SetAlign(a); }
  /** Use default alignment */
  void PopAlignment() { Layout->Alignment = UI7Align_Default; }
  /**
   * Returns a Reference to the theme
   * @return Reference to the base Theme of the context
   */
  Theme::Ref GetTheme() { return io->Theme; }
  /**
   * Directly return a Color by using the
   * m->ThemeColor(UI7Color_Text) for example
   * @param clr The Input UI7 Color
   * @return The 32bit color value
   */
  u32 ThemeColor(UI7Color clr) const { return io->Theme->Get(clr); }

  /**
   * Get IO Reference
   * @return io Reference
   */
  UI7::IO::Ref GetIO() { return io; }

  // API for Custom Objects

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
   * Animated Scroll to Position
   * @param pos Destination Position
   */
  void ScrollTo(vec2 pos) {
    scroll_anim.From(Layout->ScrollOffset)
        .To(pos)
        .In(1.f)
        .As(scroll_anim.EaseInOutSine);
  }
  /** Check if Still in ScrollAnimation */
  bool IsAnimatedScroll() { return !scroll_anim.IsFinished(); }

  // Objects API

  /**
   * Create a Parent Container to move and edit all sub
   * instances at once
   */
  void CreateParent();
  /** Destory the parent container (if one active) */
  void DestroyParent() { tmp_parent = nullptr; }

  // Draw List

  /** Get DrawList */
  DrawList::Ref GetDrawList() { return Layout->DrawList; }
  UI7::Layout::Ref GetLayout() { return Layout; }

  // Advanced

  /**
   * Display Debug Labels of a Menu
   * @param m Menu to display Data from
   * @param t Target to Write the Labels into
   */
  static void DebugLabels(Menu::Ref m, Menu::Ref t = nullptr);

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

  /** Internal Processing */
  void Update(float delta);

  // Put some Buttons and functionality into its own functions

  /** Handler of the Close Button (if exists) */
  void CloseButtonHandler();
  /** Handler of the Resize Dragging (lower right corner) */
  void ResizeHandler();
  /** Logic of the Titlebar Movement */
  void MoveHandler();
  /** Menu Collapse Button Handler */
  void CollapseHandler();
  /** Scroll Handler (Includes Slider Drag) */
  void PostScrollHandler();

  // This ability is crazy useful
  friend class Context;

  // Data

  UI7MenuFlags flags = 0;    ///< Menu Flags
  u32 id;                    ///< Menu ID
  std::string name;          ///< Menu Name
  float tbh;                 ///< Titlebar height
  bool scrollbar[2];         ///< Is Hz or Vt Scrollbar rendered
  bool scroll_allowed[2];    ///< Is Hz or Vt Scrolling Alowed
  bool has_touch;            ///< Menu has touch (depends on screen)
  bool is_open = true;       ///< For Collapse Event
  bool* is_shown = nullptr;  ///< For Close Button

  Container::Ref tmp_parent;  ///< Parent Container (for better alignment etc)

  // Objects API

  std::vector<Container*> join;  ///< List of Combined Objects
  int count_btn = 0;             ///< Count for Button ID Prefix
  int count_cbx = 0;             ///< Cound for Checkbox ID Prefix

  UI7::IO::Ref io;  ///< IO Reference

  std::map<u32, bool> tree_nodes;  ///< Map of Tree nodes

  // Animations System

  Tween<vec2> scroll_anim;  ///< for Scroll to Animation

  // Layout API
  PD::UI7::Layout::Ref Layout;
};
}  // namespace UI7
}  // namespace PD