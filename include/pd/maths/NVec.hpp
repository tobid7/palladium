#pragma once

#include <cmath>

struct NVec2 {
  // Init Funcs
  NVec2() {
    v[0] = 0;
    v[1] = 0;
  }
  NVec2(float x, float y) {
    v[0] = x;
    v[1] = y;
  }
  NVec2(const NVec2 &i) {
    v[0] = i[0];
    v[1] = i[1];
  }
  NVec2(float i) {
    v[0] = i;
    v[1] = i;
  }

  // Operations
  // Add
  NVec2 &operator+=(const NVec2 &i) {
    v[0] += i[0];
    v[1] += i[1];
    return *this;
  }

  NVec2 &operator+=(const float &i) {
    v[0] += i;
    v[1] += i;
    return *this;
  }

  NVec2 operator+(const NVec2 &i) const {
    return NVec2(v[0] + i[0], v[1] + i[1]);
  }
  NVec2 operator+(const float &i) const { return NVec2(v[0] + i, v[1] + i); }

  // Sub
  NVec2 &operator-=(const NVec2 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    return *this;
  }

  NVec2 &operator-=(const float &i) {
    v[0] -= i;
    v[1] -= i;
    return *this;
  }

  NVec2 operator-(const NVec2 &i) const {
    return NVec2(v[0] - i[0], v[1] - i[1]);
  }
  NVec2 operator-(const float &i) const { return NVec2(v[0] - i, v[1] - i); }

  // Mul
  NVec2 &operator*=(const NVec2 &i) {
    v[0] *= i[0];
    v[1] *= i[1];
    return *this;
  }

  NVec2 &operator*=(const float &i) {
    v[0] *= i;
    v[1] *= i;
    return *this;
  }

  NVec2 operator*(const NVec2 &i) const {
    return NVec2(v[0] * i[0], v[1] * i[1]);
  }
  NVec2 operator*(const float &i) const { return NVec2(v[0] * i, v[1] * i); }

  // Div
  NVec2 &operator/=(const NVec2 &i) {
    v[0] /= i[0];
    v[1] /= i[1];
    return *this;
  }

  NVec2 &operator/=(const float &i) {
    v[0] /= i;
    v[1] /= i;
    return *this;
  }

  NVec2 operator/(const NVec2 &i) const {
    return NVec2(v[0] / i[0], v[1] / i[1]);
  }
  NVec2 operator/(const float &i) const { return NVec2(v[0] / i, v[1] / i); }

  // Compare
  bool operator==(const NVec2 &in) const {
    return v[0] == in[0] && v[1] == in[1];
  }

  bool operator!=(const NVec2 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  NVec2 operator-() const { return NVec2(-v[0], -v[1]); }
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

struct NVec3 {
  // Init Funcs
  NVec3() {
    v[0] = 0.f;
    v[1] = 0.f;
    v[2] = 0.f;
  }
  NVec3(float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }
  NVec3(const NVec3 &i) {
    v[0] = i[0];
    v[1] = i[1];
    v[2] = i[2];
  }
  NVec3(float i) {
    v[0] = i;
    v[1] = i;
    v[2] = i;
  }

  // Operations
  // Add
  NVec3 &operator+=(const NVec3 &i) {
    v[0] += i[0];
    v[1] += i[1];
    v[2] += i[2];
    return *this;
  }

  NVec3 &operator+=(const float &i) {
    v[0] += i;
    v[1] += i;
    v[2] += i;
    return *this;
  }

  NVec3 operator+(const NVec3 &i) const {
    return NVec3(v[0] + i[0], v[1] + i[1], v[2] + i[2]);
  }
  NVec3 operator+(const float &i) const {
    return NVec3(v[0] + i, v[1] + i, v[2] + i);
  }

  // Sub
  NVec3 &operator-=(const NVec3 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    v[2] -= i[2];
    return *this;
  }

  NVec3 &operator-=(const float &i) {
    v[0] -= i;
    v[1] -= i;
    v[2] -= i;
    return *this;
  }

  NVec3 operator-(const NVec3 &i) const {
    return NVec3(v[0] - i[0], v[1] - i[1], v[2] - i[2]);
  }
  NVec3 operator-(const float &i) const {
    return NVec3(v[0] - i, v[1] - i, v[2] - i);
  }

  // Mul
  NVec3 &operator*=(const NVec3 &i) {
    v[0] *= i[0];
    v[1] *= i[1];
    v[2] *= i[2];
    return *this;
  }

  NVec3 &operator*=(const float &i) {
    v[0] *= i;
    v[1] *= i;
    v[2] *= i;
    return *this;
  }

  NVec3 operator*(const NVec3 &i) const {
    return NVec3(v[0] * i[0], v[1] * i[1], v[2] * i[2]);
  }
  NVec3 operator*(const float &i) const {
    return NVec3(v[0] * i, v[1] * i, v[2] * i);
  }

  // Div
  NVec3 &operator/=(const NVec3 &i) {
    v[0] /= i[0];
    v[1] /= i[1];
    v[2] /= i[2];
    return *this;
  }

  NVec3 &operator/=(const float &i) {
    v[0] /= i;
    v[1] /= i;
    v[2] /= i;
    return *this;
  }

  NVec3 operator/(const NVec3 &i) const {
    return NVec3(v[0] / i[0], v[1] / i[1], v[2] / i[2]);
  }
  NVec3 operator/(const float &i) const {
    return NVec3(v[0] / i, v[1] / i, v[2] / i);
  }

  // Compare
  bool operator==(const NVec3 &in) const {
    return v[0] == in[0] && v[1] == in[1] && v[2] == v[2];
  }

  bool operator!=(const NVec3 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  // Base
  NVec3 operator-() const { return NVec3(-v[0], -v[1], -v[2]); }
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

  float v[3];
};

struct NVec4 {
  // Init Funcs
  NVec4() {
    v[0] = 0.f;
    v[1] = 0.f;
    v[2] = 0.f;
    v[3] = 0.f;
  }
  NVec4(float x, float y, float z, float w) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
  }
  NVec4(const NVec4 &i) {
    v[0] = i[0];
    v[1] = i[1];
    v[2] = i[2];
    v[3] = i[3];
  }

  NVec4(const NVec2 &i0, const NVec2 &i1) {
    v[0] = i0[0];
    v[1] = i0[1];
    v[2] = i1[0];
    v[3] = i1[1];
  }
  NVec4(float i) {
    v[0] = i;
    v[1] = i;
    v[2] = i;
    v[3] = i;
  }

  // Operators
  // Add
  NVec4 &operator+=(const NVec4 &i) {
    v[0] += i[0];
    v[1] += i[1];
    v[2] += i[2];
    v[3] += i[3];
    return *this;
  }

  NVec4 operator+(const NVec4 &i) const {
    return NVec4(v[0] + i[0], v[1] + i[1], v[2] + i[2], v[3] + i[3]);
  }

  // Sub
  NVec4 &operator-=(const NVec4 &i) {
    v[0] -= i[0];
    v[1] -= i[1];
    v[2] -= i[2];
    v[3] -= i[3];
    return *this;
  }

  NVec4 operator-(const NVec4 &i) const {
    return NVec4(v[0] - i[0], v[1] - i[1], v[2] - i[2], v[3] - i[3]);
  }

  // Compare
  bool operator==(const NVec4 &in) const {
    return v[0] == in[0] && v[1] == in[1] && v[2] == in[2] && v[3] == in[3];
  }

  bool operator!=(const NVec4 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }

  // Base
  NVec3 operator-() const { return NVec3(-v[0], -v[1], -v[2]); }
  float operator[](int i) const { return v[i]; }
  float &operator[](int i) { return v[i]; }

  float len() const { return sqrt(sqlen()); }
  float sqlen() const {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
  }

  // Vec Acess
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