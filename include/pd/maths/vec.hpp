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

/**
 * Why Creating this:
 * Cause using makes coding much better structured
 * and easy to use like in glsl or glm
 */

#include <pd/common/common.hpp>

namespace PD {
struct vec2 {
  // Init Funcs
  vec2() {
    v[0] = 0;
    v[1] = 0;
  }
  vec2(float x, float y) {
    v[0] = x;
    v[1] = y;
  }
  vec2(const vec2 &i) {
    v[0] = i[0];
    v[1] = i[1];
  }
  vec2(float i) {
    v[0] = i;
    v[1] = i;
  }

  // Operations
  // Add
  vec2 &operator+=(const vec2 &i) {
    v[0] += i[0];
    v[1] += i[1];
    return *this;
  }

  vec2 &operator+=(const float &i) {
    v[0] += i;
    v[1] += i;
    return *this;
  }

  vec2 operator+(const vec2 &i) const { return vec2(v[0] + i[0], v[1] + i[1]); }
  vec2 operator+(const float &i) const { return vec2(v[0] + i, v[1] + i); }

  // Sub
  vec2 &operator-=(const vec2 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    return *this;
  }

  vec2 &operator-=(const float &i) {
    v[0] -= i;
    v[1] -= i;
    return *this;
  }

  vec2 operator-(const vec2 &i) const { return vec2(v[0] - i[0], v[1] - i[1]); }
  vec2 operator-(const float &i) const { return vec2(v[0] - i, v[1] - i); }

  // Mul
  vec2 &operator*=(const vec2 &i) {
    v[0] *= i[0];
    v[1] *= i[1];
    return *this;
  }

  vec2 &operator*=(const float &i) {
    v[0] *= i;
    v[1] *= i;
    return *this;
  }

  vec2 operator*(const vec2 &i) const { return vec2(v[0] * i[0], v[1] * i[1]); }
  vec2 operator*(const float &i) const { return vec2(v[0] * i, v[1] * i); }

  // Div
  vec2 &operator/=(const vec2 &i) {
    v[0] /= i[0];
    v[1] /= i[1];
    return *this;
  }

  vec2 &operator/=(const float &i) {
    v[0] /= i;
    v[1] /= i;
    return *this;
  }

  vec2 operator/(const vec2 &i) const { return vec2(v[0] / i[0], v[1] / i[1]); }
  vec2 operator/(const float &i) const { return vec2(v[0] / i, v[1] / i); }

  // Compare
  bool operator==(const vec2 &in) const {
    return v[0] == in[0] && v[1] == in[1];
  }

  bool operator!=(const vec2 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  vec2 operator-() const { return vec2(-v[0], -v[1]); }
  float operator[](int i) const { return v[i]; }
  float &operator[](int i) { return v[i]; }

  float len() const { return sqrt(sqlen()); }
  float sqlen() const { return v[0] * v[0] + v[1] * v[1]; }

  float x() const { return v[0]; }
  float &x() { return v[0]; }
  float y() const { return v[1]; }
  float &y() { return v[1]; }
  // Internal Values
  float v[2];
};

struct vec3 {
  // Init Funcs
  vec3() {
    v[0] = 0.f;
    v[1] = 0.f;
    v[2] = 0.f;
  }
  vec3(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  vec3(const vec3 &i) {
    v[0] = i[0];
    v[1] = i[1];
    v[2] = i[2];
  }
  vec3(float i) {
    v[0] = i;
    v[1] = i;
    v[2] = i;
  }
  //// PD REWRITE ADDITIONAL CONTENT ////
  vec3(const vec2 &xy, float z) {
    v[0] = xy[0];
    v[1] = xy[1];
    v[2] = z;
  }
  vec3(float x, const vec2 &yz) {
    v[0] = x;
    v[1] = yz[0];
    v[2] = yz[1];
  }

  // Operations
  // Add
  vec3 &operator+=(const vec3 &i) {
    v[0] += i[0];
    v[1] += i[1];
    v[2] += i[2];
    return *this;
  }

  vec3 &operator+=(const float &i) {
    v[0] += i;
    v[1] += i;
    v[2] += i;
    return *this;
  }

  vec3 operator+(const vec3 &i) const {
    return vec3(v[0] + i[0], v[1] + i[1], v[2] + i[2]);
  }
  vec3 operator+(const float &i) const {
    return vec3(v[0] + i, v[1] + i, v[2] + i);
  }

  // Sub
  vec3 &operator-=(const vec3 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    v[2] -= i[2];
    return *this;
  }

  vec3 &operator-=(const float &i) {
    v[0] -= i;
    v[1] -= i;
    v[2] -= i;
    return *this;
  }

  vec3 operator-(const vec3 &i) const {
    return vec3(v[0] - i[0], v[1] - i[1], v[2] - i[2]);
  }
  vec3 operator-(const float &i) const {
    return vec3(v[0] - i, v[1] - i, v[2] - i);
  }

  // Mul
  vec3 &operator*=(const vec3 &i) {
    v[0] *= i[0];
    v[1] *= i[1];
    v[2] *= i[2];
    return *this;
  }

  vec3 &operator*=(const float &i) {
    v[0] *= i;
    v[1] *= i;
    v[2] *= i;
    return *this;
  }

  vec3 operator*(const vec3 &i) const {
    return vec3(v[0] * i[0], v[1] * i[1], v[2] * i[2]);
  }
  vec3 operator*(const float &i) const {
    return vec3(v[0] * i, v[1] * i, v[2] * i);
  }

  // Div
  vec3 &operator/=(const vec3 &i) {
    v[0] /= i[0];
    v[1] /= i[1];
    v[2] /= i[2];
    return *this;
  }

  vec3 &operator/=(const float &i) {
    v[0] /= i;
    v[1] /= i;
    v[2] /= i;
    return *this;
  }

  vec3 operator/(const vec3 &i) const {
    return vec3(v[0] / i[0], v[1] / i[1], v[2] / i[2]);
  }
  vec3 operator/(const float &i) const {
    return vec3(v[0] / i, v[1] / i, v[2] / i);
  }

  // Compare
  bool operator==(const vec3 &in) const {
    return v[0] == in[0] && v[1] == in[1] && v[2] == v[2];
  }

  bool operator!=(const vec3 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  // Base
  vec3 operator-() const { return vec3(-v[0], -v[1], -v[2]); }
  float operator[](int i) const { return v[i]; }
  float &operator[](int i) { return v[i]; }

  float len() const { return sqrt(sqlen()); }
  float sqlen() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }

  float x() const { return v[0]; }
  float &x() { return v[0]; }
  float y() const { return v[1]; }
  float &y() { return v[1]; }
  float z() const { return v[2]; }
  float &z() { return v[2]; }

  //// PALLADIUM REWRITE ADDITIONAL CONTENT ////
  vec2 xy() const { return vec2(v[0], v[1]); }
  vec2 yz() const { return vec2(v[1], v[2]); }

  float v[3];
};

struct vec4 {
  // Init Funcs
  vec4() {
    v[0] = 0.f;
    v[1] = 0.f;
    v[2] = 0.f;
    v[3] = 0.f;
  }
  vec4(float x, float y, float z, float w) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
  }
  vec4(const vec4 &i) {
    v[0] = i[0];
    v[1] = i[1];
    v[2] = i[2];
    v[3] = i[3];
  }

  vec4(const vec2 &i0, const vec2 &i1) {
    v[0] = i0[0];
    v[1] = i0[1];
    v[2] = i1[0];
    v[3] = i1[1];
  }
  vec4(float i) {
    v[0] = i;
    v[1] = i;
    v[2] = i;
    v[3] = i;
  }

  // Operators
  // Add
  vec4 &operator+=(const vec4 &i) {
    v[0] += i[0];
    v[1] += i[1];
    v[2] += i[2];
    v[3] += i[3];
    return *this;
  }

  vec4 operator+(const vec4 &i) const {
    return vec4(v[0] + i[0], v[1] + i[1], v[2] + i[2], v[3] + i[3]);
  }

  // Sub
  vec4 &operator-=(const vec4 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    v[2] -= i[2];
    v[3] -= i[3];
    return *this;
  }

  vec4 operator-(const vec4 &i) const {
    return vec4(v[0] - i[0], v[1] - i[1], v[2] - i[2], v[3] - i[3]);
  }

  // Compare
  bool operator==(const vec4 &in) const {
    return v[0] == in[0] && v[1] == in[1] && v[2] == in[2] && v[3] == in[3];
  }

  bool operator!=(const vec4 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  // Base
  vec3 operator-() const { return vec3(-v[0], -v[1], -v[2]); }
  float operator[](int i) const { return v[i]; }
  float &operator[](int i) { return v[i]; }

  float len() const { return sqrt(sqlen()); }
  float sqlen() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
  }

  // Vec2 Acess
  float x() const { return v[0]; }
  float &x() { return v[0]; }
  float y() const { return v[1]; }
  float &y() { return v[1]; }
  float z() const { return v[2]; }
  float &z() { return v[2]; }
  float w() const { return v[3]; }
  float &w() { return v[3]; }
  // Quaternion Acess
  float r() const { return v[0]; }
  float &r() { return v[0]; }
  float k() const { return v[1]; }
  float &k() { return v[1]; }
  float j() const { return v[2]; }
  float &j() { return v[2]; }
  float i() const { return v[3]; }
  float &i() { return v[3]; }
  // Internal Values
  float v[4];
};
}  // namespace PD