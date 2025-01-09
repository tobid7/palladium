/*
MIT License

Copyright (c) 2024 tobid7

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
#include <pd/external/stb_image.h>

#include <pd/common/timetrace.hpp>
#include <pd/graphics/texture.hpp>
#include <pd/maths/bit_util.hpp>
#include <pd/maths/img_convert.hpp>
#include <pd/common/error.hpp>

namespace PD {
GPU_TEXCOLOR GetTexFmt(Texture::Type type) {
  if (type == Texture::RGBA32)
    return GPU_RGBA8;
  else if (type == Texture::RGB24)
    return GPU_RGB8;
  else if (type == Texture::A8)
    return GPU_A8;
  return GPU_RGBA8;  // Default
}
int GetBPP(Texture::Type type) {
  if (type == Texture::RGBA32)
    return 4;
  else if (type == Texture::RGB24)
    return 3;
  else if (type == Texture::A8)
    return 1;
  return 0;  // Error
}
void Texture::MakeTex(std::vector<u8>& buf, int w, int h, Texture::Type type,
                      Filter filter) {
  // Don't check here as check done before
  int bpp = GetBPP(type);
  if (bpp == 4) {
    // RGBA -> Abgr
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int pos = (x + y * w) * bpp;
        auto r = buf[pos + 0];
        auto g = buf[pos + 1];
        auto b = buf[pos + 2];
        auto a = buf[pos + 3];
        buf[pos + 0] = a;
        buf[pos + 1] = b;
        buf[pos + 2] = g;
        buf[pos + 3] = r;
      }
    }
  } else if (bpp == 3) {
    // RGBA -> Abgr
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int pos = (x + y * w) * bpp;
        auto r = buf[pos + 0];
        auto g = buf[pos + 1];
        auto b = buf[pos + 2];
        buf[pos + 0] = b;
        buf[pos + 1] = g;
        buf[pos + 2] = r;
      }
    }
  }

  vec2 tex_size(w, h);
  // Pow2
  if (!PD::BitUtil::IsSingleBit(w)) {
    tex_size.x() = PD::BitUtil::GetPow2((unsigned int)w);
  }
  if (!PD::BitUtil::IsSingleBit(h)) {
    tex_size.y() = PD::BitUtil::GetPow2((unsigned int)h);
  }

  this->size.x() = (u16)w;
  this->size.y() = (u16)h;
  this->uv.x() = 0.0f;
  this->uv.y() = 1.0f;
  this->uv.z() = ((float)w / (float)tex_size.x());
  this->uv.w() = 1.0 - ((float)h / (float)tex_size.y());

  // Texture Setup
  auto fltr = (filter == NEAREST ? GPU_NEAREST : GPU_LINEAR);
  auto tex_fmt = GetTexFmt(type);
  tex = new C3D_Tex;
  C3D_TexInit(tex, (u16)tex_size.x(), (u16)tex_size.y(), tex_fmt);
  C3D_TexSetFilter(tex, fltr, fltr);

  memset(tex->data, 0, tex->size);

  if (bpp == 3 || bpp == 4) {
    for (int x = 0; x < w; x++) {
      for (int y = 0; y < h; y++) {
        int dst_pos = ((((y >> 3) * ((int)tex_size.x() >> 3) + (x >> 3)) << 6) +
                       ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) |
                        ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3))) *
                      bpp;
        int src_pos = (y * w + x) * bpp;

        memcpy(&((u8*)tex->data)[dst_pos], &buf[src_pos], bpp);
      }
    }
    C3D_TexFlush(tex);
  } else if (bpp == 1) {
    C3D_TexLoadImage(tex, buf.data(), GPU_TEXFACE_2D, 0);
  }

  tex->border = 0x00000000;
  C3D_TexSetWrap(tex, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
}

void Texture::Delete() {
  if (tex) {
    C3D_TexDelete(tex);
    delete tex;
    tex = nullptr;
    size = vec2();
    uv = vec4(0.f, 1.f, 1.f, 0.f);
  }
}

void Texture::LoadFile(const std::string& path) {
  PD::TT::Scope st("texldr-" + path);
  Delete();
  int w = 0, h = 0, c = 0;
  u8* image = stbi_load(path.c_str(), &w, &h, &c, 4);
  PD::Assert(image != nullptr, "Unable to load image: " + path);
  if (w > 1024 || h > 1024) {
    stbi_image_free(image);
    PD::Error("Width or heigt is > 1024");
    return;
  }
  std::vector<u8> buf;
  if (c == 3) {
    stbi_image_free(image);
    image = stbi_load(path.c_str(), &w, &h, &c, 3);
    buf.resize(w * h * 4);
    PD::ImgConvert::RGB24toRGBA32(
        buf, std::vector<u8>(image, image + (w * h * 3)), w, h);
  } else {
    buf.assign(image, image + (w * h * 4));
    stbi_image_free(image);
  }
  MakeTex(buf, w, h);
}

void Texture::LoadMemory(const std::vector<u8>& data) {
  Delete();
  int w, h, c;
  u8* image = stbi_load_from_memory(data.data(), data.size(), &w, &h, &c, 4);
  if (image == nullptr) {
    return;
  }
  if (w > 1024 || h > 1024) {
    stbi_image_free(image);
    return;
  }
  std::vector<u8> buf;
  if (c == 3) {
    stbi_image_free(image);
    image = stbi_load_from_memory(data.data(), data.size(), &w, &h, &c, 3);
    buf.resize(w * h * 4);
    PD::ImgConvert::RGB24toRGBA32(
        buf, std::vector<u8>(image, image + (w * h * 3)), w, h);
  } else {
    buf.assign(image, image + (w * h * 4));
    stbi_image_free(image);
  }
  MakeTex(buf, w, h);
}

void Texture::LoadPixels(const std::vector<u8>& pixels, int w, int h, Type type,
                         Filter filter) {
  Delete();
  int bpp = GetBPP(type);
  if (w * h * bpp != (int)pixels.size()) {
    return;
  }
  std::vector<u8> cpy(pixels);
  MakeTex(cpy, w, h, type, filter);
}
}  // namespace PD