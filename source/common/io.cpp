/*
MIT License
Copyright (c) 2024 - 2025 René Amthor (tobid7)

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

#include <pd/common/io.hpp>

namespace PD {
namespace IO {
std::vector<u8> LoadFile2Mem(const std::string& path) {
  std::ifstream iff(path, std::ios::binary);
  if (!iff) {
    return std::vector<u8>();
  }
  iff.seekg(0, std::ios::end);
  size_t szs = iff.tellg();
  iff.seekg(0, std::ios::beg);
  std::vector<u8> res(szs, 0);
  iff.read(reinterpret_cast<char*>(res.data()), res.size());
  iff.close();
  return res;
}
u32 HashMemory(const std::vector<u8>& data) {
  u32 hash = 4477;
  for (auto& it : data) {
    hash = (hash * 33) + it;
  }
  return hash;
}
}  // namespace IO
}  // namespace PD