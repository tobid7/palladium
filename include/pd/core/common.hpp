#pragma once

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

#include <chrono>
#include <cinttypes>
#include <cmath>
#include <filesystem>  // Requires C++ 17 or later
#include <format>      // Requires C++ 20 or later
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace PD {
// New Version of Smart CTOR
// Using as Template class
template <typename T>
class SmartCtor {
 public:
  /// @brief Type Reference
  using Ref = std::shared_ptr<T>;

  /// @brief Creates a New Shared Pointer
  /// @param ...args Arguments to forward
  /// @return Shared Pointer (Reference)
  template <typename... Args>
  static Ref New(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
};
/// @brief Wrapper for SmartCtor<Type>::New
/// @tparam T class type
/// @param ...args Arguments
/// @return SmartCtor<T>::Ref type reference
/// @note Not sure if this is solving the problem or
/// if I schould switch back to the macro
template <typename T, typename... Args>
SmartCtor<T>::Ref New(Args&&... args) {
  return SmartCtor<T>::New(std::forward<Args>(args)...);
}
// Defines
using u64 = unsigned long long;
using u32 = unsigned int;
using u16 = unsigned short;
using u8 = unsigned char;

namespace LibInfo {
const std::string CompiledWith();
const std::string CxxVersion();
const std::string BuildTime();
const std::string Version();
const std::string Commit();
}  // namespace LibInfo
}  // namespace PD