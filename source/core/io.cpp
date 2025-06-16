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

PD_CORE_API void DecompressRLE16(std::vector<PD::u8>& data) {
  std::vector<PD::u8> cpy = data;
  data.clear();
  for (size_t i = 0; i < cpy.size(); i += 3) {
    for (size_t j = 0; j < cpy[i + 2]; j++) {
      data.push_back(cpy[i]);
      data.push_back(cpy[i + 1]);
    }
  }
}

PD_CORE_API void DecompressRLE32(std::vector<PD::u8>& data) {
  std::vector<PD::u8> cpy = data;
  data.clear();
  for (size_t i = 0; (i + 4) < cpy.size(); i += 5) {
    for (size_t j = 0; j < cpy[i + 4]; j++) {
      data.push_back(cpy[i]);
      data.push_back(cpy[i + 1]);
      data.push_back(cpy[i + 2]);
      data.push_back(cpy[i + 3]);
    }
  }
}

PD_CORE_API void CompressRLE16(std::vector<PD::u8>& data) {
  std::vector<PD::u8> cpy = data;
  data.clear();
  size_t i = 0;
  while ((i + 1) < cpy.size()) {
    PD::u16 v = PD::u16(cpy[i]) | (PD::u16(cpy[i + 1]) << 8);
    size_t c = 1;
    while ((i + c * 2 + 1) < cpy.size() &&
           (PD::u16(cpy[i + c * 2]) | (PD::u16(cpy[i + c * 2 + 1])) << 8) ==
               v &&
           c < 255) {
      c++;  // c++ ...
    }
    data.push_back(PD::u8(v & 0xFF));
    data.push_back(PD::u8((v >> 8) & 0xFF));
    data.push_back(c);
    i += c * 2;
  }
}

PD_CORE_API void CompressRLE32(std::vector<PD::u8>& data) {
  std::vector<PD::u8> cpy = data;
  data.clear();
  size_t i = 0;
  while ((i + 3) < cpy.size()) {
    PD::u32 v = PD::u32(cpy[i]) | (PD::u32(cpy[i + 1]) << 8) |
                (PD::u32(cpy[i + 2]) << 16) | (PD::u32(cpy[i + 3]) << 24);
    size_t c = 1;
    while ((i + c * 4 + 3) < cpy.size() &&
           (PD::u32(cpy[i + c * 4]) | (PD::u32(cpy[i + c * 4 + 1]) << 8) |
            (PD::u32(cpy[i + c * 4 + 2]) << 16) |
            (PD::u32(cpy[i + c * 4 + 3]) << 24)) == v &&
           c < 255) {
      c++;  // c++ ...
    }
    data.push_back(PD::u8(v & 0xFF));
    data.push_back(PD::u8((v >> 8) & 0xFF));
    data.push_back(PD::u8((v >> 16) & 0xFF));
    data.push_back(PD::u8((v >> 24) & 0xFF));
    data.push_back(c);
    i += c * 4;
  }
}

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

PD_CORE_API void DecompressRLE_Ex(std::vector<u8>& data) {
  if (!data.size()) {
    return;
  }
  u8 fmt = data[0];
  data.erase(data.begin());
  if (fmt == 0) {
    DecompressRLE(data);
  } else if (fmt == 1) {
    DecompressRLE16(data);
  } else if (fmt == 2) {
    DecompressRLE32(data);
  }
  /** unknown returns input data */
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

PD_CORE_API void CompressRLE_Ex(std::vector<u8>& data) {
  if (data.empty()) {
    /** No exceptions enabled :( */
    return;
  }
  std::vector<u8> _8 = data;
  std::vector<u8> _16 = data;
  std::vector<u8> _32 = data;
  CompressRLE(_8);
  CompressRLE16(_16);
  CompressRLE32(_32);
  if (_16.size() < _8.size() && _16.size() < _32.size()) {
    _16.insert(_16.begin(), 1);
    data = _16;
    return;
  } else if (_32.size() < _8.size()) {
    _32.insert(_32.begin(), 2);
    data = _32;
    return;
  }
  _8.insert(_8.begin(), 0);
  data = _8;
}
}  // namespace IO
}  // namespace PD