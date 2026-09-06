#include <pd/image/convert.hpp>
#include <pd/image/image.hpp>

#if defined(PD_INCLUDE_STB_IMAGE)
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace PD {
PD_API Image::Image() {}

PD_API Image::Image(const std::string& path) { Load(path); }

PD_API Image::Image(const std::vector<u8>& buf) { Load(buf); }

PD_API Image::Image(const std::vector<u8>& pixels, int w, int h, int bpp) {
  Copy(pixels, w, h, bpp);
}

PD_API Image::~Image() {}

PD_API void Image::Load(const u8* buf, size_t size) {
  int w = 0, h = 0, c = 0;
  u8* img = stbi_load_from_memory(buf, size, &w, &h, &c, 4);
  if (c == 3) {
    stbi_image_free(img);
    img = stbi_load_from_memory(buf, size, &w, &h, &c, 3);
    pFormat = Format::RGB;
  }
  pData.assign(img, img + (w * h * c));
  pSize = ivec2(w, h);
  stbi_image_free(img);
}

PD_API void Image::Load(const std::string& path) {
  int w = 0, h = 0, c = 0;
  u8* img = stbi_load(path.c_str(), &w, &h, &c, 4);
  if (c == 3) {
    stbi_image_free(img);
    img = stbi_load(path.c_str(), &w, &h, &c, 3);
    pFormat = Format::RGB;
  }
  pData.assign(img, img + (w * h * c));
  pSize = ivec2(w, h);
  stbi_image_free(img);
}

PD_API void Image::Load(const std::vector<u8>& buf) {
  Load(buf.data(), buf.size());
}

PD_API void Image::Copy(const std::vector<u8>& pixels, int w, int h, int bpp) {
  pData = pixels;
  pSize = ivec2(w, h);
  pFormat = GuessFmtFromBpp(bpp);
}

PD_API void Image::Convert(Format dst) {
  if (pFormat == dst) {
    return;
  } else if (pFormat == Format::RGB && dst == Format::BGR) {
    ImgConvert::ReverseBuf(pData, 3, pSize.x, pSize.y);
    pFormat = Format::BGR;
  } else if (pFormat == Format::RGB && dst == Format::RGBA) {
    std::vector<PD::u8> cpy = pData;
    pData.resize(pSize.x * pSize.y * 4);
    ImgConvert::RGB24toRGBA32(pData, cpy, pSize.x, pSize.y);
    pFormat = Format::RGBA;
  } else if (pFormat == Format::RGBA && dst == Format::RGB) {
    std::vector<PD::u8> cpy = pData;
    pData.resize(pSize.x * pSize.y * 3);
    ImgConvert::RGB32toRGBA24(pData, cpy, pSize.x, pSize.y);
    pFormat = Format::RGB;
  } else if (pFormat == Format::RGBA && dst == Format::BGRA) {
    for (int i = 0; i < (pSize.x * pSize.y * 4); i += 4) {
      u8 _tmp = pData[i + 0];
      pData[i + 0] = pData[i + 2];
      pData[i + 2] = _tmp;
    }
  } else if (pFormat == Format::RGBA && dst == Format::RGB565) {
    Convert(Format::RGB);
    Convert(Format::RGB565);
  } else if (pFormat == Format::RGB && dst == Format::RGB565) {
    auto f = [](u8 r, u8 g, u8 b) -> u16 {
      u16 _r = (r >> 3);
      u16 _g = (g >> 2);
      u16 _b = (b >> 3);
      return (_r << 11) | (_g << 5) | _b;
    };
    std::vector<PD::u8> cpy = pData;
    pData.resize(pSize.x * pSize.y * 2);
    for (int y = 0; y < pSize.x; y++) {
      for (int x = 0; x < pSize.y; x++) {
        int src = (y * pSize.x + x) * 3;
        int dst = (y * pSize.x + x) * 2;
        u16 new_px = f(cpy[src + 0], cpy[src + 1], cpy[src + 2]);
        pData[dst + 0] = new_px >> 8;
        pData[dst + 1] = new_px & 0xff;
      }
    }
    pFormat = Format::RGB565;
  }
}

PD_API int Image::Format2Bpp(Format fmt) {
  switch (fmt) {
    case Format::RGBA:
    case Format::ABGR:
    case Format::BGRA:
      return 4;
    case Format::BGR:
    case Format::RGB:
      return 3;
    case Format::RGB565:
      return 2;
  }
  return 0;
}

PD_API Image::Format Image::GuessFmtFromBpp(int bpp) {
  /** Only return defaults here */
  switch (bpp) {
    case 4:
      return Format::RGBA;
    case 3:
      return Format::RGB;
    case 2:
      return Format::RGB565;
    default:
      return Format::RGBA;
  }
}

PD_API void Image::Flip(bool hz, bool vt) {
  auto bpp = Format2Bpp(pFormat);
  int rlen = pSize.x * bpp;  // calculate as less as possible
  if (hz) {
    for (int j = 0; j < pSize.y; j++) {
      int roff = j * rlen;
      for (int i = 0; i < pSize.x / 2; i++) {
        int src = roff + (i * bpp);
        int dst = roff + (pSize.x - 1 - i) * bpp;
        for (int k = 0; k < bpp; k++) {
          PD::u8 tmp = pData[dst + k];
          pData[dst + k] = pData[src + k];
          pData[src + k] = tmp;
        }
      }
    }
  }
  if (vt) {
    for (int j = 0; j < pSize.y / 2; j++) {
      int rsrc = j * rlen;
      int rdst = (pSize.y - 1 - j) * rlen;
      for (int i = 0; i < rlen; i++) {  // swap the entire row
        PD::u8 tmp = pData[rdst + i];
        pData[rdst + i] = pData[rsrc + i];
        pData[rsrc + i] = tmp;
      }
    }
  }
}

}  // namespace PD