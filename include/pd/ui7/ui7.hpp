#pragma once

/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/core/core.hpp>
#include <pd/pd_p_api.hpp>
#include <pd/ui7/io.hpp>
#include <pd/ui7/menu.hpp>

#include "pd/ui7/flags.hpp"

/**
 * Declare UI7 Version
 * Format: 00    00    00    00
 *         Major Minor Patch Build
 * 0x01010000 -> 1.1.0-0
 */
#define UI7_VERSION 0x00070000

namespace PD {
namespace UI7 {
/**
 * Get UI7 Version String
 * @param show_build Show build num (mostly unused)
 * @return Version String (1.0.0-1 for example)
 */
PD_API std::string GetVersion(bool show_build = false);
/** Base Context for UI7 */
class PD_API Context {
 public:
  Context() {}
  ~Context() = default;

  IO& GetIO() { return pIO; }
  void AddViewPort(const ID& id, const ivec4& vp);
  void UseViewPort(const ID& id);
  void Update();
  Menu* BeginMenu(const ID& id, UI7MenuFlags flags = 0, bool* pShow = nullptr);
  Menu* CurrentMenu() { return pCurrent; }
  void EndMenu();
  void AboutMenu(bool* show = nullptr);
  void MetricsMenu(bool* show = nullptr);
  void StyleEditor(bool* show = nullptr);

  Li::Drawlist& GetDrawData() { return pIO.FDL; }

  Menu* pGetOrCreateMenu(const ID& id) {
    auto menu = pMenus.find(id);
    if (menu == pMenus.end()) {
      pMenus[id] = new Menu(id, pIO);
      menu = pMenus.find(id);
    }
    return menu->second;
  }

  IO pIO;
  /** Current Menu */
  Menu* pCurrent = nullptr;
  std::vector<u32> pCurrentMenus;
  std::vector<u32> pDFO; /** Debug Final Order */
  std::unordered_map<u32, Menu*> pMenus;
};
}  // namespace UI7
}  // namespace PD