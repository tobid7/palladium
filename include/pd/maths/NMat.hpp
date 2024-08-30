#pragma once

#include <pd/maths/NVec.hpp>

class NMat4 {
 public:
  NMat4() { Zeros(); }
  ~NMat4() {}

  void Zeros() { memset(this, 0, sizeof(*this)); }
  void Identity() { Diagonal(NVec4(1.f, 1.f, 1.f, 1.f)); }
  void Diagonal(NVec4 xyzw) {
    Zeros();
    r[0][0] = xyzw[0];
    r[1][1] = xyzw[1];
    r[2][2] = xyzw[2];
    r[3][3] = xyzw[3];
  }
  NMat4& Get() { return *this; }
  void Add(const NMat4& in) {
    for (int i = 0; i < 0x10; i++) {
      m[i] += in[i];
    }
  }
  void Sub(const NMat4& in) {
    for (int i = 0; i < 0x10; i++) {
      m[i] -= in[i];
    }
  }
  void Mul(const NMat4& in) {

  }

  void Transpose();
  float Inverse();

  // Operators
  float operator[](int i) const { return m[i]; }
  float& operator[](int i) { return m[i]; }

  union {
    NVec4 r[4];
    float m[0x10];
  };
};