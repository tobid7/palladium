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
 * Set of String Utillity Functions
 */
namespace Strings {
constexpr int HexChar2Int(char c) {
  /** Imagine man hat ne lookup table dafür verwendet :/ */
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  return -1;  // Error
}
/**
 * Check if a String ends with a specific extension
 * @param str Input string
 * @param exts List of Extensions to check for
 * @return true if one of the extensions is found in the String
 */
PD_API bool StringEndsWith(const std::string& str,
                           const std::vector<std::string>& exts);
/**
 * Function to Create a wstring of a string
 * @param s Input String to Convert
 * @return Result wstring
 * @note Returns Empty if it has an error
 */
PD_API std::wstring MakeWstring(const std::string& s);
/**
 * Generate a Formatted String by an Nanoseconds Input
 * @param nanos Nanoseconds Input
 * @return Result String
 */
PD_API const std::string FormatNanos(unsigned long long nanos);
/**
 * Generate a Formatted String by an Milliseconds Input
 * @param millis Milliseconds Input
 * @return Result String
 */
PD_API const std::string FormatMillis(unsigned long long millis);
/**
 * Create a formatted String by an input bytes value
 * @param bytes value in bytes
 * @result Formatted String for example `2.5MB`
 */
PD_API const std::string FormatBytes(unsigned long long bytes);
/**
 * Extract the Filename out of a Path
 * @param path Path to extract from
 * @param saperators Path Split Chars
 * @return extracted filename
 */
PD_API const std::string GetFileName(const std::string& path,
                                     const std::string& saperators = "/\\");
/**
 * Remove Extension from a Path / Filename
 * @param path Input Path
 * @return Path without Extension
 */
PD_API const std::string PathRemoveExtension(const std::string& path);
/**
 * Function to Convert a Type to a hex value
 * @tparam T Type
 * @param v value
 * @return hex string beginning with 0x
 */
template <typename T>
inline const std::string ToHex(const T& v) {
  std::stringstream s;
  s << "0x" << std::setfill('0') << std::setw(sizeof(v) * 2) << std::hex << v;
  return s.str();
}
/**
 * Generate a Hash out of a string
 * @param s String to hash
 * @return 32Bit Hash
 */
PD_API u32 FastHash(const std::string& s);
/**
 * Function to Generate a Compiler Name and Version String
 * Based on their Macros
 * @return CompilerName: Version
 */
inline const std::string GetCompilerVersion() {
  /// As the function looks like this Project is meant to
  /// Be ported to other systems as well
  std::stringstream res;
#ifdef __clang__  // Check clang first
  res << "Clang: " << __clang_major__ << ".";
  res << __clang_minor__ << ".";
  res << __clang_patchlevel__;
#elif __GNUC__
  res << "GCC: " << __GNUC__;
  res << "." << __GNUC_MINOR__ << ".";
  res << __GNUC_PATCHLEVEL__;
#elif _MSC_VER
  res << "MSVC: " << _MSC_VER;
#else
  res << "Unknown Compiler";
#endif
  return res.str();
}
}  // namespace Strings
class U8Iterator {
 public:
  explicit U8Iterator(const char* s) : ptr(reinterpret_cast<const u8*>(s)) {}
  ~U8Iterator() = default;

  bool Decode32(u32& ret) {
    if (ptr == nullptr || *ptr == 0) return false;
    u8 c = *ptr;
    if (c < 0x80) {
      ret = c;
      ptr += 1;
    } else if ((c >> 5) == 0x6) {
      ret = ((c & 0x1F) << 6) | (ptr[1] & 0x3F);
      ptr += 2;
    } else if ((c >> 4) == 0xE) {
      ret = ((c & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
      ptr += 3;
    } else {
      ret = ((c & 0x07) << 18) | ((ptr[1] & 0x3F) << 12) |
            ((ptr[2] & 0x3F) << 6) | (ptr[3] & 0x3F);
      ptr += 4;
    }
    return true;
  }

  bool PeekNext32(u32& ret) {
    if (ptr + 1 == nullptr || *ptr + 1 == 0) return false;
    u8 c = *ptr;
    if (c < 0x80) {
      ret = c;
    } else if ((c >> 5) == 0x6) {
      ret = ((c & 0x1F) << 6) | (ptr[1] & 0x3F);
    } else if ((c >> 4) == 0xE) {
      ret = ((c & 0x0F) << 12) | ((ptr[1] & 0x3F) << 6) | (ptr[2] & 0x3F);
    } else {
      ret = ((c & 0x07) << 18) | ((ptr[1] & 0x3F) << 12) |
            ((ptr[2] & 0x3F) << 6) | (ptr[3] & 0x3F);
    }
    return true;
  }

 private:
  const u8* ptr = nullptr;
};
}  // namespace PD