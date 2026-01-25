#pragma once

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

#include <pd/core/core.hpp>
#include <pd/pd_p_api.hpp>

namespace PD {
class PD_API Image {
 public:
  enum Format {
    RGBA,    // bpp == 4
    RGB,     // bpp == 3
    RGB565,  // bpp == 2 (not supported in laoding)
    BGR,     // bpp == 3
    ABGR,    // bpp == 4
    BGRA,    // bpp == 4
  };
  Image() = default;
  Image(const std::string& path) { this->Load(path); }
  Image(const std::vector<u8>& buf) { this->Load(buf); }
  Image(const std::vector<u8>& buf, int w, int h, int bpp = 4) {
    this->Copy(buf, w, h, bpp);
  }
  ~Image() = default;

  PD_SHARED(Image)

  void Load(const std::string& path);
  void Load(const std::vector<u8>& buf);
  void Copy(const std::vector<u8>& buf, int w, int h, int bpp = 4);

  std::vector<PD::u8>& GetBuffer() { return pBuffer; }
  std::vector<PD::u8> GetBuffer() const { return pBuffer; }

  int Width() const { return pWidth; }
  int Height() const { return pHeight; }
  ivec2 Size() const { return ivec2(pWidth, pHeight); }
  Format Fmt() const { return pFmt; }

  void FlipVertical();
  void FlipHorizontal();

  u8& operator[](int idx) { return pBuffer[idx]; }
  u8 operator[](int idx) const { return pBuffer[idx]; }

  // Probably these make th eabove ones useless

  operator std::vector<PD::u8>&() { return pBuffer; }
  operator std::vector<PD::u8>() const { return pBuffer; }

  static void Convert(Image::Ref img, Image::Format dst);
  static void ReTile(Image::Ref img,
                     std::function<u32(int x, int y, int w)> src,
                     std::function<u32(int x, int y, int w)> dst);
  static int Fmt2Bpp(Format fmt);

  std::vector<PD::u8> pBuffer;
  int pWidth;
  int pHeight;
  Format pFmt = Format::RGBA;

 private:
  /** Leftover variable used for stbi_load */
  int fmt = 0;
};
}  // namespace PD