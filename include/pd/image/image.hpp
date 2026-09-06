#pragma once

#include <pd/core/core.hpp>

namespace PD {
class PD_API Image {
 public:
  enum class Format {
    RGBA,    // bpp == 4
    RGB,     // bpp == 3
    RGB565,  // bpp == 2 (not supported in laoding)
    BGR,     // bpp == 3
    ABGR,    // bpp == 4
    BGRA,    // bpp == 4
  };
  Image();
  Image(const std::string& path);
  Image(const std::vector<u8>& buf);
  Image(const std::vector<u8>& pixels, int w, int h, int bpp = 4);
  ~Image();

  void Load(const std::string& path);
  void Load(const std::vector<u8>& buf);
  void Load(const u8* buf, size_t size);
  void Copy(const std::vector<u8>& pixels, int w, int h, int bpp = 4);

  const int& Width() const { return pSize.x; }
  const int& Height() const { return pSize.y; }
  const ivec2& Size() const { return pSize; }

  const std::vector<u8>& data() const { return pData; }
  std::vector<u8>& data() { return pData; }
  operator const std::vector<u8>&() const { return pData; }
  operator std::vector<u8>&() { return pData; }
  operator const ivec2&() const { return pSize; }
  operator const Format&() const { return pFormat; }

  void Flip(bool horizontal = true, bool vertical = true);

  static int Format2Bpp(Format fmt);
  static Format GuessFmtFromBpp(int bpp);

  void Convert(Format dst);

 private:
  std::vector<u8> pData;
  ivec2 pSize;
  Format pFormat = Format::RGBA;
};
}  // namespace PD