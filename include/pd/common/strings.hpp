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

#include <pd/common/common.hpp>

namespace PD {
namespace Strings {
bool StringEndsWith(const std::string& str,
                    const std::vector<std::string>& exts);
std::wstring MakeWstring(const std::string& s);
const std::string FormatNanos(unsigned long long nanos);
const std::string FormatMillis(unsigned long long millis);
const std::string FormatBytes(unsigned long long bytes);
const std::string GetFileName(const std::string& path,
                              const std::string& saperators = "/\\");
const std::string PathRemoveExtension(const std::string& path);
template <typename T>
inline const std::string ToHex(const T& v) {
  std::stringstream s;
  s << "0x" << std::setfill('0') << std::setw(sizeof(v) * 2) << std::hex << v;
  return s.str();
}
u32 FastHash(const std::string& s);
inline const std::string GetCompilerVersion() {
  /// As the function looks like this Project is meant to
  /// Be ported to other systems as well
  std::stringstream res;
#ifdef __GNUC__
  res << "GCC: " << __GNUC__;
  res << "." << __GNUC_MINOR__ << ".";
  res << __GNUC_PATCHLEVEL__;
#elif __clang__
  res << "Clang: " << __clang_major__ << ".";
  res << __clang_minor__ << ".";
  res << __clang_patchlevel__;
#elif _MSC_VER
  res << "MSVC: " << _MSC_VER;
#else
  res << "Unknown Compiler";
#endif
  return res.str();
}
}  // namespace Strings
}  // namespace PD