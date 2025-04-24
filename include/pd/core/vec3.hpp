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
#include <pd/core/vec2.hpp>

namespace PD {
template <typename T>
class vec3 {
 public:
  // SECTION: Constructors //

  vec3() = default;
  vec3(T v) {
    x = v;
    y = v;
    z = v;
  }
  vec3(T x, T y, T z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  vec3(const vec3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  // SECTION: Operators //

  // ADD //

  vec3 &operator+=(T v) {
    x += v;
    y += v;
    z += v;
    return *this;
  }
  vec3 &operator+=(const vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  vec3 operator+(T v) const { return vec3(x + v, y + v, z + v); }
  vec3 operator+(vec3 v) const { return vec3(x + v.x, y + v.y, z + v.z); }

  // SUB //

  vec3 &operator-=(T v) {
    x -= v;
    y -= v;
    z -= v;
    return *this;
  }
  vec3 &operator-=(const vec3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  vec3 operator-(T v) const { return vec3(x - v, y - v, z - v); }
  vec3 operator-(vec3 v) const { return vec3(x - v.x, y - v.y, z - v.z); }

  // MUL //

  vec3 &operator*=(T v) {
    x *= v;
    y *= v;
    z *= v;
    return *this;
  }
  vec3 &operator*=(const vec3 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }
  vec3 operator*(T v) const { return vec3(x * v, y * v, z * v); }
  vec3 operator*(vec3 v) const { return vec3(x * v.x, y * v.y, z * v.z); }

  // DIV //

  vec3 &operator/=(T v) {
    x /= v;
    y /= v;
    z /= v;
    return *this;
  }
  vec3 &operator/=(const vec3 &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }
  vec3 operator/(T v) const { return vec3(x / v, y / v, z / v); }
  vec3 operator/(vec3 v) const { return vec3(x / v.x, y / v.y, z / v.z); }

  // Make Negative //
  vec3 operator-() const { return vec3(-x, -y, -z); }

  bool operator==(const vec3 &v) const {
    return x == v.x && y == v.y && z == v.z;
  }
  bool operator!=(const vec3 &v) const { return !(*this == v); }

  // SECTION: Additional Functions //

  float Len() const { return sqrt(SqLen()); }
  float SqLen() const { return x * x + y * y + z * z; }
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

  // SECTION: DATA //
  T x = 0;
  T y = 0;
  T z = 0;
};

using dvec3 = vec3<double>;
using fvec3 = vec3<float>;
using ivec3 = vec3<int>;
}  // namespace PD