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

#include <pd/drivers/hid.hpp>  //// WOW A NON UI/ Header
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>
#include <pd/ui7/menu.hpp>
#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
class Context : public SmartCtor<Context> {
 public:
  Context(LI::Renderer::Ref ren, Hid::Ref hid) {
    this->ren = ren;
    this->inp = hid;
    theme = Theme::New();
    back = DrawList::New(ren);
    front = DrawList::New(ren);
  }
  ~Context() {}

  bool BeginMenu(const ID& id, UI7MenuFlags flags = 0);
  Menu::Ref GetCurrentMenu();
  Menu::Ref FindMenu(const ID& id);
  void EndMenu();

  /// @brief Get Theme reference
  /// @return Reference to the base Theme of the context
  Theme::Ref GetTheme() { return theme; }
  /// @brief Directly return a Color by using the
  /// ctx->ThemeColor(UI7Color_Text) for example
  /// @param clr The Input UI7 Color
  /// @return The 32bit color value
  u32 ThemeColor(UI7Color clr) const { return theme->Get(clr); }

  /// @brief Update Context (Render menus)
  /// @param delta deltatime
  void Update(float delta);

  /// Expose DrawLists
  DrawList::Ref BackList() { return back; }
  DrawList::Ref FrontList() { return front; }

  void RootLayer(int l) { root_layer = l; }
  int RootLayer() const { return root_layer; }

 private:
  // Used in Overlays
  int root_layer = 0;
  // Linked Renderer / Hid
  LI::Renderer::Ref ren;
  Hid::Ref inp;
  // Timing
  float delta;
  float time;
  float last;
  // Context
  bool in_menu;
  // Debug
  bool debugging;
  // Menu Handlers
  std::unordered_map<u32, Menu::Ref> menus;
  std::vector<u32> amenus;  // Active ones
  Menu::Ref current;
  // Context DrawList
  DrawList::Ref debug;
  DrawList::Ref front;
  DrawList::Ref back;
  // Theme
  Theme::Ref theme;
  // Promt Handler
};
}  // namespace UI7
}  // namespace PD