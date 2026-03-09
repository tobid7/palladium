#pragma once

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

#include <pd/core/common.hpp>

namespace PD {
/**
 * FNV-1a 32Bit hasing function
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
constexpr u32 FNV1A32(std::string_view str) {
  u32 ret = 0x811c9dc5;  // Offset basis
  for (auto& it : str) {
    ret ^= it;
    ret *= 0x01000193;  // Prime
  }
  return ret;
}
/**
 * FNV-1a 64Bit hasing function
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
constexpr u64 FNV1A64(std::string_view str) {
  u64 ret = 0xcbf29ce484222325;  // Offset basis
  for (auto& it : str) {
    ret ^= it;
    ret *= 0x00000100000001b3;  // Prime
  }
  return ret;
}

namespace Detail {
template <typename T>
struct FNV1A {};

template <>
struct FNV1A<u32> {
  static constexpr u32 Hash(std::string_view str) { return FNV1A32(str); }
  static constexpr u32 Hash(const std::string& str) { return FNV1A32(str); }
};

template <>
struct FNV1A<u64> {
  static constexpr u64 Hash(std::string_view str) { return FNV1A64(str); }
  static constexpr u64 Hash(const std::string& str) { return FNV1A64(str); }
};
}  // namespace Detail
}  // namespace PD