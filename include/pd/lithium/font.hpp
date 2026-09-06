#pragma once

#include <pd/lithium/texture.hpp>

using LiTextFlags = PD::u32;
enum LiTextFlags_ : PD::u32 {
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
class Drawlist;
class PD_API Font {
 public:
  struct Codepoint {
    u32 pCodepoint;
    fvec4 SimpleUV;
    size_t Tex;
    fvec2 Size;
    float Offset = 0.f;
    bool pInvalid;
  };
  Font() {}
  ~Font() {}

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
  fvec2 GetTextBounds(const char* text, float scale);
  /**
   * Extended Draw Text Function that vreates a Command List
   */
  void CmdTextEx(Drawlist& dl, const fvec2& pos, u32 color, float scale,
                 const char* text, LiTextFlags flags = 0, const fvec2& box = 0);

  /**
   * Garbage collection for TextMapSystem
   */
  void CleanupTMS();
  /**
   * Utility function to create a font atlas
   * During TTF loading (Internal and should not be called)
   */
  void BakeAndPush(bool final, std::vector<u8>& font_tex, int texszs);
  std::string pWrapText(const std::string& txt, float scale,
                        const PD::fvec2& max, PD::fvec2& dim);
  std::string pShortText(const std::string& txt, float scale,
                         const PD::fvec2& max, PD::fvec2& dim);

  /** Data Section */
  int PixelHeight = 0;
  int DefaultPixelHeight = 24;
  std::vector<TextureID> Textures;
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

 private:
  size_t pCurrentTex = 0;
};
}  // namespace Li
}  // namespace PD