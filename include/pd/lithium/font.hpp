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

#include <pd/core/core.hpp>
#include <pd/lithium/backend.hpp>
#include <pd/lithium/pd_p_api.hpp>
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>

using LITextFlags = PD::u32;
enum LITextFlags_ {
  LITextFlags_None = 0,             ///< Do nothing
  LITextFlags_AlignRight = 1 << 0,  ///< Align Right of position
  LITextFlags_AlignMid = 1 << 1,    ///< Align in the middle of pos and box
  LITextFlags_Shaddow = 1 << 2,     ///< Draws the text twice to create shaddow
  LITextFlags_Wrap = 1 << 3,        ///< Wrap Text: May be runs better with TMS
  LITextFlags_Short = 1 << 4,       ///< Short Text: May be runs better with TMS
  LITextFlags_Scroll = 1 << 5,      ///< Not implemented [scoll text if to long]
};

namespace PD {
namespace LI {
/** Font Loader for Lithium */
class PD_LITHIUM_API Font : public SmartCtor<Font> {
 public:
  /** Codepoint Data holder */
  struct Codepoint {
    u32 pCodepoint = 0;
    fvec4 SimpleUV;
    Texture::Ref Tex;
    fvec2 Size;
    float Offset = 0.f;
    bool pInvalid = false;
  };
  Font(Backend::Ref backend) { pBackend = backend; };
  ~Font() = default;
  /**
   * Load a TTF File
   * @param path Path to the TTF file
   * @param px_height Pixelheight of the codepoints (limit by 64)
   */
  void LoadTTF(const std::string& path, int px_height = 32);
  /**
   * Getter for Codepoint reference
   * @return codepoint dataholder reference
   */
  Codepoint& GetCodepoint(u32 c);

  /**
   * Get Text Bounding Box
   */
  fvec2 GetTextBounds(const std::string& text, float scale);
  /**
   * Extended Draw Text Function that vreates a Command List
   */
  void CmdTextEx(Vec<Command::Ref>& cmds, const fvec2& pos, u32 color,
                 float scale, const std::string& text, LITextFlags flags = 0,
                 const fvec2& box = 0);

  /** Pixelheight */
  int PixelHeight;
  int DefaultPixelHeight = 24;

 private:
  /** List of textures (codepoints are using) */
  std::vector<Texture::Ref> Textures;
  /** 32Bit Codepoint Dataholder Reference Map */
  std::map<u32, Codepoint> CodeMap;
  Backend::Ref pBackend = nullptr;
};
}  // namespace LI
}  // namespace PD