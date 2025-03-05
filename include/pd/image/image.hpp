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

#include <pd/core/common.hpp>

namespace PD {
class Image {
 public:
  Image() = default;
  Image(const std::string& path) { this->Load(path); }
  Image(const std::vector<u8>& buf) { this->Load(buf); }
  Image(const std::vector<u8>& buf, int w, int h, int fmt = 4) {
    this->Copy(buf, w, h, fmt);
  }
  ~Image() = default;

  void Load(const std::string& path);
  void Load(const std::vector<u8>& buf);
  void Copy(const std::vector<u8>& buf, int w, int h, int fmt = 4);

  std::vector<u8>& GetBuffer() { return buffer; }
  std::vector<u8> GetBuffer() const { return buffer; }

  int Width() const { return w; }
  int Height() const { return h; }

  u8& operator[](int idx) { return buffer[idx]; }
  u8 operator[](int idx) const { return buffer[idx]; }

  // Probably these make th eabove ones useless

  operator std::vector<u8>&() { return buffer; }
  operator std::vector<u8>() const { return buffer; }

 private:
  std::vector<u8> buffer;
  int w = 0;
  int h = 0;
  int fmt = 0;
};
}  // namespace PD