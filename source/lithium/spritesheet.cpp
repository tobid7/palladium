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

#include <pd/core/io.hpp>
#include <pd/app/error.hpp>
#include <pd/lithium/spritesheet.hpp>

namespace PD {
SpriteSheet::~SpriteSheet() { textures.clear(); }

void SpriteSheet::LoadFile(const std::string& path) {
  auto file = IO::LoadFile2Mem(path);
  if (file.size() == 0) {
    Error("Unable to load file:\n" + path);
  }
  C3D_Tex* tex = new C3D_Tex;
  auto t3x =
      Tex3DS_TextureImport(file.data(), file.size(), tex, nullptr, false);
  if (!t3x) {
    Error("Unable to import:\n" + path);
  }
  tex->border = 0;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
  C3D_TexSetFilter(tex, GPU_LINEAR, GPU_NEAREST);
  textures.reserve(Tex3DS_GetNumSubTextures(t3x) + 1);
  for (int i = 0; i < (int)Tex3DS_GetNumSubTextures(t3x); i++) {
    auto t = Texture::New();
    auto st = Tex3DS_GetSubTexture(t3x, i);
    LI::Rect uv(vec2(st->left, st->top), vec2(st->right, st->top),
                vec2(st->left, st->bottom), vec2(st->right, st->bottom));
    if (st->top < st->bottom) {
      uv.SwapVec2XY();
    }
    t->LoadExternal(tex, vec2(st->width, st->height), uv);
    textures.push_back(t);
  }
}

Texture::Ref SpriteSheet::Get(int idx) {
  if (idx >= (int)textures.size()) {
    Error("Trying to Access Texture " + std::to_string(idx + 1) + " of " +
          std::to_string(NumTextures()));
  }
  return textures[idx];
}

int SpriteSheet::NumTextures() const { return textures.size(); }
}  // namespace PD