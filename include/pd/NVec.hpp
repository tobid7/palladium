#pragma once

struct NVec2 {
  // Init Funcs
  NVec2() : x(0), y(0) {}
  NVec2(float i0, float i1) : x(i0), y(i1) {}
  NVec2(const NVec2 &i) {
    x = i.x;
    y = i.y;
  }

  // Operators
  // Add
  NVec2 &operator+=(const NVec2 &i) {
    x += i.x;
    y += i.y;
    return *this;
  }

  NVec2 operator+(const NVec2 &i) const { return NVec2(x + i.x, y + i.y); }

  // Sub
  NVec2 &operator-=(const NVec2 &i) {
    x -= i.x;
    y -= i.y;
    return *this;
  }

  NVec2 operator-(const NVec2 &i) const { return NVec2(x - i.x, y - i.y); }

  // Compare
  bool operator==(const NVec2 &in) const { return x == in.x && y == in.y; }

  bool operator!=(const NVec2 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }
  // Internal Values
  float x;
  float y;
};

struct NVec4 {
  // Init Funcs
  NVec4() : x(0), y(0), z(0), w(0) {}
  NVec4(float i0, float i1, float i2, float i3) : x(i0), y(i1), z(i2), w(i3) {}
  NVec4(const NVec4 &i) {
    x = i.x;
    y = i.y;
    z = i.z;
    w = i.w;
  }

  NVec4(const NVec2 &i0, const NVec2 &i1) {
    x = i0.x;
    y = i0.y;
    z = i1.x;
    w = i1.y;
  }

  // Operators
  // Add
  NVec4 &operator+=(const NVec4 &i) {
    x += i.x;
    y += i.y;
    z += i.z;
    w += i.w;
    return *this;
  }

  NVec4 operator+(const NVec4 &i) const {
    return NVec4(x + i.x, y + i.y, z + i.z, w + i.w);
  }

  // Sub
  NVec4 &operator-=(const NVec4 &i) {
    x -= i.x;
    y -= i.y;
    z -= i.z;
    w -= i.w;
    return *this;
  }

  NVec4 operator-(const NVec4 &i) const {
    return NVec4(x - i.x, y - i.y, z - i.z, w - i.w);
  }

  // Compare
  bool operator==(const NVec4 &in) const {
    return x == in.x && y == in.y && z == in.z && w == in.w;
  }

  bool operator!=(const NVec4 &in) const {
    // use the first comparefuncs result
    // and swap it lol
    return !(*this == in);
  }
  // Internal Values
  float x;
  float y;
  float z;
  float w;
};