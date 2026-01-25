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

#include <pd/ui7/theme.hpp>

namespace PD {
namespace UI7 {
PD_API void Theme::Default(Theme& theme) {
  theme.Set(UI7Color_Text, Color("#FFFFFFFF"));
  theme.Set(UI7Color_TextDead, Color("#AAAAAAFF"));
  theme.Set(UI7Color_Background, Color("#222222aa"));
  theme.Set(UI7Color_Border, Color("#999999ff"));
  theme.Set(UI7Color_Button, Color("#111111FF"));
  theme.Set(UI7Color_ButtonDead, Color("#080808FF"));
  theme.Set(UI7Color_ButtonActive, Color("#2A2A2AFF"));
  theme.Set(UI7Color_ButtonHovered, Color("#222222FF"));
  theme.Set(UI7Color_Header, Color("#111111cc"));
  theme.Set(UI7Color_HeaderDead, Color("#080808FF"));
  theme.Set(UI7Color_Selector, Color("#222222FF"));
  theme.Set(UI7Color_Checkmark, Color("#2A2A2AFF"));
  theme.Set(UI7Color_FrameBackground, Color("#555555FF"));
  theme.Set(UI7Color_FrameBackgroundHovered, Color("#777777FF"));
  theme.Set(UI7Color_Progressbar, Color("#00FF00FF"));
  theme.Set(UI7Color_ListEven, Color("#CCCCCCFF"));
  theme.Set(UI7Color_ListOdd, Color("#BBBBBBFF"));
}

PD_API void Theme::Flashbang(Theme& theme) {
  theme.Set(UI7Color_Text, Color("#000000FF"));
  theme.Set(UI7Color_TextDead, Color("#333333FF"));
  theme.Set(UI7Color_Background, Color("#eeeeeeFF"));
  theme.Set(UI7Color_Border, Color("#777777ff"));
  theme.Set(UI7Color_Button, Color("#ccccccFF"));
  theme.Set(UI7Color_ButtonDead, Color("#bbbbbbFF"));
  theme.Set(UI7Color_ButtonActive, Color("#ccccccFF"));
  theme.Set(UI7Color_ButtonHovered, Color("#acacacFF"));
  theme.Set(UI7Color_Header, Color("#ddddddFF"));
  theme.Set(UI7Color_HeaderDead, Color("#cdcdcdFF"));
  theme.Set(UI7Color_Selector, Color("#222222FF"));
  theme.Set(UI7Color_Checkmark, Color("#ccccccFF"));
  theme.Set(UI7Color_FrameBackground, Color("#aaaaaaFF"));
  theme.Set(UI7Color_FrameBackgroundHovered, Color("#909090FF"));
  theme.Set(UI7Color_Progressbar, Color("#00FF00FF"));
  theme.Set(UI7Color_ListEven, Color("#CCCCCCFF"));
  theme.Set(UI7Color_ListOdd, Color("#BBBBBBFF"));
}
}  // namespace UI7
}  // namespace PD