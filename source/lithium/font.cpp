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

#include <3ds.h>
#include <pd/external/stb_truetype.h>

#include <pd/core/bit_util.hpp>
#include <pd/core/io.hpp>
#include <pd/core/strings.hpp>
#include <pd/core/sys.hpp>
#include <pd/lithium/font.hpp>

namespace PD {
namespace LI {
void Font::LoadTTF(const std::string &path, int height) {
  sysfont = false;  // Not using System Font
  TT::Scope st("LI_LoadTTF_" + path);
  pixel_height = height;  // Set internel pixel height
  // Use NextPow2 to be able to use sizes between for example 16 and 32
  // before it only was possible to use 8, 16, 32, 64 as size
  int texszs = BitUtil::GetPow2(height * 16);
  // Load stbtt
  stbtt_fontinfo inf;
  std::ifstream loader(path, std::ios::binary);
  if (!loader.is_open()) return;
  loader.seekg(0, std::ios::end);
  size_t len = loader.tellg();
  loader.seekg(0, std::ios::beg);
  unsigned char *buffer = new unsigned char[len];
  loader.read(reinterpret_cast<char *>(buffer), len);
  loader.close();
  stbtt_InitFont(&inf, buffer, 0);
  std::vector<unsigned char> font_tex(texszs * texszs);  // Create font Texture
  float scale = stbtt_ScaleForPixelHeight(&inf, pixel_height);

  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&inf, &ascent, &descent, &lineGap);
  int baseline = static_cast<int>(ascent * scale);

  std::map<u32, int> buf_cache;  // Cache to not render same codepoint tex twice

  /// Load Codepoints
  auto tex = Texture::New();
  vec2 off;
  for (u32 ii = 0x0000; ii < 0xFFFF; ii++) {
    int i = stbtt_FindGlyphIndex(&inf, ii);
    if (i == 0) {
      continue;
    }
    if (stbtt_IsGlyphEmpty(&inf, i)) {
      continue;
    }

    Codepoint c;
    int w = 0, h = 0, xo = 0, yo = 0;
    unsigned char *bitmap =
        stbtt_GetCodepointBitmap(&inf, scale, scale, i, &w, &h, &xo, &yo);
    int x0, y0, x1, y1;
    stbtt_GetCodepointBitmapBox(&inf, i, scale, scale, &x0, &y0, &x1, &y1);

    // Check if Codepoint exists as hash and if it is use its already written
    // data
    u32 hashed_map = IO::HashMemory(std::vector<u8>(bitmap, bitmap + (w * h)));
    if (buf_cache.find(hashed_map) != buf_cache.end()) {
      c = GetCodepoint(buf_cache[hashed_map]);
      c.cp(i);
      cpmap[i] = c;
      free(bitmap);
      continue;
    } else {
      buf_cache[hashed_map] = i;
    }

    if (off[0] + w > texszs) {
      off[1] += pixel_height;
      off[0] = 0;
    }

    // Set UV Data
    vec4 uvs;
    uvs[0] = static_cast<float>(off.x() / (float)texszs);
    uvs[1] = static_cast<float>(1.f - (off.y() / (float)texszs));
    uvs[2] = static_cast<float>((float)(off.x() + w) / (float)texszs);
    uvs[3] = static_cast<float>(1.f - (float)(off.y() + h) / (float)texszs);
    c.uv(uvs);

    c.tex(tex);
    c.size(vec2(w, h));
    c.off(baseline + yo);

    // Render glyph
    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        int map_pos = ((off[1] + y) * texszs + (off[0] + x));
        font_tex[map_pos] = bitmap[x + y * w];
      }
    }

    free(bitmap);
    cpmap[i] = c;

    // Small Patch to avoid some possible artifacts
    off[0] += w + 1;
    if (off[0] + w > texszs) {
      off[1] += pixel_height;
      if (off[1] + pixel_height > texszs) {
        break;
      }
      off[0] = 0;
    }
  }
  // Load the Texture and append to list
  tex->LoadPixels(font_tex, texszs, texszs, Texture::A8, Texture::LINEAR);
  textures.push_back(tex);
}

Font::Codepoint &Font::GetCodepoint(u32 cp) {
  // Check if codepoijt exist or return a static invalid one
  auto res = cpmap.find(cp);
  if (res == cpmap.end()) {
    static Codepoint invalid;
    return invalid.invalid(true);
  }
  return res->second;
}

void Font::LoadSystemFont() {
  TT::Scope st("LI_SystemFont");  // Trace loading time
  sysfont = true;                 // Set as System Font
  fontEnsureMapped();             // Call this to be sure the font is mapped
  // Get some const references for system font loading
  const auto fnt = fontGetSystemFont();
  const auto fnt_info = fontGetInfo(fnt);
  const auto glyph_info = fontGetGlyphInfo(fnt);
  // Resize the Texture list by the num of sysfont textures
  this->textures.resize(glyph_info->nSheets + 1);
  /// Modify the Pixel Height by 1.1f to fit the
  /// Size og ttf font Rendering
  pixel_height = glyph_info->cellHeight * 1.1f;
  // Load the Textures and make sure they don't auto unload
  for (size_t i = 0; i < glyph_info->nSheets; i++) {
    auto stex = Texture::New();
    auto tx = new C3D_Tex;
    tx->data = fontGetGlyphSheetTex(fnt, i);
    tx->fmt = (GPU_TEXCOLOR)glyph_info->sheetFmt;
    tx->size = glyph_info->sheetSize;
    tx->width = glyph_info->sheetWidth;
    tx->height = glyph_info->sheetHeight;
    tx->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) |
                GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) |
                GPU_TEXTURE_WRAP_S(GPU_REPEAT) | GPU_TEXTURE_WRAP_T(GPU_REPEAT);
    tx->border = 0xffffffff;
    tx->lodParam = 0;
    stex->LoadExternal(tx, vec2(tx->width, tx->height), vec4(0, 1, 1, 0));
    stex->AutoUnLoad(false);
    textures[i] = stex;
  }
  std::vector<unsigned int> charSet;
  // Write the Charset into a vector
  for (auto cmap = fnt_info->cmap; cmap; cmap = cmap->next) {
    if (cmap->mappingMethod == CMAP_TYPE_DIRECT) {
      if (cmap->codeEnd >= cmap->codeBegin) {
        charSet.reserve(charSet.size() + cmap->codeEnd - cmap->codeBegin + 1);
        for (auto i = cmap->codeBegin; i <= cmap->codeEnd; ++i) {
          if (cmap->indexOffset + (i - cmap->codeBegin) == 0xFFFF) break;
          charSet.emplace_back(i);
        }
      }
    } else if (cmap->mappingMethod == CMAP_TYPE_TABLE) {
      if (cmap->codeEnd >= cmap->codeBegin) {
        charSet.reserve(charSet.size() + cmap->codeEnd - cmap->codeBegin + 1);
        for (auto i = cmap->codeBegin; i <= cmap->codeEnd; ++i) {
          if (cmap->indexTable[i - cmap->codeBegin] == 0xFFFF) continue;
          charSet.emplace_back(i);
        }
      }
    } else if (cmap->mappingMethod == CMAP_TYPE_SCAN) {
      charSet.reserve(charSet.size() + cmap->nScanEntries);
      for (unsigned i = 0; i < cmap->nScanEntries; ++i) {
        if (cmap->scanEntries[i].code >= cmap->codeBegin &&
            cmap->scanEntries[i].code <= cmap->codeEnd) {
          if (cmap->scanEntries[i].glyphIndex != 0xFFFF) {
            charSet.emplace_back(cmap->scanEntries[i].code);
          }
        }
      }
    } else {
      continue;
    }
  }

  // Sort the charset and make sure all values are unique
  std::sort(charSet.begin(), charSet.end());
  charSet.erase(std::unique(charSet.begin(), charSet.end()));

  // Setup the Codepoint map by the charset
  for (auto cp : charSet) {
    int gidx = fontGlyphIndexFromCodePoint(fnt, cp);
    if (gidx >= 0xFFFF) continue;
    Codepoint codepoint;
    fontGlyphPos_s dat;
    fontCalcGlyphPos(&dat, fnt, gidx, GLYPH_POS_CALC_VTXCOORD, 1.f, 1.f);

    codepoint.cp(cp);
    codepoint.uv(vec4(dat.texcoord.left, dat.texcoord.top, dat.texcoord.right,
                      dat.texcoord.bottom));

    if (dat.sheetIndex < (int)textures.size()) {
      codepoint.tex(textures[dat.sheetIndex]);
    } else {
      codepoint.invalid(true);
    }
    codepoint.size(vec2(dat.vtxcoord.right, dat.vtxcoord.bottom));
    codepoint.off(0);
    cpmap[cp] = codepoint;
  }
}
}  // namespace LI
}  // namespace PD
