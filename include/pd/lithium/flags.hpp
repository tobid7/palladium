#pragma once

/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/common.hpp>

/** Alias for Lithium Text Flags */
using LITextFlags = PD::u32;

/** LITextFlags */
enum LITextFlags_ {
  LITextFlags_None = 0,             ///< Do nothing
  LITextFlags_AlignRight = 1 << 0,  ///< Align Right of position
  LITextFlags_AlignMid = 1 << 1,    ///< Align in the middle of pos and box
  LITextFlags_Shaddow = 1 << 2,     ///< Draws the text twice to create shaddow
  LITextFlags_Wrap = 1 << 3,        ///< Wrap Text: May be runs better with TMS
  LITextFlags_Short = 1 << 4,       ///< Short Text: May be runs better with TMS
  LITextFlags_Scroll = 1 << 5,      ///< Not implemented [scoll text if to long]
  LITextFlags_RenderOOS = 1 << 6    ///< Render Out of Screen
};

/** Aliad for Lithium Render Flags */
using LIRenderFlags = PD::u32;
/** LIRenderFlags */
enum LIRenderFlags_ {
  LIRenderFlags_None = 0,      ///< Nothing
  LIRenderFlags_TMS = 1 << 0,  ///< Text Map System
  LIRenderFlags_LRS = 1 << 1,  ///< Layer Render System
  LIRenderFlags_AST = 1 << 2,  ///< Auto Static Text
  /** Default Enables all of them */
  LIRenderFlags_Default =
      LIRenderFlags_TMS | LIRenderFlags_LRS | LIRenderFlags_AST,
};

namespace PD {
namespace LI {
/** RenderMode [Required to modify TexENV] */
enum RenderMode {
  RenderMode_RGBA,  ///< RGBA [for textures or solid colors]
  RenderMode_Font,  ///< A8 [for textures only crated by 1 color channel]
};
}  // namespace LI
}  // namespace PD