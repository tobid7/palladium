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

#include <3ds.h>

#include <palladium>
#include <pd-3ds.hpp>

namespace PD {
PD::Li::Font::Ref LoadSystemFont() {
  TT::Scope st("LI_SystemFont");  // Trace loading time
  Li::Font::Ref ret = Li::Font::New();
  fontEnsureMapped();  // Call this to be sure the font is mapped
  // Get some const references for system font loading
  const auto fnt = fontGetSystemFont();
  const auto fnt_info = fontGetInfo(fnt);
  const auto glyph_info = fontGetGlyphInfo(fnt);
  // Resize the Texture list by the num of sysfont textures
  ret->Textures.resize(glyph_info->nSheets + 1);
  /// Modify the Pixel Height by 1.1f to fit the
  /// Size og ttf font Rendering
  ret->PixelHeight = glyph_info->cellHeight;
  // Load the Textures and make sure they don't auto unload
  for (size_t i = 0; i < glyph_info->nSheets; i++) {
    auto stex = Li::Texture::New();
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
    stex->Address = (Li::TexAddress)tx;
    stex->Size = fvec2(tx->width, tx->height);
    stex->UV = fvec4(0, 1, 1, 0);
    ret->Textures[i] = stex;
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
    Li::Font::Codepoint codepoint;
    fontGlyphPos_s dat;
    fontCalcGlyphPos(&dat, fnt, gidx, GLYPH_POS_CALC_VTXCOORD, 1.f, 1.f);

    codepoint.pCodepoint = cp;
    codepoint.SimpleUV = fvec4(dat.texcoord.left, dat.texcoord.top,
                               dat.texcoord.right, dat.texcoord.bottom);

    if (dat.sheetIndex < (int)ret->Textures.size()) {
      codepoint.Tex = ret->Textures[dat.sheetIndex];
    } else {
      codepoint.pInvalid = true;
    }
    codepoint.Size = fvec2(dat.vtxcoord.right, dat.vtxcoord.bottom);
    codepoint.Offset = 0;
    ret->CodeMap[cp] = codepoint;
  }
  return ret;
}

void Init(void* data) {
  // Dekstop Init Stage
  // First use default OS Driver
  PD::OS::Init();
  PD::Gfx::Init(PD::GfxC3D::New());
  PD::Hid::Init(PD::Hid3DS::New());
}
}  // namespace PD