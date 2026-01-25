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
#include <pd/lithium/command.hpp>
#include <pd/lithium/rect.hpp>
#include <pd/lithium/texture.hpp>
#include <pd/pd_p_api.hpp>

using LiTextFlags = PD::u32;
enum LiTextFlags_ {
  LiTextFlags_None = 0,             ///< Do nothing
  LiTextFlags_AlignRight = 1 << 0,  ///< Align Right of position
  LiTextFlags_AlignMid = 1 << 1,    ///< Align in the middle of pos and box
  LiTextFlags_Shaddow = 1 << 2,     ///< Draws the text twice to create shaddow
  LiTextFlags_Wrap = 1 << 3,        ///< Wrap Text: May be runs better with TMS
  LiTextFlags_Short = 1 << 4,       ///< Short Text: May be runs better with TMS
  LiTextFlags_Scroll = 1 << 5,      ///< Not implemented [scoll text if to long]
  LiTextFlags_NoOOS = 1 << 6,       ///< No Out of Screen Rendering
};

namespace PD {
namespace Li {
class PD_API Font {
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

  /** Constructore doesnt need Backand anymore */
  Font() = default;
  ~Font() = default;

  PD_SHARED(Font);

  /**
   * Load a TTF File
   * @param path Path to the TTF file
   * @param px_height Pixelheight of the codepoints (limit by 64)
   */
  void LoadTTF(const std::string& path, int px_height = 32);
  /**
   * Load a TTF File from Memory
   * @param data File data
   * @param px_height Pixelheight of the codepoints (limit by 64)
   */
  void LoadTTF(const std::vector<u8>& data, int px_height = 32);
  /**
   * Function that loads a default integrated font...
   * This will only work if PD_LI_INCLUDE_FONTS was set
   * on lithium build cause otherwise the font data is not included
   */
  void LoadDefaultFont(int id = 0, int pixel_height = 32);
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
  void CmdTextEx(CmdPool& cmds, const fvec2& pos, u32 color, float scale,
                 const std::string& text, LiTextFlags flags = 0,
                 const fvec2& box = 0);

  /**
   * Garbage collection for TextMapSystem
   */
  void CleanupTMS();
  /**
   * Utility function to create a font atlas
   * During TTF loading (Internal and should not be called)
   */
  void pMakeAtlas(bool final, std::vector<u8>& font_tex, int texszs,
                  PD::Li::Texture::Ref tex);
  std::string pWrapText(const std::string& txt, float scale,
                        const PD::fvec2& max, PD::fvec2& dim);
  std::string pShortText(const std::string& txt, float scale,
                         const PD::fvec2& max, PD::fvec2& dim);

  /** Data Section */
  int PixelHeight;
  int DefaultPixelHeight = 24;
  std::vector<Texture::Ref> Textures;
  /**
   * 32Bit Codepoint Dataholder reference map
   * **Now using unordered map**
   */
  std::unordered_map<u32, Codepoint> CodeMap;
  /** TMS */
  struct TMELEM {
    PD::u32 ID;
    PD::fvec2 Size;
    std::string Text;
    u64 TimeStamp;
  };
  std::unordered_map<u32, TMELEM> pTMS;
};
}  // namespace Li
}  // namespace PD