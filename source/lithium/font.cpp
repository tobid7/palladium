#include <pd/core/core.hpp>
#include <pd/drivers/gfx.hpp>
#include <pd/lithium/drawlist.hpp>
#include <pd/lithium/font.hpp>
#include <pd/lithium/math.hpp>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <map>
#include <utility>

namespace PD {
namespace Li {
PD_API void Font::LoadTTF(const std::string& path, int px_height,
                          LiFontFlags flags) {
  /**
   * Just use LoadFile2Mem which looks way cleaner
   * and helps not having the font loading code twice
   * when adding LoadTTF with mem support
   */
  PDLOG("Font: Loading {}...", path);
  TT::Scope st("LI_LoadTTF_" + path);
  auto font = PD::IO::LoadFile2Mem(path);
  PDLOG("Font Size: {}", PD::Strings::FormatBytes(font.size()));
  LoadTTF(font, px_height, flags);
}

PD_API void Font::LoadTTF(const std::vector<u8>& data, int px_height,
                          LiFontFlags flags) {
  /**
   * Some additional Info:
   * Removed the stbtt get bitmapbox as we dont need to place
   * the glyps nicely in the tex. next step would be using the free
   * space on the y axis to get mor glyphs inside
   */
  pFlags = flags;
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

  float mono_advance = 0.f;
  if (IsMonospace()) {
    int a, l;
    int rgi = stbtt_FindGlyphIndex(&inf, 'W');
    if (rgi == 0) rgi = stbtt_FindGlyphIndex(&inf, '0');
    if (rgi != 0) {
      stbtt_GetGlyphHMetrics(&inf, rgi, &a, &l);
      mono_advance = a * scale;
    }
  }

  // Cache to not render same codepoint tex twice
  std::map<u32, int> buf_cache;

  std::vector<u8> font_tex(texszs * texszs, 0);
  fvec2 off;

  bool empty = true;

  std::vector<std::pair<u32, u32>> ranges = {
      {0x0020, 0x007E},  // ASCII
      {0x00A0, 0x00FF},  // LATIN-1
  };

  for (auto& it : ranges) {
    for (u32 ii = it.first; ii <= it.second; ii++) {
      int gi = stbtt_FindGlyphIndex(&inf, ii);
      if (gi == 0) continue;

      int advance, lsb;
      stbtt_GetGlyphHMetrics(&inf, gi, &advance, &lsb);
      float sadvance = advance * scale;
      float final_advance = sadvance;
      float center_offset = 0.f;

      if (IsMonospace() && mono_advance > 0.f) {
        final_advance = mono_advance;
        center_offset = (mono_advance - sadvance) * 0.5f;
      }

      if (stbtt_IsGlyphEmpty(&inf, gi)) {
        Codepoint c;
        c.AdvanceX = final_advance;
        c.Size = fvec2(0.f);
        c.pCodepoint = ii;
        c.pInvalid = false;
        CodeMap[ii] = c;
        continue;
      }

      int w = 0, h = 0, xo = 0, yo = 0;
      unsigned char* bitmap = nullptr;
      if (IsSDF()) {
        bitmap = stbtt_GetCodepointSDF(&inf, scale, ii, 5, 128, 255.f / 5.f, &w,
                                       &h, &xo, &yo);
      } else {
        bitmap =
            stbtt_GetCodepointBitmap(&inf, scale, scale, ii, &w, &h, &xo, &yo);
      }
      if (!bitmap || w <= 0 || h <= 0) {
        if (bitmap) free(bitmap);
        continue;
      }

      u32 hashed_map =
          IO::HashMemory(std::vector<u8>(bitmap, bitmap + (w * h)));
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
      if (PD::Gfx::GetFlags() & PDGfxBackendFlags_FlipUV_Y) {
        uvs.y = 1.f - uvs.y;
        uvs.w = 1.f - uvs.w;
      }
      c.SimpleUV = uvs;
      c.Tex = pCurrentTex;
      c.Size = fvec2(w, h);
      c.Offset = fvec2(xo + center_offset, baseline + yo);
      c.AdvanceX = final_advance;
      c.pCodepoint = ii;

      for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
          int map_pos = ((static_cast<int>(off.y) + y) * texszs +
                         (static_cast<int>(off.x) + x));
          font_tex[map_pos] = bitmap[x + y * w];
        }
      }

      empty = false;
      CodeMap[ii] = c;
      free(bitmap);

      // offset by 1 (prevents visual glitches i had)
      off.x += w + 1;
    }
  }

  if (!empty) {
    BakeAndPush(true, font_tex, texszs);
  }

  for (u32 i = 0; i < 128; i++) {
    auto r = CodeMap.find(i);
    if (r == CodeMap.end()) {
      static Codepoint invalid;
      invalid.pInvalid = true;
      pAsciiCache[i] = invalid;
    } else {
      pAsciiCache[i] = CodeMap[i];
    }
  }
}

PD_API void Font::LoadDefaultFont(int id, int pixel_height) {}

PD_API Font::Codepoint& Font::GetCodepoint(u32 c) {
  if (c < 128) {
    // Direct Access (~11% improvement)
    return pAsciiCache[c];
  }
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
    if ((cp.pInvalid && c != L'\n' && c != L'\t' && c != L' ') && c != L'\r')
      continue;
    if (c == L'\n') {
      res.y += lh;
      res.x = std::max(res.x, x);
      x = 0.f;
      continue;
    }
    if (c == L'\t') {
      x += (cp.AdvanceX > 0 ? cp.AdvanceX : 16.f) * 4.f * cfs;
      continue;
    }
    x += cp.AdvanceX * cfs;
  }
  res.x = std::max(res.x, x);
  res.y += lh;
  return res;
}

PD_API void PrimTextQuad(Command& cmd, float x, float y, float w, float h,
                         const fvec4& uv, const PD::Color& color) {
  cmd.Reserve(4, 6);
  cmd.Add(2, 1, 0);
  cmd.Add(3, 2, 0);
  cmd.Add(Vertex(x, y, uv.x, uv.y, color));
  cmd.Add(Vertex(x + w, y, uv.z, uv.y, color));
  cmd.Add(Vertex(x + w, y + h, uv.z, uv.w, color));
  cmd.Add(Vertex(x, y + h, uv.x, uv.w, color));
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
  Command* cmd = dl.HasCommands() ? &dl.GetLastCommand() : nullptr;
  while (it.Decode32(c)) {
    auto cp = GetCodepoint(c);
    if ((cp.pInvalid && c != L'\n' && c != L'\t' && c != L' ') && c != L'\r')
      continue;

    if (c == L'\n') {
      off.y += lh;
      off.x = 0.f;
      continue;
    }
    if (c == L'\t') {
      off.x += (cp.AdvanceX > 0 ? cp.AdvanceX : 16.f) * 4.f * cfs;
      continue;
    }

    if (cp.Size.x > 0 && cp.Size.y > 0) {
      if (cmd == nullptr || cmd->Tex != Textures[cp.Tex]) {
        if (cp.Tex >= Textures.size()) continue;
        cmd = &dl.NewCommand();
        cmd->Tex = Textures[cp.Tex];
        cmd->SDF = IsSDF();
      }

      // calculating once and using PrimTextQuad to directly push
      // saves ~42% on raw multiline text draw time 6.3 -> 3.7 ms
      // and ~25% on the whole frametime 20.3 -> 15.2 ms
      // tested with Craftus-Next 0.8.0 commit:
      // 33298ccc276bf996d341710e69ecf05f99c57961
      float cx = rpos.x + off.x + (cp.Offset.x * cfs);
      float cy = rpos.y + off.y + (cp.Offset.y * cfs);
      float cw = cp.Size.x * cfs;
      float ch = cp.Size.y * cfs;

      if (flags & LiTextFlags_Shaddow) {
        PrimTextQuad(*cmd, cx + 1.f, cy + 1.f, cw, ch, cp.SimpleUV, 0xff111111);
      }
      PrimTextQuad(*cmd, cx, cy, cw, ch, cp.SimpleUV, color);
    }
    off.x += cp.AdvanceX * cfs;
  }
}

PD_API void Font::CleanupTMS() {}

PD_API void Font::BakeAndPush(bool final, std::vector<u8>& font_tex,
                              int texszs) {
  auto t = PD::Gfx::LoadTexture(font_tex, texszs, texszs, TextureFormat::A8);
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

PD_API void Font::Delete() {
  for (auto& it : Textures) {
    // Creating a tmp fake Li tex for deletion
    PD::Gfx::DeleteTexture(PD::Li::Texture(it, 0));
  }
  pCurrentTex = 0;
  PixelHeight = 0;
  pTMS.clear();
  CodeMap.clear();
}
}  // namespace Li
}  // namespace PD