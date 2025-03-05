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

#include <pd/external/stb_image.h>

#include <cstring>
#include <memory>
#include <pd/image/image.hpp>
#include <pd/image/img_convert.hpp>

namespace PD {
void Image::Load(const std::string& path) {
  u8* img = stbi_load(path.c_str(), &w, &h, &fmt, 4);
  if (fmt == 3) {
    stbi_image_free(img);
    img = stbi_load(path.c_str(), &w, &h, &fmt, 3);
    buffer.resize(w * h * 4);
    PD::ImgConvert::RGB24toRGBA32(
        buffer, std::vector<u8>(img, img + (w * h * 3)), w, h);
  } else {
    buffer.assign(img, img + (w * h * 4));
    stbi_image_free(img);
  }
}
void Image::Load(const std::vector<u8>& buf) {
  u8* img = stbi_load_from_memory(buf.data(), buf.size(), &w, &h, &fmt, 4);
  if (fmt == 3) {
    stbi_image_free(img);
    img = stbi_load_from_memory(buf.data(), buf.size(), &w, &h, &fmt, 3);
    buffer.resize(w * h * 4);
    PD::ImgConvert::RGB24toRGBA32(
        buffer, std::vector<u8>(img, img + (w * h * 3)), w, h);
  } else {
    buffer.assign(img, img + (w * h * 4));
    stbi_image_free(img);
  }
}
void Image::Copy(const std::vector<u8>& buf, int w, int h, int fmt) {}
}  // namespace PD