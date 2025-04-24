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

#include <pd/core/common.hpp>
#include <pd/core/vec3.hpp>

namespace PD {
template <typename T>
class vec4 {
 public:
  // SECTION: Constructors //

  vec4() = default;
  vec4(T v) {
    x = v;
    y = v;
    z = v;
    w = v;
  }
  vec4(T x, T y, T z, T w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
  }

  vec4(const vec4 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
  }

  vec4(const vec2<T> &xy, const vec2<T> &zw) {
    x = xy.x;
    y = xy.y;
    z = zw.x;
    w = zw.y;
  }

  // SECTION: Operators //

  // ADD //

  vec4 &operator+=(T v) {
    x += v;
    y += v;
    z += v;
    w += v;
    return *this;
  }
  vec4 &operator+=(const vec4<T> &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }
  vec4 operator+(T v) const { return vec4<T>(x + v, y + v, z + v, w + v); }
  vec4 operator+(vec4 v) const {
    return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
  }

  // SUB //

  vec4 &operator-=(T v) {
    x -= v;
    y -= v;
    z -= v;
    w -= v;
    return *this;
  }
  vec4 &operator-=(const vec4 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }
  vec4 operator-(T v) const { return vec4(x - v, y - v, z - v, w - v); }
  vec4 operator-(vec4 v) const {
    return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
  }

  // MUL //

  vec4 &operator*=(T v) {
    x *= v;
    y *= v;
    z *= v;
    w *= v;
    return *this;
  }
  vec4 &operator*=(const vec4 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
  }
  vec4 operator*(T v) const { return vec4(x * v, y * v, z * v, w * v); }
  vec4 operator*(vec4 v) const {
    return vec4(x * v.x, y * v.y, z * v.z, w * v.w);
  }

  // DIV //

  vec4 &operator/=(T v) {
    x /= v;
    y /= v;
    z /= v;
    w /= v;
    return *this;
  }
  vec4 &operator/=(const vec4 &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
    return *this;
  }
  vec4 operator/(T v) const { return vec4(x / v, y / v, z / v, w / v); }
  vec4 operator/(vec4 v) const {
    return vec4(x / v.x, y / v.y, z / v.z, w / v.w);
  }

  // Make Negative //
  vec4 operator-() const { return vec4(-x, -y, -z, -w); }

  bool operator==(const vec4 &v) const {
    return x == v.x && y == v.y && z == v.z && w == v.w;
  }
  bool operator!=(const vec4 &v) const { return !(*this == v); }

  // SECTION: Additional Functions //

  float Len() const { return sqrt(SqLen()); }
  float SqLen() const { return x * x + y * y + z * z + w * w; }
  void SwapXY() {
    T t = x;
    x = y;
    y = t;
  }
  void SwapYZ() {
    T t = z;
    z = y;
    y = t;
  }
  void SwapXZ() {
    T t = z;
    z = x;
    x = t;
  }
  // Adding ZW (to lazy to add all of those yet)
  void SwapZW() {
    T t = w;
    w = z;
    z = t;
  }

  // SECTION: DATA //
  T x = 0;
  T y = 0;
  T z = 0;
  T w = 0;
};

using dvec4 = vec4<double>;
using fvec4 = vec4<float>;
using ivec4 = vec4<int>;
}  // namespace PD