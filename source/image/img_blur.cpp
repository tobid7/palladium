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

#include <cstring>
#include <memory>
#include <pd/image/img_blur.hpp>
#include <pd/image/img_convert.hpp>

namespace PD {
namespace ImgBlur {
PD_IMAGE_API std::vector<float> GaussianKernel(int r, float si) {
  /// Define radius as r to be shorter
  int size = 2 * r + 1;
  std::vector<float> kernel(size);
  float sum = 0.0f;

  for (int i = -r; i <= r; i++) {
    kernel[i + r] = exp(-0.5f * (i * i) / (si * si));
    sum += kernel[i + r];
  }

  for (int i = 0; i < size; i++) {
    kernel[i] /= sum;
  }
  return kernel;
}
PD_IMAGE_API void GaussianBlur(std::vector<u8>& buf, int w, int h, float radius,
                               float si,
                               std::function<int(int, int, int)> idxfn) {
  GaussianBlur(buf.data(), w, h, 4, radius, si, idxfn);
}

PD_IMAGE_API void GaussianBlur(void* buf, int w, int h, int bpp, float radius,
                               float si,
                               std::function<int(int, int, int)> idxfn) {
  if (bpp != 4 && bpp != 3) {
    return;
  }
  std::vector<float> kernel = GaussianKernel(radius, si);
  int hks = kernel.size() / 2;
  int end = w * h * bpp;
  std::vector<unsigned char> res((u8*)buf, ((u8*)buf) + end);
  ImgConvert::Reverse32(res, w, h);
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float r = 0.0f, g = 0.0f, b = 0.0f;
      for (int ky = -hks; ky <= hks; ky++) {
        for (int kx = -hks; kx <= hks; kx++) {
          int xoff = std::min(std::max(x + kx, 0), w - 1);
          int yoff = std::min(std::max(y + ky, 0), h - 1);
          int idx = idxfn(xoff, yoff, w) * 4;

          float weight = kernel[ky + hks] * kernel[kx + hks];
          r += ((u8*)buf)[idx] * weight;
          g += ((u8*)buf)[idx + 1] * weight;
          b += ((u8*)buf)[idx + 2] * weight;
        }
      }
      int idx = idxfn(x, y, w) * bpp;
      res[idx] = std::min(std::max(int(r), 0), 255);
      res[idx + 1] = std::min(std::max(int(g), 0), 255);
      res[idx + 2] = std::min(std::max(int(b), 0), 255);
    }
  }
  ImgConvert::Reverse32(res, w, h);
  std::memcpy(buf, res.data(), res.size());
}
}  // namespace ImgBlur
}  // namespace PD