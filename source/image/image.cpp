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

#ifdef PD_IMAGE_BUILD_SHARED
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <pd/external/stb_image.h>

#include <cstring>
#include <memory>
#include <pd/image/image.hpp>
#include <pd/image/img_convert.hpp>

namespace PD {
PD_IMAGE_API void Image::Load(const std::string& path) {
  u8* img = stbi_load(path.c_str(), &pWidth, &pHeight, &fmt, 4);
  if (fmt == 3) {
    stbi_image_free(img);
    img = stbi_load(path.c_str(), &pWidth, &pHeight, &fmt, 3);
    pBuffer = std::vector<PD::u8>(img, img + (pWidth * pHeight * 3));
    pFmt = RGB;
    stbi_image_free(img);
  } else if (fmt == 4) {
    pBuffer = std::vector<PD::u8>(img, img + (pWidth * pHeight * 4));
    pFmt = RGBA;
    stbi_image_free(img);
  }
}
PD_IMAGE_API void Image::Load(const std::vector<u8>& buf) {
  u8* img =
      stbi_load_from_memory(buf.data(), buf.size(), &pWidth, &pHeight, &fmt, 4);
  if (fmt == 3) {
    stbi_image_free(img);
    img = stbi_load_from_memory(buf.data(), buf.size(), &pWidth, &pHeight, &fmt,
                                3);
    pBuffer = std::vector<PD::u8>(img, img + (pWidth * pHeight * 3));
    pFmt = RGB;
    stbi_image_free(img);
  } else if (fmt == 4) {
    pBuffer = std::vector<PD::u8>(img, img + (pWidth * pHeight * 4));
    stbi_image_free(img);
    pFmt = RGBA;
  }
}
PD_IMAGE_API void Image::Copy(const std::vector<u8>& buf, int w, int h,
                              int bpp) {
  this->fmt = bpp;
  if (buf.size() != (size_t)w * h * bpp) {
    // Size Error
    return;
  }
  this->pBuffer.resize(w * h * bpp);
  for (size_t i = 0; i < this->pBuffer.size(); i++) {
    pBuffer[i] = buf[i];
  }
}

PD_IMAGE_API void Image::Convert(Image::Ref img, Image::Format dst) {
  if (img->Fmt() == dst) {
    return;
  } else if (img->Fmt() == Image::RGB && dst == Image::BGR) {
    ImgConvert::ReverseBuf(img->pBuffer, 3, img->pWidth, img->pHeight);
  } else if (img->Fmt() == Image::RGB && dst == Image::RGBA) {
    std::vector<PD::u8> cpy = img->pBuffer;
    img->pBuffer.resize(img->pWidth * img->pHeight * 4);
    ImgConvert::RGB24toRGBA32(img->pBuffer, cpy, img->pWidth, img->pHeight);
  } else if (img->Fmt() == Image::RGBA && dst == Image::RGB) {
    std::vector<PD::u8> cpy = img->pBuffer;
    img->pBuffer.resize(img->pWidth * img->pHeight * 3);
    ImgConvert::RGB32toRGBA24(img->pBuffer, cpy, img->pWidth, img->pHeight);
  } else if (img->Fmt() == Image::RGBA && dst == Image::RGB565) {
    Convert(img, Image::RGB);
    Convert(img, Image::RGB565);
  } else if (img->Fmt() == Image::RGB && dst == Image::RGB565) {
    auto f = [](u8 r, u8 g, u8 b) -> u16 {
      u16 _r = (r >> 3);
      u16 _g = (g >> 2);
      u16 _b = (b >> 3);
      return (_r << 11) | (_g << 5) | _b;
    };
    std::vector<PD::u8> cpy = img->pBuffer;
    img->pBuffer.resize(img->pWidth * img->pHeight * 2);
    for (int y = 0; y < img->pWidth; y++) {
      for (int x = 0; x < img->pHeight; x++) {
        int src = (y * img->pWidth + x) * 3;
        int dst = (y * img->pWidth + x) * 2;
        u16 new_px = f(cpy[src + 0], cpy[src + 1], cpy[src + 2]);
        img->pBuffer[dst + 0] = new_px >> 8;
        img->pBuffer[dst + 1] = new_px & 0xff;
      }
    }
  }
}

PD_IMAGE_API int Image::Fmt2Bpp(Format fmt) {
  switch (fmt) {
    case RGBA:
    case ABGR:
      return 4;
      break;
    case RGB:
    case BGR:
      return 3;
      break;
    case RGB565:
      return 2;
      break;

    default:
      return 0;
      break;
  }
}

PD_IMAGE_API void Image::ReTile(Image::Ref img,
                                std::function<u32(int x, int y, int w)> src,
                                std::function<u32(int x, int y, int w)> dst) {
  std::vector<PD::u8> cpy = img->pBuffer;
  /** could use fmt here but for 565 that woulnt work as it is not supported by
   * file loading where fmt is used */
  int bpp = Fmt2Bpp(img->pFmt);
  for (int y = 0; y < img->pHeight; y++) {
    for (int x = 0; x < img->pWidth; x++) {
      int src_idx = src(x, y, img->pWidth);
      int dst_idx = dst(x, y, img->pWidth);
      for (int i = 0; i < bpp; i++) {
        img->pBuffer[dst_idx + i] = cpy[src_idx + i];
      }
    }
  }
}
}  // namespace PD