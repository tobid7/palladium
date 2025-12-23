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

#include <pd/lithium/font.hpp>

/** Due to Limitations of Shared Lib Stuff */
#ifdef PD_LITHIUM_BUILD_SHARED
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include <pd/external/stb_truetype.h>

#include <pd/lithium/renderer.hpp>

#ifdef PD_LI_INCLUDE_FONTS
#include <pd/lithium/fonts.hpp>
#endif

namespace PD {
namespace Li {
PD_LITHIUM_API void Font::LoadDefaultFont(int id, int pixel_height) {
#ifdef PD_LI_INCLUDE_FONTS
  if (id < pNumFonts) {
    auto font = pFontData[id];
    LoadTTF(std::vector<u8>(&pFontsDataRaw[font.StartOff],
                            &pFontsDataRaw[font.StartOff + font.Size]),
            pixel_height);
  }
#endif
}

PD_LITHIUM_API void Font::LoadTTF(const std::string &path, int height) {
  /**
   * Just use LoadFile2Mem which looks way cleaner
   * and helps not having the font loading code twice
   * when adding LoadTTF with mem support
   */
  TT::Scope st("LI_LoadTTF_" + path);
  auto font = PD::IO::LoadFile2Mem(path);
  LoadTTF(font, height);
}

PD_LITHIUM_API void Font::pMakeAtlas(bool final, std::vector<u8> &font_tex,
                                     int texszs, PD::Li::Texture::Ref tex) {
  auto t =
      Gfx::LoadTex(font_tex, texszs, texszs, Texture::RGBA32, Texture::LINEAR);
  tex->CopyFrom(t);
  Textures.push_back(tex);
}

PD_LITHIUM_API void Font::LoadTTF(const std::vector<u8> &data, int height) {
  /**
   * Some additional Info:
   * Removed the stbtt get bitmapbox as we dont need to place
   * the glyps nicely in the tex. next step would be using the free
   * space on the y axis to get mor glyphs inside
   */
  PixelHeight = height;
  int texszs = PD::BitUtil::GetPow2(height * 16);
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
  auto tex = Texture::New();
  fvec2 off;

  bool empty = true;

  for (u32 ii = 0x0000; ii <= 0xFFFF; ii++) {
    int gi = stbtt_FindGlyphIndex(&inf, ii);
    if (gi == 0) continue;
    if (stbtt_IsGlyphEmpty(&inf, gi)) continue;

    int w = 0, h = 0, xo = 0, yo = 0;
    unsigned char *bitmap =
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
      pMakeAtlas(false, font_tex, texszs, tex);
      tex = Texture::New();
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
    if (Gfx::Flags() & LiBackendFlags_FlipUV_Y) {
      uvs.y = 1.f - uvs.y;
      uvs.w = 1.f - uvs.w;
    }
    c.SimpleUV = uvs;
    c.Tex = tex;
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
    pMakeAtlas(true, font_tex, texszs, tex);
  }
}

PD_LITHIUM_API Font::Codepoint &Font::GetCodepoint(u32 cp) {
  // Check if codepoijt exist or return a static invalid one
  auto res = CodeMap.find(cp);
  if (res == CodeMap.end()) {
    static Codepoint invalid;
    invalid.pInvalid = true;
    return invalid;
  }
  return res->second;
}

PD_LITHIUM_API fvec2 Font::GetTextBounds(const std::string &text, float scale) {
  // Use wstring for exemple for german äöü
  auto wtext = Strings::MakeWstring(text);
  // Create a temp position and offset as [0, 0]
  fvec2 res;
  float x = 0;
  // Curent Font Scale
  float cfs = (DefaultPixelHeight * scale) / (float)PixelHeight;
  float lh = (float)PixelHeight * cfs;
  size_t index = 0;
  for (auto &it : wtext) {
    if (it == L'\0') {
      break;
    }
    index++;
    auto cp = GetCodepoint(it);
    if (cp.pInvalid && it != '\n' && it != '\t' && it != ' ') {
      continue;
    }
    switch (it) {
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
        if (index != wtext.size()) {
          x += 2 * cfs;
        }
        break;
    }
  }
  res.x = std::max(res.x, x);
  res.y += lh;
  return res;
}

PD_LITHIUM_API void Font::CmdTextEx(std::vector<Command::Ref> &cmds,
                                    const fvec2 &pos, u32 color, float scale,
                                    const std::string &text, LiTextFlags flags,
                                    const fvec2 &box) {
  fvec2 off;
  float cfs = (DefaultPixelHeight * scale) / (float)PixelHeight;
  float lh = (float)PixelHeight * cfs;
  fvec2 td;
  fvec2 rpos = pos;
  fvec2 rbox = box;
  std::string txt = text;
  if (flags & LiTextFlags_Wrap) {
    txt = pWrapText(txt, scale, box, td);
  }
  if (flags & (LiTextFlags_AlignMid | LiTextFlags_AlignRight)) {
    td = GetTextBounds(text, scale);
  }
  if (flags & LiTextFlags_AlignMid) {
    rpos = rbox * 0.5 - td * 0.5 + pos;
  }
  if (flags & LiTextFlags_AlignRight) {
    rpos.x = rpos.x - td.x;
  }

  std::vector<std::string> lines;
  std::istringstream iss(txt);
  std::string tmp;
  while (std::getline(iss, tmp)) {
    lines.push_back(tmp);
  }

  for (auto &it : lines) {
    if (flags & LiTextFlags_NoOOS) {
      if (rpos.y + off.y + lh < 0) {
        off.y += lh;
        continue;
      }
      if (rpos.y + off.y > box.y && box.y != 0) {
        break;
      }
    }
    if (flags & LiTextFlags_Short) {
      fvec2 tmp_dim;
      it = pShortText(it, scale, box - pos, tmp_dim);
    }
    auto wline = Strings::MakeWstring(it);
    auto cmd = Command::New();
    auto Tex = GetCodepoint(wline[0]).Tex;
    cmd->Tex = Tex;
    for (auto &jt : wline) {
      auto cp = GetCodepoint(jt);
      if ((cp.pInvalid && jt != L' ' && jt != L'\n' && jt != L'\t') &&
          jt != L'\r') {
        continue;
      }
      if (Tex != cp.Tex) {
        cmds.push_back(std::move(cmd));
        cmd = Command::New();
        Tex = cp.Tex;
        cmd->Tex = Tex;
      }
      if (jt == L'\t') {
        off.x += 16 * cfs;
      } else {
        if (jt != L' ') {
          if (flags & LiTextFlags_Shaddow) {
            // Draw
            Rect rec = Renderer::PrimRect(
                rpos + vec2(off.x + 1, off.y + (cp.Offset * cfs)) + 1,
                cp.Size * cfs, 0.f);
            Renderer::CmdQuad(cmd.get(), rec, cp.SimpleUV, 0xff111111);
          }
          // Draw
          Rect rec = Renderer::PrimRect(
              rpos + off + fvec2(0, (cp.Offset * cfs)), cp.Size * cfs, 0.f);
          Renderer::CmdQuad(cmd.get(), rec, cp.SimpleUV, color);
          off.x += cp.Size.x * cfs + 2 * cfs;
        } else {
          off.x += 4 * cfs;
        }
      }
    }
    cmds.push_back(std::move(cmd));
    off.y += lh;
    off.x = 0;
  }
}

PD_LITHIUM_API std::string Font::pWrapText(const std::string &txt, float scale,
                                           const PD::fvec2 &max,
                                           PD::fvec2 &dim) {
  std::string ret;
  std::string line;
  int lx = 0;
  std::stringstream s(txt);
  std::string tmp;
  // Simply go over every word
  while (s >> tmp) {
    auto d = GetTextBounds(tmp, scale);
    if (lx + d.x <= max.x) {
      line += tmp + ' ';
      lx += d.x;
    } else {
      ret += line + '\n';
      line = tmp + ' ';
      lx = GetTextBounds(line, scale).x;
    }
  }
  ret += line;
  dim = GetTextBounds(ret, scale);
  return ret;
}

PD_LITHIUM_API std::string Font::pShortText(const std::string &txt, float scale,
                                            const PD::fvec2 &max,
                                            PD::fvec2 &dim) {
  auto test = GetTextBounds(txt, scale);
  if (test.x < max.x) {
    return txt;
  }
  std::string ext;
  std::string ph = "(...)";  // placeholder
  std::string tmp = txt;
  std::string ret;
  auto maxlen = max.x;
  size_t ext_ = tmp.find_last_of('.');
  if (ext_ != tmp.npos) {
    ext = tmp.substr(ext_);
    tmp = tmp.substr(0, ext_);
  }
  maxlen -= GetTextBounds(ext, scale).x;
  maxlen -= GetTextBounds(ph, scale).x;

  for (auto &it : tmp) {
    if (GetTextBounds(ret, scale).x > maxlen) {
      ret += ph;
      ret += ext;
      dim = GetTextBounds(ret, scale);
      return ret;
    }
    ret += it;
  }
  return ret;
}

}  // namespace Li
}  // namespace PD