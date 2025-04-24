/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/color.hpp>

namespace PD {
// The Solution of the biggest performance issue
// A Simple Lookup table
static const std::map<char, int> HEX_DEC = {
    {'0', 0},  {'1', 1},  {'2', 2},  {'3', 3},  {'4', 4},  {'5', 5},
    {'6', 6},  {'7', 7},  {'8', 8},  {'9', 9},  {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15}, {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}};

PD_CORE_API Color& Color::Hex(const std::string& hex) {
#ifdef PD_NO_SAFE_CODE
  /// Safetey check (not required if you programm well xd)
  if (hex.length() != 7 || hex.length() != 9 || hex.length() != 6 ||
      hex.length() != 8 || std::find_if(hex.begin(), hex.end(), [](char c) {
                             return !std::isxdigit(c);
                           }) != hex.end()) {
    return *this;
  }
#endif
  int offset = ((hex.length() == 7 || hex.length() == 9) ? 1 : 0);
  m_r = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  m_g = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  m_b = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  offset += 2;
  if (hex.length() == 9) {
    m_a = HEX_DEC.at(hex[offset]) * 16 + HEX_DEC.at(hex[offset + 1]);
  } else {
    m_a = 255;
  }
  return *this;
}

PD_CORE_API std::string Color::Hex(bool rgba) const {
  std::stringstream s;
  s << "#";
  s << std::hex << std::setw(2) << std::setfill('0') << m_r;
  s << std::hex << std::setw(2) << std::setfill('0') << m_g;
  s << std::hex << std::setw(2) << std::setfill('0') << m_b;
  if (rgba) {
    s << std::hex << std::setw(2) << std::setfill('0') << m_a;
  }
  return s.str();
}
}  // namespace PD