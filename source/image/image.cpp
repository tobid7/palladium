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
  // TODO
}

}  // namespace PD