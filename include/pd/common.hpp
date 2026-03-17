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
#include <cmath>
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <numbers>
#include <pd/pd_p_api.hpp>
#include <string>
#include <vector>

namespace PD {
[[noreturn]] inline void Throw(const std::string& msg) {
  throw std::runtime_error(msg);
}
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using ptr = uintptr_t;
PD_API void Log(const std::string& txt);
template <typename... Args>
void Log(std::format_string<Args...> fmt, Args&&... args) {
  std::string msg = std::format(fmt, std::forward<Args>(args)...);
  Log(msg);
}
}  // namespace PD

#ifdef PD_DEBUG
#define PDLOG(fmt, ...) \
  PD::Log("[{}:{}]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define PDLOG(fmt, ...)
#endif