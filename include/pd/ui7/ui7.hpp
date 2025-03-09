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

#include <pd/core/timetrace.hpp>
#include <pd/drivers/hid.hpp>
#include <pd/ui7/drawlist.hpp>
#include <pd/ui7/flags.hpp>
#include <pd/ui7/id.hpp>
#include <pd/ui7/io.hpp>
#include <pd/ui7/menu.hpp>
#include <pd/ui7/theme.hpp>
/**
 * Declare UI7 Version
 * Format: 00    00    00    00
 *         Major Minor Patch Build
 * 0x01010000 -> 1.1.0-0
 */
#define UI7_VERSION 0x00020901

namespace PD {
namespace UI7 {
/**
 * Get UI7 Version String
 * @param show_build Ahow build num (mostly unused)
 * @return Version String (1.0.0-1 for example)
 */
std::string GetVersion(bool show_build = false);
/** Base Context for UI7 */
class Context : public SmartCtor<Context> {
 public:
  /**
   * Constructor for UI7 Context
   * @param ren Renderer Reference
   * @param hid Input Driver Reference
   */
  Context(LI::Renderer::Ref ren, Hid::Ref hid) {
    /// Set the Internal References
    io = IO::New(hid, ren);
  }
  ~Context() = default;

  /**
   * Begin a New Menu
   * @param id Menu ID / Name shown in Titlebar
   * @param flags Optional flags to change stuff
   * @return If the Menu was Created
   * (useless as false results in an error screen)
   */
  bool BeginMenu(const ID& id, UI7MenuFlags flags = 0, bool* show = nullptr);
  /**
   * Get the Current Menu
   * for example for auto m = ctx->GetCurrentMenu
   */
  Menu::Ref GetCurrentMenu();
  /**
   * Find a Menu by its ID to edit things outside of
   * the place between Begin and EndMenu
   * @param id ID (Menu Name) to search for
   */
  Menu::Ref FindMenu(const ID& id);
  /**
   * Ends the Current Menu
   * (to be able to create another one)
   */
  void EndMenu();

  /**
   * Get Theme reference
   * @return Reference to the base Theme of the context
   */
  Theme::Ref GetTheme() { return io->Theme; }
  /**
   *Directly return a Color by using the
   * ctx->ThemeColor(UI7Color_Text) for example
   * @param clr The Input UI7 Color
   * @return The 32bit color value
   */
  u32 ThemeColor(UI7Color clr) const { return io->Theme->Get(clr); }

  /**
   * Update Context (Render menus)
   * @param delta deltatime
   */
  void Update(float delta);

  // Expose DrawLists

  /** Background DrawList Reference */
  DrawList::Ref BackList() { return io->Back; }
  /** Foreground DrawList Reference */
  DrawList::Ref FrontList() { return io->Front; }

  /**
   * Set the Root Layer of the Menu
   * @param l Layer
   */
  void RootLayer(int l) { root_layer = l; }
  /** Get the Root Layer of the Menu */
  int RootLayer() const { return root_layer; }

  /** Get IO Reference */
  IO::Ref GetIO() { return io; }

  // Debugging / Demo / About

  /** About Menu */
  void AboutMenu(bool* show = nullptr);

  /** Metrics */
  void MetricsMenu(bool* show = nullptr);

  /** Style Editor Menu (Includes Theme Editor) */
  void StyleEditor(bool* show = nullptr);

 private:
  // Used in Overlays
  int root_layer = 0;
  // Map of The Menus by ID
  std::unordered_map<u32, Menu::Ref> menus;
  std::vector<u32> amenus;  ///< Active ones
  Menu::Ref current;        ///< Current Menu
  // IO
  IO::Ref io;
};
}  // namespace UI7
}  // namespace PD