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

#include <array>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <filesystem>  // Requires C++ 17 or later
#include <format>      // Requires C++ 20 or later
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numbers>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

/** Dynamic Lib loading */
#include <pd/pd_p_api.hpp>

/** Memory Management */

#define PD_SHARED(x)                                         \
  using Ref = std::shared_ptr<x>;                            \
  template <typename... Args>                                \
  static Ref New(Args&&... args) {                           \
    return std::make_shared<x>(std::forward<Args>(args)...); \
  }

#define PD_RAW(x)                           \
  using Ref = x*;                           \
  template <typename... Args>               \
  static Ref New(Args&&... args) {          \
    x* v = new x;                           \
    new (v) x(std::forward<Args>(args)...); \
    return v;                               \
  }                                         \
  static void Delete(Ref ref) { delete ref; }

#define PD_UNIQUE(x)                                         \
  using Ref = std::unique_ptr<x>;                            \
  template <typename... Args>                                \
  static Ref New(Args&&... args) {                           \
    return std::make_unique<x>(std::forward<Args>(args)...); \
  }

#define PD_BIT(x) (1 << x)

namespace PD {
[[noreturn]] inline void Throw(const std::string& str) {
#ifdef _EXCEPTIONS
  throw std::runtime_error("PD Error " + str);
#else
  std::cout << "PD Error " << str << std::endl;
  std::abort();
#endif
}
/** Types */
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
}  // namespace PD