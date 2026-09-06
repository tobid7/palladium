/*
MIT License
Copyright (c) 2024 - 2026 René Amthor (tobid7)

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

#include <pd/image/img_convert.hpp>

namespace PD::ImgConvert {

PD_API void RGB24toRGBA32(std::vector<u8>& out, const std::vector<u8>& in,
                          const int& w, const int& h) {
  // Converts RGB24 to RGBA32
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 3;
      int dst = (y * w + x) * 4;
      out[dst + 0] = in[src + 0];
      out[dst + 1] = in[src + 1];
      out[dst + 2] = in[src + 2];
      out[dst + 3] = 255;
    }
  }
}

PD_API void RGB32toRGBA24(std::vector<u8>& out, const std::vector<u8>& in,
                          const int& w, const int& h) {
  // Converts RGB24 to RGBA32
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int src = (y * w + x) * 4;
      int dst = (y * w + x) * 3;
      out[dst + 0] = in[src + 0];
      out[dst + 1] = in[src + 1];
      out[dst + 2] = in[src + 2];
    }
  }
}

PD_API void Reverse32(std::vector<u8>& buf, const int& w, const int& h) {
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int i = y * w + x;
      u8 t0 = buf[i + 0];
      u8 t1 = buf[i + 1];
      buf[i + 0] = buf[i + 3];
      buf[i + 1] = buf[i + 2];
      buf[i + 3] = t0;
      buf[i + 2] = t1;
    }
  }
}

PD_API void ReverseBuf(std::vector<u8>& buf, size_t bpp, int w, int h) {
  std::vector<u8> cpy = buf;
  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      int pos = (y * w + x) * bpp;
      for (size_t i = 0; i < bpp; i++) {
        buf[pos + bpp - 1 - i] = cpy[pos + i];
      }
    }
  }
}
}  // namespace PD::ImgConvert