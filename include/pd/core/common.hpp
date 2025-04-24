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
#include <stack>
#include <string>
#include <vector>

// Platform API
#include <pd/core/pd_p_api.hpp>

// Legacy Smart Pointer
#define PD_SMART_CTOR(x)                                      \
  using Ref = std::shared_ptr<x>;                             \
  template <typename... args>                                 \
  static Ref New(args&&... cargs) {                           \
    return std::make_shared<x>(std::forward<args>(cargs)...); \
  }

namespace PD {
/**
 * SmartCtor (std::shared_ptr) Template class for Smart Pointers
 *
 * - Just add : public PD::SmartCtor<YourClass> to your class
 * @tparam T Your Class
 */
template <typename T>
class SmartCtor {
 public:
  /** Reference alias for std::shared_ptr<Type> */
  using Ref = std::shared_ptr<T>;

  /**
   * static Function to Create a New Reference
   * @param args Additional Arguments (Depends on your classes Constructors)
   * @return New Reference Object
   */
  template <typename... Args>
  static Ref New(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
};
/**
 * Wrapper for SmartCtor<Type>::New(Args)
 * @tparam T Class Type
 * @param args Arguments
 * @return Type Reference (SmartPointer)
 */
template <typename T, typename... Args>
SmartCtor<T>::Ref New(Args&&... args) {
  return SmartCtor<T>::New(std::forward<Args>(args)...);
}
// Defines

/** alias for 64 Bit unsigned integer */
using u64 = unsigned long long;
/** alias for 32 Bit unsigned integer */
using u32 = unsigned int;
/** alias for 16 Bit unsigned integer */
using u16 = unsigned short;
/** alias for 8 Bit unsigned integer */
using u8 = unsigned char;

/**
 * LinInfo Compile Information
 */
namespace LibInfo {
/**
 * Get the Compiler Name and Version the lib got Compiled with
 * @return Compiler Name / Version
 */
PD_CORE_API const std::string CompiledWith();
/**
 * Get the C++ Version used to compile the lib
 * @return C++ Version (__cplusplus)
 */
PD_CORE_API const std::string CxxVersion();
/**
 * Get the Buildtime of the Library
 * @return Build Time
 */
PD_CORE_API const std::string BuildTime();
/**
 * Get the Library Version
 * @return Library Version String
 */
PD_CORE_API const std::string Version();
/**
 * Get the Git Commit the Lib got compiled in
 * @return Git Commit 7digit short hash
 */
PD_CORE_API const std::string Commit();
/**
 * Get the Git Branch which was active when compiling the lib
 * @return Git Branch
 */
PD_CORE_API const std::string Branch();
}  // namespace LibInfo
}  // namespace PD