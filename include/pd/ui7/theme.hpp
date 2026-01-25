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

/**
 *  Using this to support 32bit color values as well as
 * values from UI7Color_
 */
using UI7Color = PD::u32;

/** Theme Color */
enum UI7Color_ {
  UI7Color_Background,              ///< UI7 Menu Background
  UI7Color_Border,                  ///< Menu/Frame Border Color
  UI7Color_Button,                  ///< UI7 Button Idle Color
  UI7Color_ButtonDead,              ///< UI7 Disabled Button Color
  UI7Color_ButtonActive,            ///< UI7 Pressed Button Color
  UI7Color_ButtonHovered,           ///< UI7 Hovered Button Color
  UI7Color_Text,                    ///< UI7 Text Color
  UI7Color_TextDead,                ///< UI7 Dead Text Color
  UI7Color_Header,                  ///< UI7 Menu Header Color
  UI7Color_HeaderDead,              ///< Inactive Header
  UI7Color_Selector,                ///< UI7 Selector Color
  UI7Color_Checkmark,               ///< UI7 Checkmark Color
  UI7Color_FrameBackground,         ///< UI7 Frame Background
  UI7Color_FrameBackgroundHovered,  ///< UI7 Hovered Frame Background
  UI7Color_Progressbar,             ///< UI7 Progressbar Background
  UI7Color_ListEven,                ///< UI7 List (Even Entry) Background Color
  UI7Color_ListOdd,                 ///< UI7 List (Odd Entry) Background Color
};

namespace PD {
namespace UI7 {
class PD_API Theme {
 public:
  /**
   * Default Constructor Setting up the Default theme
   * @note if using Shared Reference you probably need to do
   * Theme::Default(*theme.get());
   */
  Theme() { Default(*this); }
  ~Theme() {}
  PD_SHARED(Theme);

  /**
   * Simple static Loader for the Default Theme
   * @param theme Theme Reference
   */
  static void Default(Theme& theme);
  /**
   * White Mode Theme
   * @param Theme theme reference
   */
  static void Flashbang(Theme& theme);

  /** Revert the last Color Change */
  Theme& Pop() {
    pTheme[pChanges[pChanges.size() - 1].first] =
        pChanges[pChanges.size() - 1].second;
    pChanges.pop_back();
    return *this;
  }

  /**
   * Revert the last color Change done for a specific color
   * @param c Color to revert change from
   */
  Theme& Pop(UI7Color c) {
    for (size_t i = pChanges.size() - 1; i > 0; i--) {
      if (pChanges[i].first == c) {
        pTheme[c] = pChanges[i].second;
        pChanges.erase(pChanges.begin() + i);
        break;
      }
    }
    return *this;
  }

  /**
   * Change a Color
   * @param tc Color Identifier
   * @param color Color to change to
   */
  Theme& Change(UI7Color tc, u32 color) {
    if (pTheme.find(tc) == pTheme.end()) {
      return *this;
    }
    pChanges.push_back(std::make_pair(tc, pTheme[tc]));
    pTheme[tc] = color;
    return *this;
  }

  /**
   * Get the Color of a Color ReferenceID
   * @param c ReferenceID
   */
  u32 Get(UI7Color c) const {
    auto e = pTheme.find(c);
    if (e == pTheme.end()) {
      return 0x00000000;
    }
    return e->second;
  }

  /**
   * [UNSAFE] to use
   * Get the Color Ref of a Color ReferenceID
   * @param c ReferenceID
   */
  u32& GetRef(UI7Color c) {
    auto e = pTheme.find(c);
    if (e == pTheme.end()) {
      static u32 noclr = 0x00000000;
      return noclr;
    }
    return e->second;
  }

  /**
   * Operator wrapper for get
   * @param c Color ReferenceID
   */
  u32 operator[](UI7Color c) const { return Get(c); }

  /**
   * Change but just sets [can implement completly new ids]
   * @param tc Color ID (Can be self creeated ones as well)
   * @param clr Color it should be set to
   */
  void Set(UI7Color tc, u32 clr) { pTheme[tc] = clr; }

  std::unordered_map<u32, u32> pTheme;             ///< Theme Data
  std::vector<std::pair<UI7Color, u32>> pChanges;  ///< List of Changes
};
}  // namespace UI7
}  // namespace PD