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

#include <pd/core/color.hpp>

namespace PD {
PD_API std::string Color::Hex(bool rgba) const {
  /** Need to int cast (so it is used as num and not char...) */
  std::stringstream s;
  s << "#";
  s << std::hex << std::setw(2) << std::setfill('0') << (int)r;
  s << std::hex << std::setw(2) << std::setfill('0') << (int)g;
  s << std::hex << std::setw(2) << std::setfill('0') << (int)b;
  if (rgba || a != 255) {  // QoL change btw
    s << std::hex << std::setw(2) << std::setfill('0') << (int)a;
  }
  return s.str();
}
}  // namespace PD