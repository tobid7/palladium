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

namespace PD {
template <typename T>
class vec2 {
 public:
  // SECTION: Constructors //

  vec2() = default;
  vec2(T v) {
    x = v;
    y = v;
  }
  vec2(T x, T y) {
    this->x = x;
    this->y = y;
  }

  vec2(const vec2<T> &v) {
    x = v.x;
    y = v.y;
  }

  // SECTION: Operators //

  // ADD //

  vec2 &operator+=(T v) {
    x += v;
    y += v;
    return *this;
  }
  vec2 &operator+=(const vec2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  vec2 operator+(T v) const { return vec2(x + v, y + v); }
  vec2 operator+(vec2 v) const { return vec2(x + v.x, y + v.y); }

  // SUB //

  vec2 &operator-=(T v) {
    x -= v;
    y -= v;
    return *this;
  }
  vec2 &operator-=(const vec2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  vec2 operator-(T v) const { return vec2(x - v, y - v); }
  vec2 operator-(vec2 v) const { return vec2(x - v.x, y - v.y); }

  // MUL //

  vec2 &operator*=(T v) {
    x *= v;
    y *= v;
    return *this;
  }
  vec2 &operator*=(const vec2 &v) {
    x *= v.x;
    y *= v.y;
    return *this;
  }
  vec2 operator*(T v) const { return vec2(x * v, y * v); }
  vec2 operator*(vec2 v) const { return vec2(x * v.x, y * v.y); }

  // DIV //

  vec2 &operator/=(T v) {
    x /= v;
    y /= v;
    return *this;
  }
  vec2 &operator/=(const vec2 &v) {
    x /= v.x;
    y /= v.y;
    return *this;
  }
  vec2 operator/(T v) const { return vec2(x / v, y / v); }
  vec2 operator/(vec2 v) const { return vec2(x / v.x, y / v.y); }

  // Make Negative //
  vec2 operator-() const { return vec2(-x, -y); }

  bool operator==(const vec2 &v) const { return x == v.x && y == v.y; }
  bool operator!=(const vec2 &v) const { return !(*this == v); }

  // SECTION: Additional Functions //

  float Len() const { return sqrt(SqLen()); }
  float SqLen() const { return x * x + y * y; }
  void Swap() {
    T t = x;
    x = y;
    y = t;
  }

  // SECTION: DATA //
  T x = 0;
  T y = 0;
};

using dvec2 = vec2<double>;
using fvec2 = vec2<float>;
using ivec2 = vec2<int>;
}  // namespace PD