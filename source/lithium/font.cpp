#include <pd/core/core.hpp>
#include <pd/drivers/gfx.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/math.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <map>

namespace PD {
namespace Li {
PD_API void Font::LoadTTF(const std::string& path, int px_height) {
  /**
   * Just use LoadFile2Mem which looks way cleaner
   * and helps not having the font loading code twice
   * when adding LoadTTF with mem support
   */
  PDLOG("Font: Loading {}...", path);
  TT::Scope st("LI_LoadTTF_" + path);
  auto font = PD::IO::LoadFile2Mem(path);
  PDLOG("Font Size: {}", PD::Strings::FormatBytes(font.size()));
  LoadTTF(font, px_height);
}

PD_API void Font::LoadTTF(const std::vector<u8>& data, int px_height) {
  /**
   * Some additional Info:
   * Removed the stbtt get bitmapbox as we dont need to place
   * the glyps nicely in the tex. next step would be using the free
   * space on the y axis to get mor glyphs inside
   */
  PixelHeight = px_height;
  int texszs = PD::Bits::GetPow2(px_height * 16);
  if (texszs > 1024) {
    texszs = 1024;  // Max size
  }

  stbtt_fontinfo inf;
  if (!stbtt_InitFont(&inf, data.data(), 0)) {
    return;
  }

  float scale = stbtt_ScaleForPixelHeight(&inf, PixelHeight);

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&inf, &ascent, &descent, &lineGap);
  int baseline = static_cast<int>(ascent * scale);

  // Cache to not render same codepoint tex twice
  std::map<u32, int> buf_cache;

  std::vector<u8> font_tex(texszs * texszs * 4, 0);
  fvec2 off;

  bool empty = true;

  for (u32 ii = 0x0000; ii <= 0xFFFF; ii++) {
    int gi = stbtt_FindGlyphIndex(&inf, ii);
    if (gi == 0) continue;
    if (stbtt_IsGlyphEmpty(&inf, gi)) continue;

    int w = 0, h = 0, xo = 0, yo = 0;
    unsigned char* bitmap =
        stbtt_GetCodepointBitmap(&inf, scale, scale, ii, &w, &h, &xo, &yo);
    if (!bitmap || w <= 0 || h <= 0) {
      if (bitmap) free(bitmap);
      continue;
    }

    u32 hashed_map = IO::HashMemory(std::vector<u8>(bitmap, bitmap + (w * h)));
    if (buf_cache.find(hashed_map) != buf_cache.end()) {
      Codepoint c = GetCodepoint(buf_cache[hashed_map]);
      c.pCodepoint = ii;
      CodeMap[ii] = c;
      free(bitmap);
      continue;
    } else {
      buf_cache[hashed_map] = ii;
    }

    // Next row
    if (off.x + w > texszs) {
      off.y += PixelHeight;
      off.x = 0.0f;
    }
    // Bake cause we go out of the tex
    if (off.y + PixelHeight > texszs) {
      BakeAndPush(false, font_tex, texszs);
      off = 0;
      std::fill(font_tex.begin(), font_tex.end(), 0);
      empty = true;
    }

    // UVs & Codepoint
    Codepoint c;
    fvec4 uvs;
    // cast the ints to floats and not the floats...
    // dont know where my mind was when creating the code
    uvs.x = off.x / static_cast<float>(texszs);
    uvs.y = off.y / static_cast<float>(texszs);
    uvs.z = (off.x + w) / static_cast<float>(texszs);
    uvs.w = (off.y + h) / static_cast<float>(texszs);
    // Flip if needed
    if (PD::Gfx::GetFlags() & PDBackendFlags_FlipUV_Y) {
      uvs.y = 1.f - uvs.y;
      uvs.w = 1.f - uvs.w;
    }
    c.SimpleUV = uvs;
    c.Tex = pCurrentTex;
    c.Size = fvec2(w, h);
    c.Offset = baseline + yo;
    c.pCodepoint = ii;

    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        int map_pos = ((static_cast<int>(off.y) + y) * texszs +
                       (static_cast<int>(off.x) + x)) *
                      4;
        font_tex[map_pos + 0] = 255;
        font_tex[map_pos + 1] = 255;
        font_tex[map_pos + 2] = 255;
        font_tex[map_pos + 3] = bitmap[x + y * w];
      }
    }

    empty = false;
    CodeMap[ii] = c;
    free(bitmap);

    // offset by 1 (prevents visual glitches i had)
    off.x += w + 1;
  }

  if (!empty) {
    BakeAndPush(true, font_tex, texszs);
  }
}

PD_API void Font::LoadDefaultFont(int id, int pixel_height) {}

PD_API Font::Codepoint& Font::GetCodepoint(u32 c) {
  // Check if codepoijt exist or return a static invalid one
  auto res = CodeMap.find(c);
  if (res == CodeMap.end()) {
    static Codepoint invalid;
    invalid.pInvalid = true;
    return invalid;
  }
  return res->second;
}

PD_API fvec2 Font::GetTextBounds(const char* text, float scale) {
  // Create a temp position and offset as [0, 0]
  fvec2 res;
  float x = 0;
  // Curent Font Scale
  float cfs = (DefaultPixelHeight * scale) / (float)PixelHeight;
  float lh = (float)PixelHeight * cfs;
  U8Iterator it(text);
  u32 c;
  while (it.Decode32(c)) {
    auto cp = GetCodepoint(c);
    if (!cp.pInvalid && c != '\n' && c != '\t' && c != ' ') {
      continue;
    }
    switch (c) {
      case L'\n':
        res.y += lh;
        res.x = std::max(res.x, x);
        x = 0.f;
        break;
      case L'\t':
        x += 16 * cfs;
        break;
      case L' ':
        x += 4 * cfs;
      // Fall trough here to get the same result as in
      // TextCommand if/else Section
      default:
        x += cp.Size.x * cfs;
        // Omly passing c cause i know its not used
        if (it.PeekNext32(c)) {
          x += 2 * cfs;
        }
        break;
    }
  }
  res.x = std::max(res.x, x);
  res.y += lh;
  return res;
}

PD_API void Font::CmdTextEx(Drawlist& dl, const fvec2& pos, u32 color,
                            float scale, const char* text, LiTextFlags flags,
                            const fvec2& box) {
  fvec2 off;
  float cfs = (DefaultPixelHeight * scale) / (float)PixelHeight;
  float lh = (float)PixelHeight * cfs;
  fvec2 td;
  fvec2 rpos = pos;
  fvec2 rbox = box;

  if (flags & (LiTextFlags_AlignMid | LiTextFlags_AlignRight)) {
    td = GetTextBounds(text, scale);
  }
  if (flags & LiTextFlags_AlignMid) rpos = rbox * 0.5 - td * 0.5 + pos;
  if (flags & LiTextFlags_AlignRight) rpos.x -= td.x;

  U8Iterator it(text);
  u32 c;
  Command* cmd = nullptr;

  while (it.Decode32(c)) {
    auto cp = GetCodepoint(c);
    if ((!cp.pInvalid && c != '\n' && c != '\t' && c != ' ') && c != '\r')
      continue;

    if (c == L'\n') {
      off.y += lh;
      off.x = 0.f;
      continue;
    }

    if (c == L'\t') {
      off.x += 16 * cfs;
      continue;
    }
    if (c == L' ') {
      off.x += 4 * cfs;
      continue;
    }

    if (cmd == nullptr || cmd->Tex != Textures[cp.Tex]) {
      if (cp.Tex >= Textures.size()) continue;
      cmd = &dl.NewCommand();
      cmd->Tex = Textures[cp.Tex];
    }

    if (flags & LiTextFlags_Shaddow) {
      Rect rec = Math::PrimRect(rpos + off + fvec2(1, cp.Offset * cfs + 1),
                                cp.Size * cfs, 0.f);
      dl.PrimQuad(*cmd, rec, cp.SimpleUV, 0xff111111);
    }

    Rect rec = Math::PrimRect(rpos + off + fvec2(0, cp.Offset * cfs),
                              cp.Size * cfs, 0.f);
    dl.PrimQuad(*cmd, rec, cp.SimpleUV, color);

    off.x += cp.Size.x * cfs + 2 * cfs;
  }
}

PD_API void Font::CleanupTMS() {}

PD_API void Font::BakeAndPush(bool final, std::vector<u8>& font_tex,
                              int texszs) {
  auto t = PD::Gfx::LoadTexture(font_tex, texszs, texszs);
  PDLOG("Font: Texture backed as 0x{:X} at {}", t.GetID(), pCurrentTex);
  Textures.push_back(t.GetID());
  pCurrentTex = Textures.size();
}

PD_API std::string Font::pWrapText(const std::string& txt, float scale,
                                   const PD::fvec2& max, PD::fvec2& dim) {
  return "";
}

PD_API std::string Font::pShortText(const std::string& txt, float scale,
                                    const PD::fvec2& max, PD::fvec2& dim) {
  return "";
}

}  // namespace Li
}  // namespace PD