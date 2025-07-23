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

#include <pd/core/io.hpp>

namespace PD {
namespace IO {
PD_CORE_API std::vector<u8> LoadFile2Mem(const std::string& path) {
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

PD_CORE_API std::string LoadFile2Str(const std::string& path) {
  std::ifstream iff(path, std::ios::binary);
  if (!iff) {
    return "";
  }
  std::string ret;
  std::string line;
  while (std::getline(iff, line)) {
    ret += line;
  }
  iff.close();
  return ret;
}

PD_CORE_API u32 HashMemory(const std::vector<u8>& data) {
  u32 hash = 4477;
  for (auto& it : data) {
    hash = (hash * 33) + it;
  }
  return hash;
}

PD_CORE_API void DecompressRLE(std::vector<u8>& data) {
  if ((data.size() % 2) != 0) {
    return;
  }
  std::vector<u8> cpy = data;
  data.clear();
  for (size_t i = 0; i < cpy.size(); i += 2) {
    data.insert(data.end(), cpy[i + 1], cpy[i]);
  }
}

PD_CORE_API void CompressRLE(std::vector<u8>& data) {
  if (data.empty()) {
    /** No exceptions enabled :( */
    return;
  }
  std::vector<u8> cpy = data;
  data.clear();
  /** 8-Bit RLE */
  data.push_back(0);
  size_t i = 0;
  while (i < cpy.size()) {
    u8 v = cpy[i];
    u8 c = 1;
    while (i + c < cpy.size() && cpy[i + c] == v && c < 255) {
      c++;  // c++ ...
    }
    data.push_back(v);
    data.push_back(c);
    i += c;
  }
}
}  // namespace IO
}  // namespace PD