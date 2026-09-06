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

// This file is based on fvec4

#include <pd/common.hpp>
#include <pd/core/vec4.hpp>

namespace PD {
class fquat : public fvec4 {
  constexpr fquat() : fvec4(0.f, 0.f, 0.f, 1.f) {}
  constexpr fquat(float x, float y, float z, float w) : fvec4(x, y, z, w) {}
  constexpr fquat(const fvec4& v) : fvec4(v) {}

  static fquat Identity() { return fquat(0.f, 0.f, 0.f, 1.f); }

  constexpr fquat Conjugate() const { return fquat(-x, -y, -z, w); }
  fquat Inverse() const {
    float len = SqLen();
    if (len == 0.0f) {
      return fquat();
    }
    return Conjugate() / len;
  }

  fquat operator*(const fquat& v) const {
    return fquat(w * v.x + x * v.w + y * v.z - z * v.y,
                 w * v.y - x * v.z + y * v.w + z * v.x,
                 w * v.z + x * v.y - y * v.x + z * v.w,
                 w * v.w - x * v.x - y * v.y - z * v.z);
  }
};
}  // namespace PD