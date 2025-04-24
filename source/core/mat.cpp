/*
MIT License

Copyright (c) 2024 - 2025 tobid7

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

#include <pd/core/mat.hpp>

namespace PD {
PD_CORE_API void Mat4::Zeros() {
  for (int i = 0; i < 16; i++) {
    m[i] = 0.0f;
  }
}

PD_CORE_API void Mat4::Ortho(float left, float right, float bottom, float top,
                             float near, float far) {
  m[0] = 2.0f / (right - left);
  m[1] = 0.0f;
  m[2] = 0.0f;
  m[3] = -(right + left) / (right - left);

  m[4] = 0.0f;
  m[5] = 2.0f / (top - bottom);
  m[6] = 0.0f;
  m[7] = -(top + bottom) / (top - bottom);

  m[8] = 0.0f;
  m[9] = 0.0f;
  m[10] = -2.0f / (far - near);
  m[11] = -(far + near) / (far - near);

  m[12] = 0.0f;
  m[13] = 0.0f;
  m[14] = 0.0f;
  m[15] = 1.0f;
}
}  // namespace PD