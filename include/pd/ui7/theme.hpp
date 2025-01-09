#pragma once

/*
MIT License
Copyright (c) 2024 René Amthor (tobid7)

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

using UI7Color = unsigned int;

enum UI7Color_ {
  UI7Color_Background,

};

namespace PD {
/// Using UI7Color as a Class to be able to
/// define it as struct as well as using it as enum
class UI7Color {
 public:
  UI7Color() {
    /// No Color
  }
  UI7Color(unsigned int c) { color = c; }
  UI7Color(UI7Color_ c) {}
  ~UI7Color() {}

  operator u32() const { return color; }

 private:
  u32 color;
};
}  // namespace PD