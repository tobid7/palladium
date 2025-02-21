#pragma once

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

#include <pd/core/common.hpp>
#include <pd/maths/vec.hpp>

namespace PD {
namespace LI {
class Vertex {
 public:
  Vertex() {}
  Vertex(const vec2& p, const vec2& u, u32 c) {
    pos[0] = p[0];
    pos[1] = p[1];
    uv = u;
    color = c;
  }
  ~Vertex() {}

  Vertex& Pos(const vec2& v) {
    pos = v;
    return *this;
  }
  Vertex& Uv(const vec2& v) {
    uv = v;
    return *this;
  }
  Vertex& Color(u32 v) {
    color = v;
    return *this;
  }

  // private:
  vec2 pos;
  vec2 uv;
  u32 color;
};
}  // namespace LI
}  // namespace PD